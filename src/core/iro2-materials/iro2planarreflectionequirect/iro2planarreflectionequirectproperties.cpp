
/*
    iro2planarreflectionequirectproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#include "iro2planarreflectionequirectproperties.h"
#include "iro2planarreflectionequirectshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::Iro2PlanarReflectionEquiRectProperties
    \inheaderfile Kuesa/Iro2PlanarReflectionEquiRectProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2PlanarReflectionEquiRectProperties holds the properties controlling the visual appearance of a Iro2PlanarReflectionEquiRectMaterial instance.
*/

/*!
    \qmltype Iro2PlanarReflectionEquiRectProperties
    \instantiates Kuesa::Iro2PlanarReflectionEquiRectProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2PlanarReflectionEquiRectProperties holds the properties controlling the visual appearance of a Iro2PlanarReflectionEquiRectMaterial instance.
*/

Iro2PlanarReflectionEquiRectProperties::Iro2PlanarReflectionEquiRectProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2PlanarReflectionEquiRectShaderData(this))
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::reflectionGainChanged, this, &Iro2PlanarReflectionEquiRectProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::reflectionInnerFilterChanged, this, &Iro2PlanarReflectionEquiRectProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::reflectionOuterFilterChanged, this, &Iro2PlanarReflectionEquiRectProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::innerAlphaChanged, this, &Iro2PlanarReflectionEquiRectProperties::innerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::outerAlphaChanged, this, &Iro2PlanarReflectionEquiRectProperties::outerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::alphaGainChanged, this, &Iro2PlanarReflectionEquiRectProperties::alphaGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::usesNormalMapChanged, this, &Iro2PlanarReflectionEquiRectProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::normalMapGainChanged, this, &Iro2PlanarReflectionEquiRectProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::normalScalingChanged, this, &Iro2PlanarReflectionEquiRectProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::normalDisturbChanged, this, &Iro2PlanarReflectionEquiRectProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::postVertexColorChanged, this, &Iro2PlanarReflectionEquiRectProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::postGainChanged, this, &Iro2PlanarReflectionEquiRectProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionEquiRectShaderData::gltfYUpChanged, this, &Iro2PlanarReflectionEquiRectProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2PlanarReflectionEquiRectProperties::~Iro2PlanarReflectionEquiRectProperties() = default;

Qt3DRender::QShaderData *Iro2PlanarReflectionEquiRectProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2PlanarReflectionEquiRectProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2PlanarReflectionEquiRectProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2PlanarReflectionEquiRectProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2PlanarReflectionEquiRectProperties::setInnerAlpha(float innerAlpha)
{
    m_shaderData->setInnerAlpha(innerAlpha);
}

void Iro2PlanarReflectionEquiRectProperties::setOuterAlpha(float outerAlpha)
{
    m_shaderData->setOuterAlpha(outerAlpha);
}

void Iro2PlanarReflectionEquiRectProperties::setAlphaGain(float alphaGain)
{
    m_shaderData->setAlphaGain(alphaGain);
}

void Iro2PlanarReflectionEquiRectProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2PlanarReflectionEquiRectProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2PlanarReflectionEquiRectProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2PlanarReflectionEquiRectProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2PlanarReflectionEquiRectProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2PlanarReflectionEquiRectProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2PlanarReflectionEquiRectProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2PlanarReflectionEquiRectProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
{
    if (m_normalMap == normalMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_normalMap != nullptr)
        d->unregisterDestructionHelper(m_normalMap);
    m_normalMap = normalMap;
    if (m_normalMap != nullptr) {
        if (m_normalMap->parent() == nullptr)
            m_normalMap->setParent(this);
        d->registerDestructionHelper(m_normalMap, &Iro2PlanarReflectionEquiRectProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}

/*!
    \qmlproperty float Iro2PlanarReflectionEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2PlanarReflectionEquiRectProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2PlanarReflectionEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2PlanarReflectionEquiRectProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2PlanarReflectionEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2PlanarReflectionEquiRectProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty float Iro2PlanarReflectionEquiRectProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
float Iro2PlanarReflectionEquiRectProperties::innerAlpha() const
{
    return m_shaderData->innerAlpha();
}

/*!
    \qmlproperty float Iro2PlanarReflectionEquiRectProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
float Iro2PlanarReflectionEquiRectProperties::outerAlpha() const
{
    return m_shaderData->outerAlpha();
}

/*!
    \qmlproperty float Iro2PlanarReflectionEquiRectProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
float Iro2PlanarReflectionEquiRectProperties::alphaGain() const
{
    return m_shaderData->alphaGain();
}

/*!
    \qmlproperty bool Iro2PlanarReflectionEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2PlanarReflectionEquiRectProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2PlanarReflectionEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2PlanarReflectionEquiRectProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2PlanarReflectionEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2PlanarReflectionEquiRectProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2PlanarReflectionEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2PlanarReflectionEquiRectProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2PlanarReflectionEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2PlanarReflectionEquiRectProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float Iro2PlanarReflectionEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2PlanarReflectionEquiRectProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2PlanarReflectionEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2PlanarReflectionEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2PlanarReflectionEquiRectProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture *Iro2PlanarReflectionEquiRectProperties::normalMap() const
{
    return m_normalMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
