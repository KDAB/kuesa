/*
    materialwidget.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "materialwidget.h"
#include "ui_materialwidget.h"
#include "materialinspector.h"
#include <Kuesa/UnlitMaterial>
#include <Qt3DRender/qabstracttexture.h>
#include <QQmlContext>

MaterialWidget::MaterialWidget(QWidget *parent)
    : QWidget(parent)
    , m_inspector(nullptr)
    , m_ui(new Ui::MaterialWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->baseColorMapValue, &QLabel::linkActivated, this, &MaterialWidget::showAsset);
    connect(m_ui->metallicColorMapValue, &QLabel::linkActivated, this, &MaterialWidget::showAsset);
    connect(m_ui->normalMapValue, &QLabel::linkActivated, this, &MaterialWidget::showAsset);
    connect(m_ui->emissiveMapValue, &QLabel::linkActivated, this, &MaterialWidget::showAsset);
    connect(m_ui->ambientOcclusionMapValue, &QLabel::linkActivated, this, &MaterialWidget::showAsset);
}

MaterialWidget::~MaterialWidget()
{
    delete m_ui;
}

void MaterialWidget::setMaterialInspector(MaterialInspector *inspector)
{
    m_inspector = inspector;
    connect(m_inspector, &MaterialInspector::materialPropertiesChanged, this, &MaterialWidget::updateData);
}

void MaterialWidget::setPreviewRenderContext(QQmlContext *context)
{
    context->setContextProperty("_materialCaptureReceiver", m_ui->previewLabel);
    context->setContextProperty("_materialInspector", m_inspector);
}

void MaterialWidget::updateData()
{
    const bool isUnlit = qobject_cast<Kuesa::UnlitMaterial *>(m_inspector->material()) != nullptr;

    m_ui->unlitGroupBox->setVisible(isUnlit);
    m_ui->metallicRoughnessGroupBox->setVisible(!isUnlit);
    m_ui->emissiveGroupBox->setVisible(!isUnlit);
    m_ui->normalGroupBox->setVisible(!isUnlit);
    m_ui->occlusionGroupBox->setVisible(!isUnlit);

    if (isUnlit) {
        m_ui->unlitBaseColorFactorValue->setColor(m_inspector->baseColor());
        setTextureLabelState(m_ui->unlitBaseColorMapLabel, m_ui->unlitBaseColorMapValue, m_inspector->baseColorMap());
    } else {
        m_ui->baseColorFactorValue->setColor(m_inspector->baseColor());
        setTextureLabelState(m_ui->baseColorMapLabel, m_ui->baseColorMapValue, m_inspector->baseColorMap());
        m_ui->metallicFactorValue->setText(QString::number(m_inspector->metallicFactor(), 'f', 3));
        m_ui->roughnessFactorValue->setText(QString::number(m_inspector->roughnessFactor(), 'f', 3));
        setTextureLabelState(m_ui->MetallicColorMapLabel, m_ui->metallicColorMapValue, m_inspector->metalRoughMap());

        const bool hasNormalMap = m_inspector->normalMap();
        m_ui->normalGroupBox->setVisible(hasNormalMap);
        if (hasNormalMap) {
            m_ui->normalScaleValue->setText(QString::number(m_inspector->normalScale(), 'f', 4));
            setTextureLabelState(m_ui->NormalMapLabel, m_ui->normalMapValue, m_inspector->normalMap());
        }

        const bool hasOcclusionMap = m_inspector->ambientOcclusionMap();
        m_ui->occlusionGroupBox->setVisible(hasOcclusionMap);
        if (hasOcclusionMap) {
            setTextureLabelState(m_ui->AmbientOcclusionMapLabel, m_ui->ambientOcclusionMapValue, m_inspector->ambientOcclusionMap());
        }

        m_ui->emissiveFactorValue->setColor(m_inspector->emissiveFactor());
        setTextureLabelState(m_ui->emissiveMapLabel, m_ui->emissiveMapValue, m_inspector->emissiveMap());
    }

    m_ui->textureTransformLabel->setMatrix(m_inspector->textureTransform());
    m_ui->doubleSidedValue->setText(boolTextValue(m_inspector->doubleSided()));
    m_ui->opaqueValue->setText(boolTextValue(m_inspector->opaque()));
}

QString MaterialWidget::boolTextValue(bool value) const
{
    return value ? tr("True") : tr("False");
}

void MaterialWidget::setTextureLabelState(QLabel *nameLabel, QLabel *valueLabel, Qt3DRender::QAbstractTexture *texture) const
{
    nameLabel->setHidden(texture == nullptr);
    valueLabel->setHidden(texture == nullptr);
    if (texture)
        valueLabel->setText(QString::fromLatin1("<a href=\"%1\">%2</a>").arg(texture->objectName(), texture->objectName()));
}

MatrixLabel::MatrixLabel(QWidget *parent)
    : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setColumnStretch(4, 1);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 9; ++i)
        m_gridLayout->addWidget(new QLabel, i / 3, i % 3);
}

void MatrixLabel::setMatrix(const QMatrix3x3 &matrix)
{
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            auto label = qobject_cast<QLabel *>(m_gridLayout->itemAtPosition(row, col)->widget());
            Q_ASSERT(label);
            if (matrix.isIdentity()) {
                if (row == 0 && col == 0)
                    label->setText(tr("None"));
                else
                    label->hide();
            } else {
                label->setText(QString::number(matrix(row, col), 'f', 4));
                label->show();
            }
        }
    }
}
