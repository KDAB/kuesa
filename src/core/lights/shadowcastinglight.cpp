/*
    ShadowCastingLight.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "shadowcastinglight.h"
#include "shadowcastinglight_p.h"
#include <Qt3DRender/private/shaderdata_p.h>
#include <QMatrix4x4>
#include <QVector2D>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::ShadowCastingLight
    \inheaderfile Kuesa/ShadowCastingLight
    \inmodule Kuesa
    \since Kuesa 1.3
    \brief Encapsulate a light object that is able to cast shadows
    in a Qt 3D scene.

    Shadow are currently only supported with Qt3D running with the OpenGL
    backend which supports geometry shaders when running on devices supporting
    OpenGL 3.2 or OpenGL ES 3.2. When Qt3D is using the Qt RHI (rendering
    abstraction interface) backend. shadows are not supported due to the lack
    of geometry shader support in RHI.
 */

/*!
    \qmltype ShadowCastingLight
    \instantiates Kuesa::ShadowCastingLight
    \inherits AbstractLight
    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \brief Encapsulate a light object that is able to cast shadows
    in a Qt 3D scene.

    Shadow are currently only supported with Qt3D running with the OpenGL
    backend which supports geometry shaders when running on devices supporting
    OpenGL 3.2 or OpenGL ES 3.2. When Qt3D is using the Qt RHI (rendering
    abstraction interface) backend. shadows are not supported due to the lack
    of geometry shader support in RHI.
*/

/*!
    \qmlproperty bool ShadowCastingLight::castsShadows
    Specifies whether or not the light casts shadows.
*/

/*!
    \property ShadowCastingLight::castsShadows
    Specifies whether or not the light casts shadows.
*/

/*!
    \qmlproperty QSize ShadowCastingLight::textureSize
    Specifies the size of the texture to use for the shadowmap. Larger textures result in
    higher quality shadows
*/

/*!
    \property ShadowCastingLight::textureSize
    Specifies the size of the texture to use for the shadowmap. Larger textures result in
    higher quality shadows
*/

/*!
    \qmlproperty bool ShadowCastingLight::softShadows
    Determines whether shadow edges are soft or hard. Soft shadows look more realistic
    but can impact rendering performance
*/

/*!
    \property ShadowCastingLight::softShadows
    Determines whether shadow edges are soft or hard. Soft shadows look more realistic
    but can impact rendering performance
*/

/*!
    \qmlproperty float ShadowCastingLight::shadowMapBias
    Holds the amount to offset the shadow map perpendicular to the surface when checking
    whether a surface is in shadow. This helps eliminate self-shadowing and "shadow-acne".
    Larger values reduce these effects but if the value is too large, objects can appear
    to float above their shadows.
*/

/*!
    \property ShadowCastingLight::shadowMapBias
    Holds the amount to offset the shadow map perpendicular to the surface when checking
    whether a surface is in shadow. This helps eliminate self-shadowing and "shadow-acne".
    Larger values reduce these effects but if the value is too large, objects can appear
    to float above their shadows.
*/

/*!
    \qmlproperty float ShadowCastingLight::nearPlane
    Holds the distance from the light at which it should begin casting shadows. Setting this
    as large as possible while still including all shadow-casting objects will increase the
    precision of the shadowMap depth texture, reducing self-shadowing and "shadow-acne"
*/

/*!
    \property ShadowCastingLight::nearPlane
    Holds the distance from the light at which it should begin casting shadows. Setting this
    as large as possible while still including all shadow-casting objects will increase the
    precision of the shadowMap depth texture, reducing self-shadowing and "shadow-acne"
*/

ShadowCastingLightPrivate::ShadowCastingLightPrivate(Qt3DRender::QAbstractLight::Type type)
    : QAbstractLightPrivate(type)
{
    // Set all light uniforms, including ones that don't apply to this light type
    m_shaderData->setProperty("direction", QVector3D(0.0f, -1.0f, 0.0f));
    m_shaderData->setProperty("range", 0.0f);
    m_shaderData->setProperty("lightAngleScale", 0);
    m_shaderData->setProperty("lightAngleOffset", 0);
    m_shaderData->setProperty("castsShadows", false);
    m_shaderData->setProperty("shadowMapCubeDepthTexture", QVariant());
    m_shaderData->setProperty("shadowMapDepthTexture", QVariant());
    m_shaderData->setProperty("lightProjectionMatrix", QMatrix4x4());
    m_shaderData->setProperty("nearFarPlanes", QVector2D());
    m_shaderData->setProperty("shadowBias", 0.005f);
    m_shaderData->setProperty("depthArrayIndex", 0);
    m_shaderData->setProperty("usePCF", true);

    if (type == QAbstractLight::Type::SpotLight)
        m_shaderData->setProperty("directionTransformed", Qt3DRender::Render::ShaderData::ModelToWorldDirection);
    else
        m_shaderData->setProperty("directionTransformed", Qt3DRender::Render::ShaderData::NoTransform);
}

/*! \internal */
ShadowCastingLight::~ShadowCastingLight()
{
}

/*! \internal */
ShadowCastingLight::ShadowCastingLight(ShadowCastingLightPrivate &dd, QNode *parent)
    : QAbstractLight(dd, parent)
    , m_textureSize(1024, 1024)
{
}

void ShadowCastingLight::setCastsShadows(bool shadow)
{
    if (shadow == castsShadows())
        return;

    Q_D(ShadowCastingLight);
    d->m_shaderData->setProperty("castsShadows", shadow);

    emit castsShadowsChanged(shadow);
} // namespace Kuesa

void ShadowCastingLight::setLightViewProjectionMatrix(const QMatrix4x4 &matrix)
{
    Q_D(ShadowCastingLight);
    d->m_shaderData->setProperty("lightProjectionMatrix", matrix);
}

void ShadowCastingLight::setNearFarPlanes(float nearPlane, float farPlane)
{
    Q_D(ShadowCastingLight);
    d->m_shaderData->setProperty("nearFarPlanes", QVector2D{ nearPlane, farPlane });
}

void ShadowCastingLight::setTextureSize(const QSize &size)
{
    if (size == m_textureSize)
        return;

    m_textureSize = size;
    emit textureSizeChanged(size);
}

bool ShadowCastingLight::castsShadows() const
{
    Q_D(const ShadowCastingLight);
    return d->m_shaderData->property("castsShadows").toBool();
}

QSize ShadowCastingLight::textureSize() const
{
    return m_textureSize;
}

float ShadowCastingLight::shadowMapBias() const
{
    Q_D(const ShadowCastingLight);
    return d->m_shaderData->property("shadowBias").toFloat();
}

void ShadowCastingLight::setShadowMapBias(float b)
{
    if (qFuzzyCompare(shadowMapBias(), b))
        return;

    Q_D(const ShadowCastingLight);
    d->m_shaderData->setProperty("shadowBias", b);
    emit biasChanged(b);
}

void Kuesa::ShadowCastingLight::setSoftShadows(bool soft)
{
    if (soft == softShadows())
        return;

    Q_D(const ShadowCastingLight);
    d->m_shaderData->setProperty("usePCF", soft);
    emit softShadowsChanged(soft);
}

void ShadowCastingLight::setLightIndex(int index)
{
    Q_D(const ShadowCastingLight);
    d->m_shaderData->setProperty("depthArrayIndex", index);
}

void ShadowCastingLight::setNearPlane(float nearPlane)
{
    if (qFuzzyCompare(m_nearPlane, nearPlane))
        return;

    m_nearPlane = nearPlane;
    emit nearPlaneChanged(m_nearPlane);
}

bool ShadowCastingLight::softShadows() const
{
    Q_D(const ShadowCastingLight);
    return d->m_shaderData->property("usePCF").toBool();
}

float ShadowCastingLight::nearPlane() const
{
    return m_nearPlane;
}

} // namespace Kuesa

QT_END_NAMESPACE
