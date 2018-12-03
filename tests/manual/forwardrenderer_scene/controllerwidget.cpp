/*
    controllerwidget.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "controllerwidget.h"
#include "ui_controllerwidget.h"
#include "scenecontroller.h"
#include <QColorDialog>

ControllerWidget::ControllerWidget(SceneController *controller,
                                   QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControllerWidget)
    , m_controller(controller)
{
    Q_ASSERT(m_controller);
    ui->setupUi(this);

    connect(ui->sceneComboBox, &QComboBox::currentTextChanged,
            m_controller, &SceneController::setSceneName);

    connect(ui->alphaBlendingCheckbox, &QCheckBox::toggled,
            m_controller, &SceneController::setAlphaBlending);

    connect(ui->frustumCheckBox, &QCheckBox::toggled,
            m_controller, &SceneController::setFrustumCulling);

    connect(ui->zFillCheckBox, &QCheckBox::toggled,
            m_controller, &SceneController::setZFill);

    connect(ui->colorButton, &QPushButton::clicked,
            this, &ControllerWidget::setClearColor);

    //bloom
     connect(ui->bloomFilterGroupBox, &QGroupBox::toggled,
            m_controller, &SceneController::setBloomEffect);

    connect(ui->exposureSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_controller, &SceneController::setBloomExposure);
    connect(ui->bloomThresholdSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_controller, &SceneController::setBloomThreshold);
    connect(ui->bloomBlurPassCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            m_controller, &SceneController::setBloomBlurPassCount);

    //blur
    connect(ui->blurEffectGroupBox, &QGroupBox::toggled,
            m_controller, &SceneController::setBlurEffect);

    connect(ui->blurPassCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            m_controller, &SceneController::setBlurPassCount);

    //threshold
    connect(ui->thresholdGroupBox, &QGroupBox::toggled,
           m_controller, &SceneController::setThresholdEffect);
    connect(ui->thresholdSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_controller, &SceneController::setThreshold);

    // opacity mask
    connect(ui->opacityMaskGroupBox, &QGroupBox::toggled,
            m_controller, &SceneController::setOpacityMaskEffect);
    connect(ui->premultipliedAlphaCheckBox, &QCheckBox::toggled,
            m_controller, &SceneController::setOpacityMaskPremultipliedAlpha);

    ui->sceneComboBox->addItems(m_controller->scenes());
    ui->sceneComboBox->setCurrentIndex(m_controller->scenes().indexOf(m_controller->sceneName()));

    QString qss = QString("background-color: %1").arg(controller->clearColor().name());
    ui->colorButton->setStyleSheet(qss);

    ui->alphaBlendingCheckbox->setChecked(controller->alphaBlending());
    ui->frustumCheckBox->setChecked(controller->frustumCulling());

    ui->bloomFilterGroupBox->setChecked(controller->bloomEffect());
    ui->bloomThresholdSpinBox->setValue(controller->bloomThreshold());
    ui->bloomBlurPassCountSpinBox->setValue(controller->bloomBlurPassCount());
    ui->exposureSpinBox->setValue(controller->bloomExposure());

    ui->blurEffectGroupBox->setChecked(controller->blurEffect());
    ui->blurPassCountSpinBox->setValue(controller->blurPassCount());

    ui->thresholdGroupBox->setChecked(controller->thresholdEffect());
    ui->thresholdSpinBox->setValue(controller->threshold());

    ui->opacityMaskGroupBox->setChecked(controller->opacityMaskEffect());
    ui->premultipliedAlphaCheckBox->setChecked(controller->opacityMaskPremultipliedAlpha());
}

ControllerWidget::~ControllerWidget()
{
    delete ui;
}

void ControllerWidget::setClearColor()
{
    const QColor color = QColorDialog::getColor(m_controller->clearColor());
    if (color.isValid()) {
        m_controller->setClearColor(color);
        QString qss = QString("background-color: %1").arg(color.name());
        ui->colorButton->setStyleSheet(qss);
    }
}


