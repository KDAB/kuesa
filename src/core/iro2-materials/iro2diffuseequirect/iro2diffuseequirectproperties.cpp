
/*
    iro2diffuseequirectproperties.cpp

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

#include "iro2diffuseequirectproperties.h"
#include "iro2diffuseequirectshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::Iro2DiffuseEquiRectProperties
    \inheaderfile Kuesa/Iro2DiffuseEquiRectProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseEquiRectProperties holds the properties controlling the visual appearance of a Iro2DiffuseEquiRectMaterial instance.
*/

/*!
    \qmltype Iro2DiffuseEquiRectProperties
    \instantiates Kuesa::Iro2DiffuseEquiRectProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseEquiRectProperties holds the properties controlling the visual appearance of a Iro2DiffuseEquiRectMaterial instance.
*/

Iro2DiffuseEquiRectProperties::Iro2DiffuseEquiRectProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2DiffuseEquiRectShaderData(this))
    , m_diffuseMap(nullptr)
    , m_reflectionMap(nullptr)
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::usesDiffuseMapChanged, this, &Iro2DiffuseEquiRectProperties::usesDiffuseMapChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::diffuseGainChanged, this, &Iro2DiffuseEquiRectProperties::diffuseGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::diffuseInnerFilterChanged, this, &Iro2DiffuseEquiRectProperties::diffuseInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::diffuseOuterFilterChanged, this, &Iro2DiffuseEquiRectProperties::diffuseOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::usesReflectionMapChanged, this, &Iro2DiffuseEquiRectProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::reflectionGainChanged, this, &Iro2DiffuseEquiRectProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::reflectionInnerFilterChanged, this, &Iro2DiffuseEquiRectProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::reflectionOuterFilterChanged, this, &Iro2DiffuseEquiRectProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::usesNormalMapChanged, this, &Iro2DiffuseEquiRectProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::normalMapGainChanged, this, &Iro2DiffuseEquiRectProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::normalScalingChanged, this, &Iro2DiffuseEquiRectProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::normalDisturbChanged, this, &Iro2DiffuseEquiRectProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::postVertexColorChanged, this, &Iro2DiffuseEquiRectProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::postHemiFilterChanged, this, &Iro2DiffuseEquiRectProperties::postHemiFilterChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::postGainChanged, this, &Iro2DiffuseEquiRectProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2DiffuseEquiRectShaderData::gltfYUpChanged, this, &Iro2DiffuseEquiRectProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setDiffuseMap(new Empty2DTexture());
    setReflectionMap(new Empty2DTexture());
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2DiffuseEquiRectProperties::~Iro2DiffuseEquiRectProperties() = default;

Qt3DRender::QShaderData *Iro2DiffuseEquiRectProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2DiffuseEquiRectProperties::setUsesDiffuseMap(bool usesDiffuseMap)
{
    m_shaderData->setUsesDiffuseMap(usesDiffuseMap);
}

void Iro2DiffuseEquiRectProperties::setDiffuseGain(float diffuseGain)
{
    m_shaderData->setDiffuseGain(diffuseGain);
}

void Iro2DiffuseEquiRectProperties::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    m_shaderData->setDiffuseInnerFilter(diffuseInnerFilter);
}

void Iro2DiffuseEquiRectProperties::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    m_shaderData->setDiffuseOuterFilter(diffuseOuterFilter);
}

void Iro2DiffuseEquiRectProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void Iro2DiffuseEquiRectProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2DiffuseEquiRectProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2DiffuseEquiRectProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2DiffuseEquiRectProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2DiffuseEquiRectProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2DiffuseEquiRectProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2DiffuseEquiRectProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2DiffuseEquiRectProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2DiffuseEquiRectProperties::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    m_shaderData->setPostHemiFilter(postHemiFilter);
}

void Iro2DiffuseEquiRectProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2DiffuseEquiRectProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2DiffuseEquiRectProperties::setDiffuseMap(Qt3DRender::QAbstractTexture *diffuseMap)
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
        d->registerDestructionHelper(m_diffuseMap, &Iro2DiffuseEquiRectProperties::setDiffuseMap, m_diffuseMap);
    }
    emit diffuseMapChanged(m_diffuseMap);
}

void Iro2DiffuseEquiRectProperties::setReflectionMap(Qt3DRender::QAbstractTexture *reflectionMap)
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
        d->registerDestructionHelper(m_reflectionMap, &Iro2DiffuseEquiRectProperties::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void Iro2DiffuseEquiRectProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
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
        d->registerDestructionHelper(m_normalMap, &Iro2DiffuseEquiRectProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}

/*!
    \qmlproperty bool Iro2DiffuseEquiRectProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
bool Iro2DiffuseEquiRectProperties::usesDiffuseMap() const
{
    return m_shaderData->usesDiffuseMap();
}

/*!
    \qmlproperty float Iro2DiffuseEquiRectProperties::diffuseGain
    Specifies the gain to apply to the diffuse color.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::diffuseGain
    Specifies the gain to apply to the diffuse color.
*/
float Iro2DiffuseEquiRectProperties::diffuseGain() const
{
    return m_shaderData->diffuseGain();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseEquiRectProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D Iro2DiffuseEquiRectProperties::diffuseInnerFilter() const
{
    return m_shaderData->diffuseInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseEquiRectProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D Iro2DiffuseEquiRectProperties::diffuseOuterFilter() const
{
    return m_shaderData->diffuseOuterFilter();
}

/*!
    \qmlproperty bool Iro2DiffuseEquiRectProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool Iro2DiffuseEquiRectProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty float Iro2DiffuseEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2DiffuseEquiRectProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2DiffuseEquiRectProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2DiffuseEquiRectProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty bool Iro2DiffuseEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2DiffuseEquiRectProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2DiffuseEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2DiffuseEquiRectProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2DiffuseEquiRectProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2DiffuseEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2DiffuseEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2DiffuseEquiRectProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2DiffuseEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2DiffuseEquiRectProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty QVector3D Iro2DiffuseEquiRectProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
QVector3D Iro2DiffuseEquiRectProperties::postHemiFilter() const
{
    return m_shaderData->postHemiFilter();
}

/*!
    \qmlproperty float Iro2DiffuseEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2DiffuseEquiRectProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2DiffuseEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2DiffuseEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2DiffuseEquiRectProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture *Iro2DiffuseEquiRectProperties::diffuseMap() const
{
    return m_diffuseMap;
}

Qt3DRender::QAbstractTexture *Iro2DiffuseEquiRectProperties::reflectionMap() const
{
    return m_reflectionMap;
}

Qt3DRender::QAbstractTexture *Iro2DiffuseEquiRectProperties::normalMap() const
{
    return m_normalMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
