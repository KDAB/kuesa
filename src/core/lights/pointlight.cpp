/*
    pointlight.cpp

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

#include "pointlight.h"
#include "pointlight_p.h"

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
  \class Kuesa::PointLight
  \inmodule Kuesa
  \since Kuesa 1.1
    \brief Encapsulate a Point Light object in a Qt 3D scene.

 */

/*!
    \qmltype PointLight
    \instantiates Kuesa::PointLight
    \inherits AbstractLight
    \inqmlmodule Kuesa
    \since Kuesa 1.1
    \brief Encapsulate a Point Light object in a Qt 3D scene.
*/

/*!
  \fn Kuesa::PointLight::PointLight(Qt3DCore::QNode *parent)
  Constructs a new PointLight with the specified \a parent.
 */

/*!
  \qmlproperty float Kuesa::PointLight::range
    Specifies the destance at which the light intensity may be considered
    to have reached zero and has no effect. A range of 0 (default) indicates
    infinite range
*/

/*!
  \property float Kuesa::PointLight::range
    Specifies the destance at which the light intensity may be considered
    to have reached zero and has no effect. A range of 0 (default) indicates
    infinite range
 */

PointLightPrivate::PointLightPrivate()
    : QAbstractLightPrivate(Qt3DRender::QAbstractLight::PointLight)
{
    m_shaderData->setProperty("range", 0.0f);
}

PointLight::PointLight(QNode *parent)
    : QAbstractLight(*new PointLightPrivate, parent)
{
}

/*! \internal */
PointLight::~PointLight()
{
}

/*! \internal */
PointLight::PointLight(PointLightPrivate &dd, QNode *parent)
    : QAbstractLight(dd, parent)
{
}

float PointLight::range() const
{
    Q_D(const PointLight);
    return d->m_shaderData->property("range").toFloat();
}

void PointLight::setRange(float value)
{
    Q_D(PointLight);
    if (range() != value) {
        d->m_shaderData->setProperty("range", value);
        emit rangeChanged(value);
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
