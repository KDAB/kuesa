
/*
    iro2diffusesemproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "iro2diffusesemproperties.h"
#include "iro2diffusesemshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::Iro2DiffuseSemProperties
    \inheaderfile Kuesa/Iro2DiffuseSemProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseSemProperties holds the properties controlling the visual appearance of a Iro2DiffuseSemMaterial instance.
*/

/*!
    \qmltype Iro2DiffuseSemProperties
    \instantiates Kuesa::Iro2DiffuseSemProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseSemProperties holds the properties controlling the visual appearance of a Iro2DiffuseSemMaterial instance.
*/

Iro2DiffuseSemProperties::Iro2DiffuseSemProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2DiffuseSemShaderData(this))
    , m_diffuseMap(nullptr)
    , m_reflectionMap(nullptr)
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::usesDiffuseMapChanged, this, &Iro2DiffuseSemProperties::usesDiffuseMapChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::diffuseGainChanged, this, &Iro2DiffuseSemProperties::diffuseGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::diffuseInnerFilterChanged, this, &Iro2DiffuseSemProperties::diffuseInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::diffuseOuterFilterChanged, this, &Iro2DiffuseSemProperties::diffuseOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::usesReflectionMapChanged, this, &Iro2DiffuseSemProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::reflectionGainChanged, this, &Iro2DiffuseSemProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::reflectionInnerFilterChanged, this, &Iro2DiffuseSemProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::reflectionOuterFilterChanged, this, &Iro2DiffuseSemProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::usesNormalMapChanged, this, &Iro2DiffuseSemProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::normalMapGainChanged, this, &Iro2DiffuseSemProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::normalScalingChanged, this, &Iro2DiffuseSemProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::normalDisturbChanged, this, &Iro2DiffuseSemProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::postVertexColorChanged, this, &Iro2DiffuseSemProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::postHemiFilterChanged, this, &Iro2DiffuseSemProperties::postHemiFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::postGainChanged, this, &Iro2DiffuseSemProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseSemShaderData::gltfYUpChanged, this, &Iro2DiffuseSemProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setDiffuseMap(new Empty2DTexture());
    setReflectionMap(new Empty2DTexture());
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2DiffuseSemProperties::~Iro2DiffuseSemProperties() = default;

Qt3DRender::QShaderData *Iro2DiffuseSemProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2DiffuseSemProperties::setUsesDiffuseMap(bool usesDiffuseMap)
{
    m_shaderData->setUsesDiffuseMap(usesDiffuseMap);
}

void Iro2DiffuseSemProperties::setDiffuseGain(float diffuseGain)
{
    m_shaderData->setDiffuseGain(diffuseGain);
}

void Iro2DiffuseSemProperties::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    m_shaderData->setDiffuseInnerFilter(diffuseInnerFilter);
}

void Iro2DiffuseSemProperties::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    m_shaderData->setDiffuseOuterFilter(diffuseOuterFilter);
}

void Iro2DiffuseSemProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void Iro2DiffuseSemProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2DiffuseSemProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2DiffuseSemProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2DiffuseSemProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2DiffuseSemProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2DiffuseSemProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2DiffuseSemProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2DiffuseSemProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2DiffuseSemProperties::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    m_shaderData->setPostHemiFilter(postHemiFilter);
}

void Iro2DiffuseSemProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2DiffuseSemProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2DiffuseSemProperties::setDiffuseMap(Qt3DRender::QAbstractTexture * diffuseMap)
{
    if (m_diffuseMap == diffuseMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_diffuseMap != nullptr)
        d->unregisterDestructionHelper(m_diffuseMap);
    m_diffuseMap = diffuseMap;
    if (m_diffuseMap != nullptr) {
        if (m_diffuseMap->parent() == nullptr)
            m_diffuseMap->setParent(this);
        d->registerDestructionHelper(m_diffuseMap, &Iro2DiffuseSemProperties::setDiffuseMap, m_diffuseMap);
    }
    emit diffuseMapChanged(m_diffuseMap);
}

void Iro2DiffuseSemProperties::setReflectionMap(Qt3DRender::QAbstractTexture * reflectionMap)
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
        d->registerDestructionHelper(m_reflectionMap, &Iro2DiffuseSemProperties::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void Iro2DiffuseSemProperties::setNormalMap(Qt3DRender::QAbstractTexture * normalMap)
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
        d->registerDestructionHelper(m_normalMap, &Iro2DiffuseSemProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}


/*!
    \qmlproperty bool Iro2DiffuseSemProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
/*!
    \property Iro2DiffuseSemProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
bool Iro2DiffuseSemProperties::usesDiffuseMap() const
{
    return m_shaderData->usesDiffuseMap();
}

/*!
    \qmlproperty float Iro2DiffuseSemProperties::diffuseGain
    Specifies the gain to apply to the diffuse color.
*/
/*!
    \property Iro2DiffuseSemProperties::diffuseGain
    Specifies the gain to apply to the diffuse color.
*/
float Iro2DiffuseSemProperties::diffuseGain() const
{
    return m_shaderData->diffuseGain();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseSemProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property Iro2DiffuseSemProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D Iro2DiffuseSemProperties::diffuseInnerFilter() const
{
    return m_shaderData->diffuseInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseSemProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property Iro2DiffuseSemProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D Iro2DiffuseSemProperties::diffuseOuterFilter() const
{
    return m_shaderData->diffuseOuterFilter();
}

/*!
    \qmlproperty bool Iro2DiffuseSemProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property Iro2DiffuseSemProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool Iro2DiffuseSemProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty float Iro2DiffuseSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2DiffuseSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2DiffuseSemProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2DiffuseSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2DiffuseSemProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2DiffuseSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2DiffuseSemProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty bool Iro2DiffuseSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2DiffuseSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2DiffuseSemProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2DiffuseSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2DiffuseSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2DiffuseSemProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2DiffuseSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2DiffuseSemProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2DiffuseSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2DiffuseSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2DiffuseSemProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2DiffuseSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2DiffuseSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2DiffuseSemProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseSemProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
/*!
    \property Iro2DiffuseSemProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
QVector3D Iro2DiffuseSemProperties::postHemiFilter() const
{
    return m_shaderData->postHemiFilter();
}

/*!
    \qmlproperty float Iro2DiffuseSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2DiffuseSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2DiffuseSemProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2DiffuseSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2DiffuseSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2DiffuseSemProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture * Iro2DiffuseSemProperties::diffuseMap() const
{
    return m_diffuseMap;
}

Qt3DRender::QAbstractTexture * Iro2DiffuseSemProperties::reflectionMap() const
{
    return m_reflectionMap;
}

Qt3DRender::QAbstractTexture * Iro2DiffuseSemProperties::normalMap() const
{
    return m_normalMap;
}


} // namespace Kuesa

QT_END_NAMESPACE
