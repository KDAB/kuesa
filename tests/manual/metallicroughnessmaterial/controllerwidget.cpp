/*
    controllerwidget.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Kevin Ottens <kevin.ottens@kdab.com>

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

#include "controllerwidget.h"
#include "ui_controllerwidget.h"
#include "scenecontroller.h"

#include <QTransform>

ControllerWidget::ControllerWidget(SceneController *sceneController,
                                   QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControllerWidget)
    , m_controller(sceneController)
{
    Q_ASSERT(m_controller);
    ui->setupUi(this);

    connect(ui->baseColorRedFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onBaseColorComponentChanged);
    connect(ui->baseColorGreenFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onBaseColorComponentChanged);
    connect(ui->baseColorBlueFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onBaseColorComponentChanged);

    connect(ui->emissiveRedFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onEmissiveFactorComponentChanged);
    connect(ui->emissiveGreenFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onEmissiveFactorComponentChanged);
    connect(ui->emissiveBlueFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onEmissiveFactorComponentChanged);

    connect(this, &ControllerWidget::baseColorChanged,
            m_controller, &SceneController::setBaseColorFactor);
    connect(ui->useBaseColorMap, &QCheckBox::toggled,
            m_controller, &SceneController::setUseBaseColorMap);
    connect(ui->metallicFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_controller, &SceneController::setMetallicFactor);
    connect(ui->roughnessFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_controller, &SceneController::setRoughnessFactor);
    connect(ui->useMetalRoughMap, &QCheckBox::toggled,
            m_controller, &SceneController::setUseMetalRoughMap);
    connect(ui->normalScale, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_controller, &SceneController::setNormalScale);
    connect(ui->useNormalMap, &QCheckBox::toggled,
            m_controller, &SceneController::setUseNormalMap);
    connect(ui->useAmbientOcclusionMap, &QCheckBox::toggled,
            m_controller, &SceneController::setUseAmbientOcclusionMap);
    connect(this, &ControllerWidget::emissiveFactorChanged,
            m_controller, &SceneController::setEmissiveFactor);
    connect(ui->useEmissiveMap, &QCheckBox::toggled,
            m_controller, &SceneController::setUseEmissiveMap);
    connect(ui->textureScale, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onTextureTransformChanged);
    connect(ui->textureOffsetX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onTextureTransformChanged);
    connect(ui->textureOffsetY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onTextureTransformChanged);
    connect(ui->textureRotationZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ControllerWidget::onTextureTransformChanged);
    connect(ui->useColorAttribute, &QCheckBox::toggled,
            m_controller, &SceneController::setUseColorAttribute);
    connect(ui->doubleSided, &QCheckBox::toggled,
            m_controller, &SceneController::setDoubleSided);
    connect(ui->toneMapComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx) {
                if (idx >= 0 && idx < ui->toneMapComboBox->count())
                    m_controller->setToneMappingAlgorithm(
                            static_cast<Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping>(ui->toneMapComboBox->itemData(idx).toInt()));
            });

    ui->toneMapComboBox->addItem(QStringLiteral("None"), QVariant::fromValue(Kuesa::ToneMappingAndGammaCorrectionEffect::None));
    ui->toneMapComboBox->addItem(QStringLiteral("Reinhard"), QVariant::fromValue(Kuesa::ToneMappingAndGammaCorrectionEffect::Reinhard));
    ui->toneMapComboBox->addItem(QStringLiteral("Filmic"), QVariant::fromValue(Kuesa::ToneMappingAndGammaCorrectionEffect::Filmic));
}

ControllerWidget::~ControllerWidget()
{
    delete ui;
}

void ControllerWidget::onBaseColorComponentChanged()
{
    emit baseColorChanged(QColor::fromRgbF(ui->baseColorRedFactor->value(),
                                           ui->baseColorGreenFactor->value(),
                                           ui->baseColorBlueFactor->value()));
}

void ControllerWidget::onEmissiveFactorComponentChanged()
{
    emit emissiveFactorChanged(QColor::fromRgbF(ui->emissiveRedFactor->value(),
                                                ui->emissiveGreenFactor->value(),
                                                ui->emissiveBlueFactor->value()));
}

void ControllerWidget::onTextureTransformChanged()
{
    const auto scale = ui->textureScale->value();
    const auto offsetX = ui->textureOffsetX->value();
    const auto offsetY = ui->textureOffsetY->value();
    const auto rotationZ = ui->textureRotationZ->value();

    const auto transform = [=] {
        auto t = QTransform();
        t.scale(scale, scale);
        t.rotate(rotationZ);
        t.translate(offsetX, offsetY);

        auto m = QMatrix3x3();
        m(0, 0) = static_cast<float>(t.m11());
        m(0, 1) = static_cast<float>(t.m21());
        m(0, 2) = static_cast<float>(t.m31());
        m(1, 0) = static_cast<float>(t.m12());
        m(1, 1) = static_cast<float>(t.m22());
        m(1, 2) = static_cast<float>(t.m32());
        m(2, 0) = static_cast<float>(t.m13());
        m(2, 1) = static_cast<float>(t.m23());
        m(2, 2) = static_cast<float>(t.m33());
        return m;
    }();

    m_controller->setTextureTransform(transform);
}
