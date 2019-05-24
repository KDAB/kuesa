/*
    directionallight.cpp

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

#include "directionallight.h"
#include "directionallight_p.h"
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DRender/private/shaderdata_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
  \class Kuesa::DirectionalLight
  \inmodule Kuesa
  \since 1.1
  \brief Encapsulate a Directional Light object in a Qt 3D scene.

 */

/*!
    \qmltype DirectionalLight
    \instantiates Kuesa::DirectionalLight
    \inherits AbstractLight
    \inqmlmodule Kuesa
    \since 1.1
    \brief Encapsulate a Directional Light object in a Qt 3D scene.

*/

/*!
  \fn Kuesa::DirectionalLight::DirectionalLight(Qt3DCore::QNode *parent)
  Constructs a new DirectionalLight with the specified \a parent.
 */

/*!
  \qmlproperty vector3d Qt3D.Render::DirectionalLight::localDirection
    Specifies the local direction of the directional light. The direction will be
    transformed by any rotations in parent nodes
*/

/*!
  \property Kuesa::DirectionalLight::localDirection
    Specifies the local direction of the directional light. The direction will be
    transformed by rotations in parent nodes
 */

DirectionalLightPrivate::DirectionalLightPrivate()
    : QAbstractLightPrivate(Qt3DRender::QAbstractLight::DirectionalLight)
{
    m_shaderData->setProperty("direction", QVector3D(0.0f, -1.0f, 0.0f));
    m_shaderData->setProperty("directionTransformed", Qt3DRender::Render::ShaderData::ModelToWorldDirection);
}
DirectionalLight::DirectionalLight(QNode *parent)
    : QAbstractLight(*new DirectionalLightPrivate, parent)
{
}

/*! \internal */
DirectionalLight::~DirectionalLight()
{
}

/*! \internal */
DirectionalLight::DirectionalLight(DirectionalLightPrivate &dd, QNode *parent)
    : QAbstractLight(dd, parent)
{
}

void DirectionalLight::setLocalDirection(const QVector3D &direction)
{
    Q_D(DirectionalLight);
    if (localDirection() != direction) {
        const QVector3D dir = direction.normalized();
        d->m_shaderData->setProperty("direction", dir);
        emit localDirectionChanged(dir);
    }
}

QVector3D DirectionalLight::localDirection() const
{
    Q_D(const DirectionalLight);
    return d->m_shaderData->property("direction").value<QVector3D>();
}

} // namespace Kuesa

QT_END_NAMESPACE
