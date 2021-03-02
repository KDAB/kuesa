
/*
    iro2glassequirectproperties.cpp

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

#include "iro2glassequirectproperties.h"
#include "iro2glassequirectshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::Iro2GlassEquiRectProperties
    \inheaderfile Kuesa/Iro2GlassEquiRectProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2GlassEquiRectProperties holds the properties controlling the visual appearance of a Iro2GlassEquiRectMaterial instance.
*/

/*!
    \qmltype Iro2GlassEquiRectProperties
    \instantiates Kuesa::Iro2GlassEquiRectProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2GlassEquiRectProperties holds the properties controlling the visual appearance of a Iro2GlassEquiRectMaterial instance.
*/

Iro2GlassEquiRectProperties::Iro2GlassEquiRectProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2GlassEquiRectShaderData(this))
    , m_reflectionMap(nullptr)
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::glassInnerFilterChanged, this, &Iro2GlassEquiRectProperties::glassInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::glassOuterFilterChanged, this, &Iro2GlassEquiRectProperties::glassOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::usesReflectionMapChanged, this, &Iro2GlassEquiRectProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::reflectionGainChanged, this, &Iro2GlassEquiRectProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::reflectionInnerFilterChanged, this, &Iro2GlassEquiRectProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::reflectionOuterFilterChanged, this, &Iro2GlassEquiRectProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::usesNormalMapChanged, this, &Iro2GlassEquiRectProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::normalMapGainChanged, this, &Iro2GlassEquiRectProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::normalScalingChanged, this, &Iro2GlassEquiRectProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::normalDisturbChanged, this, &Iro2GlassEquiRectProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::postVertexColorChanged, this, &Iro2GlassEquiRectProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::postGainChanged, this, &Iro2GlassEquiRectProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2GlassEquiRectShaderData::gltfYUpChanged, this, &Iro2GlassEquiRectProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setReflectionMap(new Empty2DTexture());
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2GlassEquiRectProperties::~Iro2GlassEquiRectProperties() = default;

Qt3DRender::QShaderData *Iro2GlassEquiRectProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2GlassEquiRectProperties::setGlassInnerFilter(const QVector3D &glassInnerFilter)
{
    m_shaderData->setGlassInnerFilter(glassInnerFilter);
}

void Iro2GlassEquiRectProperties::setGlassOuterFilter(const QVector3D &glassOuterFilter)
{
    m_shaderData->setGlassOuterFilter(glassOuterFilter);
}

void Iro2GlassEquiRectProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void Iro2GlassEquiRectProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2GlassEquiRectProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2GlassEquiRectProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2GlassEquiRectProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2GlassEquiRectProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2GlassEquiRectProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2GlassEquiRectProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2GlassEquiRectProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2GlassEquiRectProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2GlassEquiRectProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2GlassEquiRectProperties::setReflectionMap(Qt3DRender::QAbstractTexture *reflectionMap)
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
        d->registerDestructionHelper(m_reflectionMap, &Iro2GlassEquiRectProperties::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void Iro2GlassEquiRectProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
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
        d->registerDestructionHelper(m_normalMap, &Iro2GlassEquiRectProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}

/*!
    \qmlproperty QVector3D Iro2GlassEquiRectProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
/*!
    \property Iro2GlassEquiRectProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
QVector3D Iro2GlassEquiRectProperties::glassInnerFilter() const
{
    return m_shaderData->glassInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2GlassEquiRectProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
/*!
    \property Iro2GlassEquiRectProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
QVector3D Iro2GlassEquiRectProperties::glassOuterFilter() const
{
    return m_shaderData->glassOuterFilter();
}

/*!
    \qmlproperty bool Iro2GlassEquiRectProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property Iro2GlassEquiRectProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool Iro2GlassEquiRectProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty float Iro2GlassEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2GlassEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2GlassEquiRectProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2GlassEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2GlassEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2GlassEquiRectProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2GlassEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2GlassEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2GlassEquiRectProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty bool Iro2GlassEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2GlassEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2GlassEquiRectProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2GlassEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2GlassEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2GlassEquiRectProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2GlassEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2GlassEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2GlassEquiRectProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2GlassEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2GlassEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2GlassEquiRectProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2GlassEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2GlassEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2GlassEquiRectProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float Iro2GlassEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2GlassEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2GlassEquiRectProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2GlassEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2GlassEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2GlassEquiRectProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture *Iro2GlassEquiRectProperties::reflectionMap() const
{
    return m_reflectionMap;
}

Qt3DRender::QAbstractTexture *Iro2GlassEquiRectProperties::normalMap() const
{
    return m_normalMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
