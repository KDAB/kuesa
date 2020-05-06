/*
    directionallight.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/private/shaderdata_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::DirectionalLight
    \inheaderfile Kuesa/DirectionalLight
    \inmodule Kuesa
    \since Kuesa 1.1
    \brief Encapsulate a Directional Light object in a Qt 3D scene.
 */

/*!
    \qmltype DirectionalLight
    \instantiates Kuesa::DirectionalLight
    \inherits AbstractLight
    \inqmlmodule Kuesa
    \since Kuesa 1.1
    \brief Encapsulate a Directional Light object in a Qt 3D scene.

*/

/*!
    \qmlproperty vector3d DirectionalLight::direction
    Specifies the direction of the light. If the directionType is
    Local, the light direction wil lbe transformed by any rotations
    in parent nodes. If the directionType is World, the direction
    is unaffected by any transformations in parent nodes.
*/

/*!
    \property DirectionalLight::direction
    Specifies the direction of the light. If the directionType is
    Local, the light direction wil lbe transformed by any rotations
    in parent nodes. If the directionType is World, the direction
    is unaffected by any transformations in parent nodes.
 */

/*!
    \qmlproperty vector3d DirectionalLight::directionMode
    Specifies whether the direction property should be interpreted
    as a world or local direction. Local directions inherit parent
    node rotation transformations and World directions do not.
    The default is World.
*/

/*!
    \property DirectionalLight::directionMode
    Specifies the direction of the light. If the directionType is
    Specifies whether the direction property should be interpreted
    as a world or local direction. Local directions inherit parent
    node rotation transformations and World directions do not.
    The default is World.
 */

DirectionalLightPrivate::DirectionalLightPrivate()
    : QAbstractLightPrivate(Qt3DRender::QAbstractLight::DirectionalLight)
{
    m_shaderData->setProperty("direction", QVector3D(0.0f, -1.0f, 0.0f));
    m_shaderData->setProperty("directionTransformed", Qt3DRender::Render::ShaderData::NoTransform);
}

/*!
    Constructs a new DirectionalLight with the specified \a parent.
 */
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

void DirectionalLight::setDirection(const QVector3D &dir)
{
    Q_D(DirectionalLight);
    if (direction() != dir) {
        const QVector3D normalDir = dir.normalized();
        d->m_shaderData->setProperty("direction", normalDir);
        emit directionChanged(normalDir);
    }
}

QVector3D DirectionalLight::direction() const
{
    Q_D(const DirectionalLight);
    return d->m_shaderData->property("direction").value<QVector3D>();
}

void DirectionalLight::setDirectionMode(DirectionalLight::DirectionMode directionType)
{
    Q_D(DirectionalLight);

    if (m_directionType == directionType)
        return;

    m_directionType = directionType;
    d->m_shaderData->setProperty("directionTransformed", directionType == Local ? Qt3DRender::Render::ShaderData::ModelToWorldDirection : Qt3DRender::Render::ShaderData::NoTransform);

    emit directionModeChanged(m_directionType);
}

DirectionalLight::DirectionMode DirectionalLight::directionMode() const
{
    return m_directionType;
}

} // namespace Kuesa

QT_END_NAMESPACE
