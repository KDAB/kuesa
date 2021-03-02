
/*
    irodiffusehemiproperties.cpp

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

#include "irodiffusehemiproperties.h"
#include "irodiffusehemishaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::IroDiffuseHemiProperties
    \inheaderfile Kuesa/IroDiffuseHemiProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroDiffuseHemiProperties holds the properties controlling the visual appearance of a IroDiffuseHemiMaterial instance.
*/

/*!
    \qmltype IroDiffuseHemiProperties
    \instantiates Kuesa::IroDiffuseHemiProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroDiffuseHemiProperties holds the properties controlling the visual appearance of a IroDiffuseHemiMaterial instance.
*/

IroDiffuseHemiProperties::IroDiffuseHemiProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroDiffuseHemiShaderData(this))
    , m_reflectionMap(nullptr)
    , m_diffuseMap(nullptr)
{
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::normalScalingChanged, this, &IroDiffuseHemiProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::normalDisturbChanged, this, &IroDiffuseHemiProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::postVertexColorChanged, this, &IroDiffuseHemiProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::postHemiFilterChanged, this, &IroDiffuseHemiProperties::postHemiFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::postGainChanged, this, &IroDiffuseHemiProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::reflectionGainChanged, this, &IroDiffuseHemiProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::reflectionInnerFilterChanged, this, &IroDiffuseHemiProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::reflectionOuterFilterChanged, this, &IroDiffuseHemiProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::usesReflectionMapChanged, this, &IroDiffuseHemiProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::projectReflectionMapChanged, this, &IroDiffuseHemiProperties::projectReflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::diffuseInnerFilterChanged, this, &IroDiffuseHemiProperties::diffuseInnerFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::diffuseOuterFilterChanged, this, &IroDiffuseHemiProperties::diffuseOuterFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::diffuseGainChanged, this, &IroDiffuseHemiProperties::diffuseGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::usesDiffuseMapChanged, this, &IroDiffuseHemiProperties::usesDiffuseMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseHemiShaderData::gltfYUpChanged, this, &IroDiffuseHemiProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setReflectionMap(new Empty2DTexture());
    setDiffuseMap(new Empty2DTexture());
#endif
}

IroDiffuseHemiProperties::~IroDiffuseHemiProperties() = default;

Qt3DRender::QShaderData *IroDiffuseHemiProperties::shaderData() const
{
    return m_shaderData;
}

void IroDiffuseHemiProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void IroDiffuseHemiProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void IroDiffuseHemiProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroDiffuseHemiProperties::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    m_shaderData->setPostHemiFilter(postHemiFilter);
}

void IroDiffuseHemiProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroDiffuseHemiProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void IroDiffuseHemiProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void IroDiffuseHemiProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void IroDiffuseHemiProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void IroDiffuseHemiProperties::setProjectReflectionMap(bool projectReflectionMap)
{
    m_shaderData->setProjectReflectionMap(projectReflectionMap);
}

void IroDiffuseHemiProperties::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    m_shaderData->setDiffuseInnerFilter(diffuseInnerFilter);
}

void IroDiffuseHemiProperties::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    m_shaderData->setDiffuseOuterFilter(diffuseOuterFilter);
}

void IroDiffuseHemiProperties::setDiffuseGain(float diffuseGain)
{
    m_shaderData->setDiffuseGain(diffuseGain);
}

void IroDiffuseHemiProperties::setUsesDiffuseMap(bool usesDiffuseMap)
{
    m_shaderData->setUsesDiffuseMap(usesDiffuseMap);
}

void IroDiffuseHemiProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void IroDiffuseHemiProperties::setReflectionMap(Qt3DRender::QAbstractTexture *reflectionMap)
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
        d->registerDestructionHelper(m_reflectionMap, &IroDiffuseHemiProperties::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void IroDiffuseHemiProperties::setDiffuseMap(Qt3DRender::QAbstractTexture *diffuseMap)
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
        d->registerDestructionHelper(m_diffuseMap, &IroDiffuseHemiProperties::setDiffuseMap, m_diffuseMap);
    }
    emit diffuseMapChanged(m_diffuseMap);
}

/*!
    \qmlproperty QVector3D IroDiffuseHemiProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property IroDiffuseHemiProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D IroDiffuseHemiProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D IroDiffuseHemiProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property IroDiffuseHemiProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D IroDiffuseHemiProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float IroDiffuseHemiProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroDiffuseHemiProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroDiffuseHemiProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty QVector3D IroDiffuseHemiProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
/*!
    \property IroDiffuseHemiProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
QVector3D IroDiffuseHemiProperties::postHemiFilter() const
{
    return m_shaderData->postHemiFilter();
}

/*!
    \qmlproperty float IroDiffuseHemiProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property IroDiffuseHemiProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float IroDiffuseHemiProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty float IroDiffuseHemiProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property IroDiffuseHemiProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float IroDiffuseHemiProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D IroDiffuseHemiProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseHemiProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseHemiProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D IroDiffuseHemiProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseHemiProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseHemiProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty bool IroDiffuseHemiProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property IroDiffuseHemiProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool IroDiffuseHemiProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty bool IroDiffuseHemiProperties::projectReflectionMap
    Specifies whether equirectangular projection should be used for lookups on the reflection map.
*/
/*!
    \property IroDiffuseHemiProperties::projectReflectionMap
    Specifies whether equirectangular projection should be used for lookups on the reflection map.
*/
bool IroDiffuseHemiProperties::projectReflectionMap() const
{
    return m_shaderData->projectReflectionMap();
}

/*!
    \qmlproperty QVector3D IroDiffuseHemiProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property IroDiffuseHemiProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D IroDiffuseHemiProperties::diffuseInnerFilter() const
{
    return m_shaderData->diffuseInnerFilter();
}

/*!
    \qmlproperty QVector3D IroDiffuseHemiProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property IroDiffuseHemiProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D IroDiffuseHemiProperties::diffuseOuterFilter() const
{
    return m_shaderData->diffuseOuterFilter();
}

/*!
    \qmlproperty float IroDiffuseHemiProperties::diffuseGain
    Specifies the gain factor to be applied to the diffuse map lookup.
*/
/*!
    \property IroDiffuseHemiProperties::diffuseGain
    Specifies the gain factor to be applied to the diffuse map lookup.
*/
float IroDiffuseHemiProperties::diffuseGain() const
{
    return m_shaderData->diffuseGain();
}

/*!
    \qmlproperty bool IroDiffuseHemiProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
/*!
    \property IroDiffuseHemiProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
bool IroDiffuseHemiProperties::usesDiffuseMap() const
{
    return m_shaderData->usesDiffuseMap();
}

/*!
    \qmlproperty bool IroDiffuseHemiProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property IroDiffuseHemiProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool IroDiffuseHemiProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture *IroDiffuseHemiProperties::reflectionMap() const
{
    return m_reflectionMap;
}

Qt3DRender::QAbstractTexture *IroDiffuseHemiProperties::diffuseMap() const
{
    return m_diffuseMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
