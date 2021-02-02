
/*
    iro2planarreflectionsemproperties.cpp

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

#include "iro2planarreflectionsemproperties.h"
#include "iro2planarreflectionsemshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::Iro2PlanarReflectionSemProperties
    \inheaderfile Kuesa/Iro2PlanarReflectionSemProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2PlanarReflectionSemProperties holds the properties controlling the visual appearance of a Iro2PlanarReflectionSemMaterial instance.
*/

/*!
    \qmltype Iro2PlanarReflectionSemProperties
    \instantiates Kuesa::Iro2PlanarReflectionSemProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2PlanarReflectionSemProperties holds the properties controlling the visual appearance of a Iro2PlanarReflectionSemMaterial instance.
*/

Iro2PlanarReflectionSemProperties::Iro2PlanarReflectionSemProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2PlanarReflectionSemShaderData(this))
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::reflectionGainChanged, this, &Iro2PlanarReflectionSemProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::reflectionInnerFilterChanged, this, &Iro2PlanarReflectionSemProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::reflectionOuterFilterChanged, this, &Iro2PlanarReflectionSemProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::innerAlphaChanged, this, &Iro2PlanarReflectionSemProperties::innerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::outerAlphaChanged, this, &Iro2PlanarReflectionSemProperties::outerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::alphaGainChanged, this, &Iro2PlanarReflectionSemProperties::alphaGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::usesNormalMapChanged, this, &Iro2PlanarReflectionSemProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::normalMapGainChanged, this, &Iro2PlanarReflectionSemProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::normalScalingChanged, this, &Iro2PlanarReflectionSemProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::normalDisturbChanged, this, &Iro2PlanarReflectionSemProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::postVertexColorChanged, this, &Iro2PlanarReflectionSemProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::postGainChanged, this, &Iro2PlanarReflectionSemProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2PlanarReflectionSemShaderData::gltfYUpChanged, this, &Iro2PlanarReflectionSemProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2PlanarReflectionSemProperties::~Iro2PlanarReflectionSemProperties() = default;

Qt3DRender::QShaderData *Iro2PlanarReflectionSemProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2PlanarReflectionSemProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2PlanarReflectionSemProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2PlanarReflectionSemProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2PlanarReflectionSemProperties::setInnerAlpha(float innerAlpha)
{
    m_shaderData->setInnerAlpha(innerAlpha);
}

void Iro2PlanarReflectionSemProperties::setOuterAlpha(float outerAlpha)
{
    m_shaderData->setOuterAlpha(outerAlpha);
}

void Iro2PlanarReflectionSemProperties::setAlphaGain(float alphaGain)
{
    m_shaderData->setAlphaGain(alphaGain);
}

void Iro2PlanarReflectionSemProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2PlanarReflectionSemProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2PlanarReflectionSemProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2PlanarReflectionSemProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2PlanarReflectionSemProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2PlanarReflectionSemProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2PlanarReflectionSemProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2PlanarReflectionSemProperties::setNormalMap(Qt3DRender::QAbstractTexture * normalMap)
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
        d->registerDestructionHelper(m_normalMap, &Iro2PlanarReflectionSemProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}


/*!
    \qmlproperty float Iro2PlanarReflectionSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2PlanarReflectionSemProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2PlanarReflectionSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2PlanarReflectionSemProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2PlanarReflectionSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2PlanarReflectionSemProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty float Iro2PlanarReflectionSemProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
float Iro2PlanarReflectionSemProperties::innerAlpha() const
{
    return m_shaderData->innerAlpha();
}

/*!
    \qmlproperty float Iro2PlanarReflectionSemProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
float Iro2PlanarReflectionSemProperties::outerAlpha() const
{
    return m_shaderData->outerAlpha();
}

/*!
    \qmlproperty float Iro2PlanarReflectionSemProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
float Iro2PlanarReflectionSemProperties::alphaGain() const
{
    return m_shaderData->alphaGain();
}

/*!
    \qmlproperty bool Iro2PlanarReflectionSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2PlanarReflectionSemProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2PlanarReflectionSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2PlanarReflectionSemProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2PlanarReflectionSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2PlanarReflectionSemProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2PlanarReflectionSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2PlanarReflectionSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2PlanarReflectionSemProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2PlanarReflectionSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2PlanarReflectionSemProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float Iro2PlanarReflectionSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2PlanarReflectionSemProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2PlanarReflectionSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2PlanarReflectionSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2PlanarReflectionSemProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture * Iro2PlanarReflectionSemProperties::normalMap() const
{
    return m_normalMap;
}


} // namespace Kuesa

QT_END_NAMESPACE
