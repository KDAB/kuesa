/*
    spotlight.cpp

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

#include "spotlight.h"
#include "spotlight_p.h"
#include <qmath.h>
#include <cmath>
#include <Qt3DRender/private/shaderdata_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::SpotLight
    \inmodule Kuesa
    \since Kuesa 1.1
    \brief Encapsulate a Spot Light object in a Qt 3D scene.
 */

/*!
    \qmltype SpotLight
    \instantiates Kuesa::SpotLight
    \inherits AbstractLight
    \inqmlmodule Kuesa
    \since Kuesa 1.1
    \brief Encapsulate a Spot Light object in a Qt 3D scene.
*/

/*!
    \qmlproperty vector3d Kuesa::SpotLight::localDirection
    Specifies the local direction of the spot light.  Local
    direction is affected by parent transform rotations.
*/

/*!
    \property Kuesa::SpotLight::localDirection
    Specifies the local direction of the spot light. Local
    direction is affected by parent transform rotations.
 */

/*!
    \property Kuesa::SpotLight::innerConeAngle
    Specifies the angle from center of the spot light where light
    begins to fall off.  The value must be greater than or equal to 0
    and less than outerConeAngle.
 */

/*!
    \qmlproperty float Kuesa::SpotLight::innerConeAngle
    Specifies the angle from center of the spot light where light
    begins to fall off.  The value must be greater than or equal to 0
    and less than outerConeAngle.
*/

/*!
    \qmlproperty float Kuesa::SpotLight::outerConeAngle
    Specifies the cut off angle of the spot light. Objects outside
    this angle receives no light from the SpotLight. The value must be
    greater than innerConeAngle and less than or equal to 180.
*/

/*!
    \property Kuesa::SpotLight::outerConeAngle
    Specifies the cut off angle of the spot light.  Objects outside
    this angle receives no light from the SpotLight. The value must be
    greater than innerConeAngle and less than or equal to 180.
 */

/*!
    \qmlproperty float Kuesa::SpotLight::range
    Specifies the destance at which the light intensity may be considered
    to have reached zero and has no effect. A range of 0 (default) indicates
    infinite range
*/

/*!
    \property Kuesa::SpotLight::range
    Specifies the destance at which the light intensity may be considered
    to have reached zero and has no effect. A range of 0 (default) indicates
    infinite range
 */

SpotLightPrivate::SpotLightPrivate()
    : QAbstractLightPrivate(Qt3DRender::QAbstractLight::SpotLight)
{
    // Set all light uniforms, including ones that don't apply to this light type
    // TODO: Move this into common class when adding shadows
    m_shaderData->setProperty("direction", QVector3D(0.0f, -1.0f, 0.0f));
    m_shaderData->setProperty("directionTransformed", Qt3DRender::Render::ShaderData::ModelToWorldDirection);
    m_shaderData->setProperty("range", 0.0f);
    m_shaderData->setProperty("lightAngleScale", 0);
    m_shaderData->setProperty("lightAngleOffset", 0);
}

/*!
    Constructs a new SpotLight with the specified \a parent. The
    SpotLight will be positioned and rotated according to transform
    of its containing Entity.
 */
SpotLight::SpotLight(QNode *parent)
    : QAbstractLight(*new SpotLightPrivate, parent)
    , m_innerConeAngle(90.0f)
    , m_outerConeAngle(90.0f)
{
    updateAngularAttenuationUniforms();
}

/*! \internal */
SpotLight::~SpotLight()
{
}

/*! \internal */
SpotLight::SpotLight(SpotLightPrivate &dd, QNode *parent)
    : QAbstractLight(dd, parent)
{
}

QVector3D SpotLight::localDirection() const
{
    Q_D(const SpotLight);
    return d->m_shaderData->property("direction").value<QVector3D>();
}

void SpotLight::setLocalDirection(const QVector3D &direction)
{
    Q_D(SpotLight);
    if (localDirection() != direction) {
        const QVector3D dir = direction.normalized();
        d->m_shaderData->setProperty("direction", dir);
        emit localDirectionChanged(dir);
    }
}

float SpotLight::outerConeAngle() const
{
    return m_outerConeAngle;
}

void SpotLight::setOuterConeAngle(float value)
{
    if (value < 0.0 || value > 180.0)
        qWarning() << "invalid outerConeAngle value: " << value << ".  outerConeAngle should be between 0 and 180.";

    if (m_outerConeAngle != value) {
        m_outerConeAngle = value;
        updateAngularAttenuationUniforms();
        emit outerConeAngleChanged(value);
    }
}

float SpotLight::innerConeAngle() const
{
    return m_innerConeAngle;
}

void SpotLight::setInnerConeAngle(float value)
{
    if (value < 0.0 || value > 180.0)
        qWarning() << "invalid innerConeAngle value: " << value << ".  innerConeAngle should be between 0 and 180.";

    if (m_innerConeAngle != value) {
        m_innerConeAngle = value;
        updateAngularAttenuationUniforms();
        emit innerConeAngleChanged(value);
    }
}

/*!
 * \internal
 *
 * precompute lightAngleScale and lightAngleoffset uniforms based on
 * innerConeAngle and outerConeAngle.  These values are used to calculate
 * angular attenuation of the spotlight giving a bright central area and
 * smooth folloff to the edge.
 * See KHR_lights_punctual spec for implementation details
 *
 */
void SpotLight::updateAngularAttenuationUniforms()
{
    Q_D(SpotLight);
    const float innerConeRadians = qDegreesToRadians(innerConeAngle());
    const float outerConeRadians = qDegreesToRadians(outerConeAngle());
    const float lightAngleScale = 1.0f / std::max(0.001f, std::cos(innerConeRadians) - std::cos(outerConeRadians));
    const float lightAngleOffset = -cos(outerConeRadians) * lightAngleScale;
    d->m_shaderData->setProperty("lightAngleScale", lightAngleScale);
    d->m_shaderData->setProperty("lightAngleOffset", lightAngleOffset);
}


float SpotLight::range() const
{
    Q_D(const SpotLight);
    return d->m_shaderData->property("range").toFloat();
}

void SpotLight::setRange(float value)
{
    Q_D(SpotLight);
    if (range() != value) {
        d->m_shaderData->setProperty("range", value);
        emit rangeChanged(value);
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
