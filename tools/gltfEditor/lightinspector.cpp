/*
    lightinspector.cpp

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

#include "lightinspector.h"
#include <Kuesa/SpotLight>
#include <Kuesa/DirectionalLight>
#include <Kuesa/PointLight>

LightInspector::LightInspector(QObject *parent)
    : QObject(parent)
{
}

void LightInspector::setLight(Qt3DRender::QAbstractLight *light)
{
    if (light == m_light)
        return;

    disconnect(m_lightConnection);
    if (light)
        m_lightConnection = connect(light, &Qt3DCore::QNode::nodeDestroyed, this, [this]() { setLight(nullptr); });

    m_light = light;

    if (light) {
        m_name = light->objectName();
        m_color = light->color();
        m_intensity = light->intensity();
        m_type = light->type();

        //TODO:  UPdate this when/if we have range
        if (light->type() == Qt3DRender::QAbstractLight::DirectionalLight) {
            m_range = 0.0f;
        } else if (light->type() == Qt3DRender::QAbstractLight::PointLight) {
            auto point = qobject_cast<Kuesa::PointLight *>(light);
            m_range = point->range();
        } else if (light->type() == Qt3DRender::QAbstractLight::SpotLight) {
            auto spot = qobject_cast<Kuesa::SpotLight *>(light);
            m_innerConeAngle = spot->innerConeAngle();
            m_outerConeAngle = spot->outerConeAngle();
            m_range = spot->range();
        }
    } else {
        m_name.clear();
        m_color = Qt::black;
        m_intensity = 0.0f;
        m_innerConeAngle = 0.0f;
        m_outerConeAngle = 0.0f;
        m_range = 0.0f;
        m_type = -1;
    }

    emit lightParamsChanged();
}

QString LightInspector::name() const
{
    return m_name;
}

int LightInspector::type() const
{
    return m_type;
}

QColor LightInspector::color() const
{
    return m_color;
}

float LightInspector::intensity() const
{
    return m_intensity;
}

float LightInspector::range() const
{
    return m_range;
}

float LightInspector::innerConeAngle() const
{
    return m_innerConeAngle;
}

float LightInspector::outerConeAngle() const
{
    return m_outerConeAngle;
}
