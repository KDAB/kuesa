
/*
    irodiffuseproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    QObject::connect(m_shaderData, &IroDiffuseShaderData::factorsChanged, this, &IroDiffuseProperties::factorsChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::disturbationChanged, this, &IroDiffuseProperties::disturbationChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::postVertexColorChanged, this, &IroDiffuseProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::postGainChanged, this, &IroDiffuseProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::semGainChanged, this, &IroDiffuseProperties::semGainChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::semChanged, this, &IroDiffuseProperties::semChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::semInnerFilterChanged, this, &IroDiffuseProperties::semInnerFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::semOuterFilterChanged, this, &IroDiffuseProperties::semOuterFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseInnerFilterChanged, this, &IroDiffuseProperties::diffuseInnerFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseOuterFilterChanged, this, &IroDiffuseProperties::diffuseOuterFilterChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseMapChanged, this, &IroDiffuseProperties::diffuseMapChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::diffuseFactorChanged, this, &IroDiffuseProperties::diffuseFactorChanged);
    QObject::connect(m_shaderData, &IroDiffuseShaderData::usesDiffuseMapChanged, this, &IroDiffuseProperties::usesDiffuseMapChanged);

}

IroDiffuseProperties::~IroDiffuseProperties() = default;

Qt3DRender::QShaderData *IroDiffuseProperties::shaderData() const
{
    return m_shaderData;
}

void IroDiffuseProperties::setFactors(const QVector3D &factors)
{
    m_shaderData->setFactors(factors);
}

void IroDiffuseProperties::setDisturbation(const QVector2D &disturbation)
{
    m_shaderData->setDisturbation(disturbation);
}

void IroDiffuseProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroDiffuseProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroDiffuseProperties::setSemGain(float semGain)
{
    m_shaderData->setSemGain(semGain);
}

void IroDiffuseProperties::setSem(Qt3DRender::QAbstractTexture * sem)
{
    m_shaderData->setSem(sem);
}

void IroDiffuseProperties::setSemInnerFilter(const QVector3D &semInnerFilter)
{
    m_shaderData->setSemInnerFilter(semInnerFilter);
}

void IroDiffuseProperties::setSemOuterFilter(const QVector3D &semOuterFilter)
{
    m_shaderData->setSemOuterFilter(semOuterFilter);
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

void IroDiffuseProperties::setDiffuseFactor(const QVector3D &diffuseFactor)
{
    m_shaderData->setDiffuseFactor(diffuseFactor);
}

void IroDiffuseProperties::setUsesDiffuseMap(bool usesDiffuseMap)
{
    m_shaderData->setUsesDiffuseMap(usesDiffuseMap);
}


/*!
    \qmlproperty QVector3D IroDiffuseProperties::factors
    Specifies a factor to be applied to the normal.
*/
/*!
    \property IroDiffuseProperties::factors
    Specifies a factor to be applied to the normal.
*/
QVector3D IroDiffuseProperties::factors() const
{
    return m_shaderData->factors();
}

/*!
    \qmlproperty QVector2D IroDiffuseProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property IroDiffuseProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D IroDiffuseProperties::disturbation() const
{
    return m_shaderData->disturbation();
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
    \qmlproperty float IroDiffuseProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property IroDiffuseProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float IroDiffuseProperties::semGain() const
{
    return m_shaderData->semGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * IroDiffuseProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
/*!
    \property IroDiffuseProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * IroDiffuseProperties::sem() const
{
    return m_shaderData->sem();
}

/*!
    \qmlproperty QVector3D IroDiffuseProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseProperties::semInnerFilter() const
{
    return m_shaderData->semInnerFilter();
}

/*!
    \qmlproperty QVector3D IroDiffuseProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroDiffuseProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroDiffuseProperties::semOuterFilter() const
{
    return m_shaderData->semOuterFilter();
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
    \qmlproperty QVector3D IroDiffuseProperties::diffuseFactor
    Specifies the diffuse color to use in case no diffuse map was specified.
*/
/*!
    \property IroDiffuseProperties::diffuseFactor
    Specifies the diffuse color to use in case no diffuse map was specified.
*/
QVector3D IroDiffuseProperties::diffuseFactor() const
{
    return m_shaderData->diffuseFactor();
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
