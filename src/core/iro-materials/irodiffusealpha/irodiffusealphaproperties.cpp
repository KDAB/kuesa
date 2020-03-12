
/*
    irodiffusealphaproperties.cpp

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

#include "irodiffusealphaproperties.h"
#include "irodiffusealphashaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroDiffuseAlphaProperties
    \inheaderfile Kuesa/IroDiffuseAlphaProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroDiffuseAlphaProperties holds the properties controlling the visual appearance of a IroDiffuseAlphaMaterial instance
*/

/*!
    \qmltype IroDiffuseAlphaProperties
    \instantiates Kuesa::IroDiffuseAlphaProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroDiffuseAlphaProperties holds the properties controlling the visual appearance of a IroDiffuseAlphaMaterial instance
*/

IroDiffuseAlphaProperties::IroDiffuseAlphaProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroDiffuseAlphaShaderData(this))
{
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::normalScalingChanged, this, &IroDiffuseAlphaProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::normalDisturbChanged, this, &IroDiffuseAlphaProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::postVertexColorChanged, this, &IroDiffuseAlphaProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::postGainChanged, this, &IroDiffuseAlphaProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::reflectionGainChanged, this, &IroDiffuseAlphaProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::reflectionMapChanged, this, &IroDiffuseAlphaProperties::reflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::reflectionInnerFilterChanged, this, &IroDiffuseAlphaProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::reflectionOuterFilterChanged, this, &IroDiffuseAlphaProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::usesReflectionMapChanged, this, &IroDiffuseAlphaProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::projectReflectionMapChanged, this, &IroDiffuseAlphaProperties::projectReflectionMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::reflectionInnerAlphaChanged, this, &IroDiffuseAlphaProperties::reflectionInnerAlphaChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::reflectionOuterAlphaChanged, this, &IroDiffuseAlphaProperties::reflectionOuterAlphaChanged);
    QObject::connect(m_shaderData, &IroDiffuseAlphaShaderData::alphaGainChanged, this, &IroDiffuseAlphaProperties::alphaGainChanged);

}

IroDiffuseAlphaProperties::~IroDiffuseAlphaProperties() = default;

Qt3DRender::QShaderData *IroDiffuseAlphaProperties::shaderData() const
{
    return m_shaderData;
}

void IroDiffuseAlphaProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void IroDiffuseAlphaProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void IroDiffuseAlphaProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroDiffuseAlphaProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroDiffuseAlphaProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void IroDiffuseAlphaProperties::setReflectionMap(Qt3DRender::QAbstractTexture * reflectionMap)
{
    m_shaderData->setReflectionMap(reflectionMap);
}

void IroDiffuseAlphaProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void IroDiffuseAlphaProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void IroDiffuseAlphaProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void IroDiffuseAlphaProperties::setProjectReflectionMap(bool projectReflectionMap)
{
    m_shaderData->setProjectReflectionMap(projectReflectionMap);
}

void IroDiffuseAlphaProperties::setReflectionInnerAlpha(float reflectionInnerAlpha)
{
    m_shaderData->setReflectionInnerAlpha(reflectionInnerAlpha);
}

void IroDiffuseAlphaProperties::setReflectionOuterAlpha(float reflectionOuterAlpha)
{
    m_shaderData->setReflectionOuterAlpha(reflectionOuterAlpha);
}

void IroDiffuseAlphaProperties::setAlphaGain(float alphaGain)
{
    m_shaderData->setAlphaGain(alphaGain);
}


/*!
    \qmlproperty QVector3D IroDiffuseAlphaProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property IroDiffuseAlphaProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D IroDiffuseAlphaProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D IroDiffuseAlphaProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property IroDiffuseAlphaProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D IroDiffuseAlphaProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float IroDiffuseAlphaProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroDiffuseAlphaProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroDiffuseAlphaProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroDiffuseAlphaProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
/*!
    \property IroDiffuseAlphaProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
float IroDiffuseAlphaProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty float IroDiffuseAlphaProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property IroDiffuseAlphaProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float IroDiffuseAlphaProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * IroDiffuseAlphaProperties::reflectionMap
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
/*!
    \property IroDiffuseAlphaProperties::reflectionMap
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * IroDiffuseAlphaProperties::reflectionMap() const
{
    return m_shaderData->reflectionMap();
}

/*!
    \qmlproperty QVector3D IroDiffuseAlphaProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseAlphaProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseAlphaProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D IroDiffuseAlphaProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseAlphaProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseAlphaProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty bool IroDiffuseAlphaProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property IroDiffuseAlphaProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool IroDiffuseAlphaProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty bool IroDiffuseAlphaProperties::projectReflectionMap
    Specifies whether equirectangular projection should be used for lookups on the reflection map.
*/
/*!
    \property IroDiffuseAlphaProperties::projectReflectionMap
    Specifies whether equirectangular projection should be used for lookups on the reflection map.
*/
bool IroDiffuseAlphaProperties::projectReflectionMap() const
{
    return m_shaderData->projectReflectionMap();
}

/*!
    \qmlproperty float IroDiffuseAlphaProperties::reflectionInnerAlpha
    Specifies the inner alpha factor to be applied.
*/
/*!
    \property IroDiffuseAlphaProperties::reflectionInnerAlpha
    Specifies the inner alpha factor to be applied.
*/
float IroDiffuseAlphaProperties::reflectionInnerAlpha() const
{
    return m_shaderData->reflectionInnerAlpha();
}

/*!
    \qmlproperty float IroDiffuseAlphaProperties::reflectionOuterAlpha
    Specifies the outer alpha factor to be applied.
*/
/*!
    \property IroDiffuseAlphaProperties::reflectionOuterAlpha
    Specifies the outer alpha factor to be applied.
*/
float IroDiffuseAlphaProperties::reflectionOuterAlpha() const
{
    return m_shaderData->reflectionOuterAlpha();
}

/*!
    \qmlproperty float IroDiffuseAlphaProperties::alphaGain
    Specifies the gain to apply to the alpha of the reflection map lookup.
*/
/*!
    \property IroDiffuseAlphaProperties::alphaGain
    Specifies the gain to apply to the alpha of the reflection map lookup.
*/
float IroDiffuseAlphaProperties::alphaGain() const
{
    return m_shaderData->alphaGain();
}


} // namespace Kuesa

QT_END_NAMESPACE
