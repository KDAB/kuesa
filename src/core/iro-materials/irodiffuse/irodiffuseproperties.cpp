
/*
    irodiffuseproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "irodiffuseproperties.h"
#include "irodiffuseshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroDiffuseProperties
    \inheaderfile Kuesa/IroDiffuseProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroDiffuseProperties holds the properties controlling the visual appearance of a IroDiffuseMaterial instance
*/

/*!
    \qmltype IroDiffuseProperties
    \instantiates Kuesa::IroDiffuseProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroDiffuseProperties holds the properties controlling the visual appearance of a IroDiffuseMaterial instance
*/

IroDiffuseProperties::IroDiffuseProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroDiffuseShaderData(this))
{
    QObject::connect(m_shaderData, &IroDiffuseShaderData::normalScalingChanged, this, &IroDiffuseProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::normalDisturbChanged, this, &IroDiffuseProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::postVertexColorChanged, this, &IroDiffuseProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::postGainChanged, this, &IroDiffuseProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::reflectionGainChanged, this, &IroDiffuseProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::reflectionMapChanged, this, &IroDiffuseProperties::reflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::reflectionInnerFilterChanged, this, &IroDiffuseProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::reflectionOuterFilterChanged, this, &IroDiffuseProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::usesReflectionMapChanged, this, &IroDiffuseProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::projectReflectionMapChanged, this, &IroDiffuseProperties::projectReflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseInnerFilterChanged, this, &IroDiffuseProperties::diffuseInnerFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseOuterFilterChanged, this, &IroDiffuseProperties::diffuseOuterFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseMapChanged, this, &IroDiffuseProperties::diffuseMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseGainChanged, this, &IroDiffuseProperties::diffuseGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::usesDiffuseMapChanged, this, &IroDiffuseProperties::usesDiffuseMapChanged);

}

IroDiffuseProperties::~IroDiffuseProperties() = default;

Qt3DRender::QShaderData *IroDiffuseProperties::shaderData() const
{
    return m_shaderData;
}

void IroDiffuseProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void IroDiffuseProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void IroDiffuseProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroDiffuseProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroDiffuseProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void IroDiffuseProperties::setReflectionMap(Qt3DRender::QAbstractTexture * reflectionMap)
{
    m_shaderData->setReflectionMap(reflectionMap);
}

void IroDiffuseProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void IroDiffuseProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void IroDiffuseProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void IroDiffuseProperties::setProjectReflectionMap(bool projectReflectionMap)
{
    m_shaderData->setProjectReflectionMap(projectReflectionMap);
}

void IroDiffuseProperties::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    m_shaderData->setDiffuseInnerFilter(diffuseInnerFilter);
}

void IroDiffuseProperties::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    m_shaderData->setDiffuseOuterFilter(diffuseOuterFilter);
}

void IroDiffuseProperties::setDiffuseMap(Qt3DRender::QAbstractTexture * diffuseMap)
{
    m_shaderData->setDiffuseMap(diffuseMap);
}

void IroDiffuseProperties::setDiffuseGain(float diffuseGain)
{
    m_shaderData->setDiffuseGain(diffuseGain);
}

void IroDiffuseProperties::setUsesDiffuseMap(bool usesDiffuseMap)
{
    m_shaderData->setUsesDiffuseMap(usesDiffuseMap);
}


/*!
    \qmlproperty QVector3D IroDiffuseProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property IroDiffuseProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D IroDiffuseProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D IroDiffuseProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property IroDiffuseProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D IroDiffuseProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float IroDiffuseProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroDiffuseProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroDiffuseProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroDiffuseProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
/*!
    \property IroDiffuseProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
float IroDiffuseProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty float IroDiffuseProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property IroDiffuseProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float IroDiffuseProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * IroDiffuseProperties::reflectionMap
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
/*!
    \property IroDiffuseProperties::reflectionMap
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * IroDiffuseProperties::reflectionMap() const
{
    return m_shaderData->reflectionMap();
}

/*!
    \qmlproperty QVector3D IroDiffuseProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D IroDiffuseProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty bool IroDiffuseProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property IroDiffuseProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool IroDiffuseProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty bool IroDiffuseProperties::projectReflectionMap
    Specifies whether equirectangular projection should be used for lookups on the reflection map.
*/
/*!
    \property IroDiffuseProperties::projectReflectionMap
    Specifies whether equirectangular projection should be used for lookups on the reflection map.
*/
bool IroDiffuseProperties::projectReflectionMap() const
{
    return m_shaderData->projectReflectionMap();
}

/*!
    \qmlproperty QVector3D IroDiffuseProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property IroDiffuseProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D IroDiffuseProperties::diffuseInnerFilter() const
{
    return m_shaderData->diffuseInnerFilter();
}

/*!
    \qmlproperty QVector3D IroDiffuseProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property IroDiffuseProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D IroDiffuseProperties::diffuseOuterFilter() const
{
    return m_shaderData->diffuseOuterFilter();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * IroDiffuseProperties::diffuseMap
    Specifies the diffuse map to use. It is expected to be in sRGB color space.
*/
/*!
    \property IroDiffuseProperties::diffuseMap
    Specifies the diffuse map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * IroDiffuseProperties::diffuseMap() const
{
    return m_shaderData->diffuseMap();
}

/*!
    \qmlproperty float IroDiffuseProperties::diffuseGain
    Specifies the gain to apply to the diffuse color.
*/
/*!
    \property IroDiffuseProperties::diffuseGain
    Specifies the gain to apply to the diffuse color.
*/
float IroDiffuseProperties::diffuseGain() const
{
    return m_shaderData->diffuseGain();
}

/*!
    \qmlproperty bool IroDiffuseProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
/*!
    \property IroDiffuseProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
bool IroDiffuseProperties::usesDiffuseMap() const
{
    return m_shaderData->usesDiffuseMap();
}


} // namespace Kuesa

QT_END_NAMESPACE
