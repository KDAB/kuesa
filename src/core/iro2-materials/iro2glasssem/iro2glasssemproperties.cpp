
/*
    iro2glasssemproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Nicolas Guichard <nicolas.guichard@kdab.com>

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

#include "iro2glasssemproperties.h"
#include "iro2glasssemshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::Iro2GlassSemProperties
    \inheaderfile Kuesa/Iro2GlassSemProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2GlassSemProperties holds the properties controlling the visual appearance of a Iro2GlassSemMaterial instance.
*/

/*!
    \qmltype Iro2GlassSemProperties
    \instantiates Kuesa::Iro2GlassSemProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2GlassSemProperties holds the properties controlling the visual appearance of a Iro2GlassSemMaterial instance.
*/

Iro2GlassSemProperties::Iro2GlassSemProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2GlassSemShaderData(this))
    , m_reflectionMap(nullptr)
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::glassInnerFilterChanged, this, &Iro2GlassSemProperties::glassInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::glassOuterFilterChanged, this, &Iro2GlassSemProperties::glassOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::usesReflectionMapChanged, this, &Iro2GlassSemProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::reflectionGainChanged, this, &Iro2GlassSemProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::reflectionInnerFilterChanged, this, &Iro2GlassSemProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::reflectionOuterFilterChanged, this, &Iro2GlassSemProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::usesNormalMapChanged, this, &Iro2GlassSemProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::normalMapGainChanged, this, &Iro2GlassSemProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::normalScalingChanged, this, &Iro2GlassSemProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::normalDisturbChanged, this, &Iro2GlassSemProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::postVertexColorChanged, this, &Iro2GlassSemProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::postGainChanged, this, &Iro2GlassSemProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2GlassSemShaderData::gltfYUpChanged, this, &Iro2GlassSemProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setReflectionMap(new Empty2DTexture());
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2GlassSemProperties::~Iro2GlassSemProperties() = default;

Qt3DRender::QShaderData *Iro2GlassSemProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2GlassSemProperties::setGlassInnerFilter(const QVector3D &glassInnerFilter)
{
    m_shaderData->setGlassInnerFilter(glassInnerFilter);
}

void Iro2GlassSemProperties::setGlassOuterFilter(const QVector3D &glassOuterFilter)
{
    m_shaderData->setGlassOuterFilter(glassOuterFilter);
}

void Iro2GlassSemProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void Iro2GlassSemProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2GlassSemProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2GlassSemProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2GlassSemProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2GlassSemProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2GlassSemProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2GlassSemProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2GlassSemProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2GlassSemProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2GlassSemProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2GlassSemProperties::setReflectionMap(Qt3DRender::QAbstractTexture *reflectionMap)
{
    if (m_reflectionMap == reflectionMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_reflectionMap != nullptr)
        d->unregisterDestructionHelper(m_reflectionMap);
    m_reflectionMap = reflectionMap;
    if (m_reflectionMap != nullptr) {
        if (m_reflectionMap->parent() == nullptr)
            m_reflectionMap->setParent(this);
        d->registerDestructionHelper(m_reflectionMap, &Iro2GlassSemProperties::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void Iro2GlassSemProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
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
        d->registerDestructionHelper(m_normalMap, &Iro2GlassSemProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}

/*!
    \qmlproperty QVector3D Iro2GlassSemProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
/*!
    \property Iro2GlassSemProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
QVector3D Iro2GlassSemProperties::glassInnerFilter() const
{
    return m_shaderData->glassInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2GlassSemProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
/*!
    \property Iro2GlassSemProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
QVector3D Iro2GlassSemProperties::glassOuterFilter() const
{
    return m_shaderData->glassOuterFilter();
}

/*!
    \qmlproperty bool Iro2GlassSemProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property Iro2GlassSemProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool Iro2GlassSemProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty float Iro2GlassSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2GlassSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2GlassSemProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2GlassSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2GlassSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2GlassSemProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2GlassSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2GlassSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2GlassSemProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty bool Iro2GlassSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2GlassSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2GlassSemProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2GlassSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2GlassSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2GlassSemProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2GlassSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2GlassSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2GlassSemProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2GlassSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2GlassSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2GlassSemProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2GlassSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2GlassSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2GlassSemProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float Iro2GlassSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2GlassSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2GlassSemProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2GlassSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2GlassSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2GlassSemProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture *Iro2GlassSemProperties::reflectionMap() const
{
    return m_reflectionMap;
}

Qt3DRender::QAbstractTexture *Iro2GlassSemProperties::normalMap() const
{
    return m_normalMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
