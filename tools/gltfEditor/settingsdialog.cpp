/*
    settingsdialog.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "mainwindow.h"

#include <QPainter>
#include <QColorDialog>

namespace {

void setButtonColor(QAbstractButton *button, QColor c)
{
    QPixmap px(16, 16);
    QPainter p(&px);
    p.fillRect(0, 0, 16, 16, c);
    button->setIcon(QIcon(px));
}

} // namespace

SettingsDialog::SettingsDialog(MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , m_generateTangents(false)
    , m_generateNormals(false)
    , m_gamma(2.2f)
    , m_exposure(0.0f)
    , m_toneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::None)
{
    ui->setupUi(this);
    connect(ui->selectionColorButton, &QToolButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(m_selectionColor, this, tr("Pick Selection Color"));
        setSelectionColor(c);
    });
    connect(ui->customClearColorButton, &QToolButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(m_selectionColor, this, tr("Pick Selection Color"));
        setClearColor(c);
    });
    connect(ui->defaultClearColor, &QRadioButton::toggled, this, [this](bool checked) {
        m_defaultClearColor = checked;
        emit defaultClearColorChanged(m_defaultClearColor);
    });
    connect(ui->generateTangentsCB, &QRadioButton::toggled, this, [this](bool checked) {
        m_generateTangents = checked;
        emit generateTangentsChanged(m_generateTangents);
    });
    connect(ui->generateNormalsCB, &QRadioButton::toggled, this, [this](bool checked) {
        m_generateNormals = checked;
        emit generateNormalsChanged(m_generateNormals);
    });
    connect(ui->gammaSlider, &QAbstractSlider::valueChanged, this, [this](int value) {
        // Bring slider Range -6 - 6 to -1 - 1 range
        m_gamma = 2.2f + float(value) / 6.0f;
        emit gammaChanged(m_gamma);
    });
    connect(this, &SettingsDialog::gammaChanged, this, [this]() {
        ui->gammaLabel->setText(QStringLiteral("Gamma(%1):)").arg(m_gamma, 0, 'f', 1));
    });
    connect(ui->exposureSlider, &QAbstractSlider::valueChanged, this, [this](int value) {
        // Bring slider Range -6 - 6 to -3 - 3 range
        m_exposure = float(value) / 2.0f;
        emit exposureChanged(m_exposure);
    });
    connect(this, &SettingsDialog::exposureChanged, this, [this]() {
        ui->exposureLabel->setText(QStringLiteral("Exposure(%1):)").arg(m_exposure, 0, 'f', 1));
    });
    connect(ui->toneMappingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx) {
                if (idx >= 0 && idx < ui->toneMappingCombo->count()) {
                    m_toneMappingAlgorithm = static_cast<Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping>(ui->toneMappingCombo->itemData(idx).toInt());
                    emit toneMappingAlgorithmChanged(m_toneMappingAlgorithm);
                }
            });

    ui->toneMappingCombo->addItem(QStringLiteral("None"), QVariant::fromValue(Kuesa::ToneMappingAndGammaCorrectionEffect::None));
    ui->toneMappingCombo->addItem(QStringLiteral("Reinhard"), QVariant::fromValue(Kuesa::ToneMappingAndGammaCorrectionEffect::Reinhard));
    ui->toneMappingCombo->addItem(QStringLiteral("Filmic"), QVariant::fromValue(Kuesa::ToneMappingAndGammaCorrectionEffect::Filmic));
    ui->toneMappingCombo->addItem(QStringLiteral("Uncharted"), QVariant::fromValue(Kuesa::ToneMappingAndGammaCorrectionEffect::Uncharted));

    // Initial UI update
    ui->exposureLabel->setText(QStringLiteral("Exposure(%1):)").arg(m_exposure, 0, 'f', 1));
    ui->gammaLabel->setText(QStringLiteral("Gamma(%1):)").arg(m_gamma, 0, 'f', 1));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

QColor SettingsDialog::selectionColor() const
{
    return m_selectionColor;
}

QColor SettingsDialog::clearColor() const
{
    return m_clearColor;
}

bool SettingsDialog::defaultClearColor() const
{
    return m_defaultClearColor;
}

bool SettingsDialog::generateTangents() const
{
    return m_generateTangents;
}

bool SettingsDialog::generateNormals() const
{
    return m_generateNormals;
}

float SettingsDialog::gamma() const
{
    return m_gamma;
}

float SettingsDialog::exposure() const
{
    return m_exposure;
}

Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping SettingsDialog::toneMappingAlgorithm() const
{
    return m_toneMappingAlgorithm;
}

void SettingsDialog::setSelectionColor(QColor selectionColor)
{
    if (m_selectionColor == selectionColor)
        return;

    m_selectionColor = selectionColor;
    emit selectionColorChanged(m_selectionColor);
    setButtonColor(ui->selectionColorButton, m_selectionColor);
}

void SettingsDialog::setClearColor(QColor clearColor)
{
    if (m_clearColor == clearColor)
        return;

    m_clearColor = clearColor;
    emit clearColorChanged(m_clearColor);
    setButtonColor(ui->customClearColorButton, m_clearColor);
}

void SettingsDialog::setDefaultClearColor(bool defaultClearColor)
{
    if (m_defaultClearColor != defaultClearColor) {
        m_defaultClearColor = defaultClearColor;
        emit defaultClearColorChanged(m_defaultClearColor);
        if (m_defaultClearColor)
            ui->defaultClearColor->setChecked(true);
        else
            ui->customClearColor->setChecked(true);
    }
}

void SettingsDialog::setGenerateTangents(bool generateTangents)
{
    if (m_generateTangents != generateTangents) {
        m_generateTangents = generateTangents;
        emit generateTangentsChanged(m_generateTangents);
        ui->generateTangentsCB->setChecked(m_generateTangents);
    }
}

void SettingsDialog::setGenerateNormals(bool generateNormals)
{
    if (m_generateNormals != generateNormals) {
        m_generateNormals = generateNormals;
        emit generateNormalsChanged(m_generateNormals);
        ui->generateNormalsCB->setChecked(m_generateNormals);
    }
}

void SettingsDialog::setGamma(float gamma)
{
    if (m_gamma != gamma) {
        m_gamma = gamma;
        emit gammaChanged(m_gamma);
        ui->gammaSlider->setValue(int((m_gamma - 2.2f) * 6.0f));
    }
}

void SettingsDialog::setExposure(float exposure)
{
    if (m_exposure != exposure) {
        m_exposure = exposure;
        emit exposureChanged(m_exposure);
        ui->exposureSlider->setValue(int(m_exposure * 2.0f));
    }
}

void SettingsDialog::setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm)
{
    if (m_toneMappingAlgorithm != toneMappingAlgorithm) {
        m_toneMappingAlgorithm = toneMappingAlgorithm;
        emit toneMappingAlgorithmChanged(m_toneMappingAlgorithm);
        ui->toneMappingCombo->setCurrentIndex(static_cast<int>(m_toneMappingAlgorithm));
    }
}
