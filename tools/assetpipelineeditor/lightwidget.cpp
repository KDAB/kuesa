/*
    lightwidget.cpp

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

#include "lightwidget.h"
#include "ui_lightwidget.h"
#include "lightinspector.h"
#include <QAbstractLight>

LightWidget::LightWidget(QWidget *parent)
    : QWidget(parent)
    , m_inspector(nullptr)
    , m_ui(new Ui::LightWidget)
{
    m_ui->setupUi(this);
}

LightWidget::~LightWidget()
{
    delete m_ui;
}

void LightWidget::setLightInspector(LightInspector *inspector)
{
    m_inspector = inspector;
    connect(m_inspector, &LightInspector::lightParamsChanged, this, &LightWidget::updateData);
}

void LightWidget::updateData()
{
    m_ui->nameLabel->setText(m_inspector->name());
    int type = m_inspector->type();
    QString typeName("invalid");
    if (type == Qt3DRender::QAbstractLight::PointLight)
        typeName = tr("point light");
    if (type == Qt3DRender::QAbstractLight::DirectionalLight)
        typeName = tr("directional light");
    if (type == Qt3DRender::QAbstractLight::SpotLight)
        typeName = tr("spot light");

    m_ui->typeLabel->setText(typeName);
    m_ui->colorLabel->setColor(m_inspector->color());
    m_ui->intensityLabel->setText(QString::number(m_inspector->intensity()));
    m_ui->innerConeValueLabel->setText(QString::number(m_inspector->innerConeAngle()));
    m_ui->outerConeValueLabel->setText(QString::number(m_inspector->outerConeAngle()));
    auto range = m_inspector->range();
    m_ui->rangeValueLabel->setText(range > 0.0 ? QString::number(range) : tr("Infinite"));

    const bool isSpotLight = type == Qt3DRender::QAbstractLight::SpotLight;
    m_ui->innerConeLabel->setVisible(isSpotLight);
    m_ui->innerConeValueLabel->setVisible(isSpotLight);
    m_ui->outerConeLabel->setVisible(isSpotLight);
    m_ui->outerConeValueLabel->setVisible(isSpotLight);

    const bool isDirectional = type == Qt3DRender::QAbstractLight::DirectionalLight;
    m_ui->rangeLabel->setHidden(isDirectional);
    m_ui->rangeValueLabel->setHidden(isDirectional);
}
