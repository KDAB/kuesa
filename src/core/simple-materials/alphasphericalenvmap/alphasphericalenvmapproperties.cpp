
/*
    alphasphericalenvmapproperties.cpp

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

#include "alphasphericalenvmapproperties.h"
#include "alphasphericalenvmapshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::AlphaSphericalEnvMapProperties
    \inheaderfile Kuesa/AlphaSphericalEnvMapProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::AlphaSphericalEnvMapProperties holds the properties controlling the visual appearance of a AlphaSphericalEnvMapMaterial instance
*/

/*!
    \qmltype AlphaSphericalEnvMapProperties
    \instantiates Kuesa::AlphaSphericalEnvMapProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::AlphaSphericalEnvMapProperties holds the properties controlling the visual appearance of a AlphaSphericalEnvMapMaterial instance
*/

AlphaSphericalEnvMapProperties::AlphaSphericalEnvMapProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new AlphaSphericalEnvMapShaderData(this))
{
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::factorsChanged, this, &AlphaSphericalEnvMapProperties::factorsChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::disturbationChanged, this, &AlphaSphericalEnvMapProperties::disturbationChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::postVertexColorChanged, this, &AlphaSphericalEnvMapProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::postGainChanged, this, &AlphaSphericalEnvMapProperties::postGainChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::semGainChanged, this, &AlphaSphericalEnvMapProperties::semGainChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::semChanged, this, &AlphaSphericalEnvMapProperties::semChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::semInnerFilterChanged, this, &AlphaSphericalEnvMapProperties::semInnerFilterChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::semOuterFilterChanged, this, &AlphaSphericalEnvMapProperties::semOuterFilterChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::semInnerAlphaChanged, this, &AlphaSphericalEnvMapProperties::semInnerAlphaChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::semOuterAlphaChanged, this, &AlphaSphericalEnvMapProperties::semOuterAlphaChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::glassInnerFilterChanged, this, &AlphaSphericalEnvMapProperties::glassInnerFilterChanged);
    QObject::connect(m_shaderData, &AlphaSphericalEnvMapShaderData::glassOuterFilterChanged, this, &AlphaSphericalEnvMapProperties::glassOuterFilterChanged);

}

AlphaSphericalEnvMapProperties::~AlphaSphericalEnvMapProperties() = default;

Qt3DRender::QShaderData *AlphaSphericalEnvMapProperties::shaderData() const
{
    return m_shaderData;
}

void AlphaSphericalEnvMapProperties::setFactors(const QVector3D &factors)
{
    m_shaderData->setFactors(factors);
}

void AlphaSphericalEnvMapProperties::setDisturbation(const QVector2D &disturbation)
{
    m_shaderData->setDisturbation(disturbation);
}

void AlphaSphericalEnvMapProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void AlphaSphericalEnvMapProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void AlphaSphericalEnvMapProperties::setSemGain(float semGain)
{
    m_shaderData->setSemGain(semGain);
}

void AlphaSphericalEnvMapProperties::setSem(Qt3DRender::QAbstractTexture * sem)
{
    m_shaderData->setSem(sem);
}

void AlphaSphericalEnvMapProperties::setSemInnerFilter(const QVector3D &semInnerFilter)
{
    m_shaderData->setSemInnerFilter(semInnerFilter);
}

void AlphaSphericalEnvMapProperties::setSemOuterFilter(const QVector3D &semOuterFilter)
{
    m_shaderData->setSemOuterFilter(semOuterFilter);
}

void AlphaSphericalEnvMapProperties::setSemInnerAlpha(float semInnerAlpha)
{
    m_shaderData->setSemInnerAlpha(semInnerAlpha);
}

void AlphaSphericalEnvMapProperties::setSemOuterAlpha(float semOuterAlpha)
{
    m_shaderData->setSemOuterAlpha(semOuterAlpha);
}

void AlphaSphericalEnvMapProperties::setGlassInnerFilter(const QVector3D &glassInnerFilter)
{
    m_shaderData->setGlassInnerFilter(glassInnerFilter);
}

void AlphaSphericalEnvMapProperties::setGlassOuterFilter(const QVector3D &glassOuterFilter)
{
    m_shaderData->setGlassOuterFilter(glassOuterFilter);
}


/*!
    \qmlproperty QVector3D AlphaSphericalEnvMapProperties::factors
    Specifies a factor to be applied to the normal.
*/
/*!
    \property AlphaSphericalEnvMapProperties::factors
    Specifies a factor to be applied to the normal.
*/
QVector3D AlphaSphericalEnvMapProperties::factors() const
{
    return m_shaderData->factors();
}

/*!
    \qmlproperty QVector2D AlphaSphericalEnvMapProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property AlphaSphericalEnvMapProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D AlphaSphericalEnvMapProperties::disturbation() const
{
    return m_shaderData->disturbation();
}

/*!
    \qmlproperty float AlphaSphericalEnvMapProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property AlphaSphericalEnvMapProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float AlphaSphericalEnvMapProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float AlphaSphericalEnvMapProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
/*!
    \property AlphaSphericalEnvMapProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
float AlphaSphericalEnvMapProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty float AlphaSphericalEnvMapProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property AlphaSphericalEnvMapProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float AlphaSphericalEnvMapProperties::semGain() const
{
    return m_shaderData->semGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * AlphaSphericalEnvMapProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
/*!
    \property AlphaSphericalEnvMapProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * AlphaSphericalEnvMapProperties::sem() const
{
    return m_shaderData->sem();
}

/*!
    \qmlproperty QVector3D AlphaSphericalEnvMapProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property AlphaSphericalEnvMapProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D AlphaSphericalEnvMapProperties::semInnerFilter() const
{
    return m_shaderData->semInnerFilter();
}

/*!
    \qmlproperty QVector3D AlphaSphericalEnvMapProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property AlphaSphericalEnvMapProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D AlphaSphericalEnvMapProperties::semOuterFilter() const
{
    return m_shaderData->semOuterFilter();
}

/*!
    \qmlproperty float AlphaSphericalEnvMapProperties::semInnerAlpha
    Specifies the inner alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property AlphaSphericalEnvMapProperties::semInnerAlpha
    Specifies the inner alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
float AlphaSphericalEnvMapProperties::semInnerAlpha() const
{
    return m_shaderData->semInnerAlpha();
}

/*!
    \qmlproperty float AlphaSphericalEnvMapProperties::semOuterAlpha
    Specifies the outer alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property AlphaSphericalEnvMapProperties::semOuterAlpha
    Specifies the outer alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
float AlphaSphericalEnvMapProperties::semOuterAlpha() const
{
    return m_shaderData->semOuterAlpha();
}

/*!
    \qmlproperty QVector3D AlphaSphericalEnvMapProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
/*!
    \property AlphaSphericalEnvMapProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
QVector3D AlphaSphericalEnvMapProperties::glassInnerFilter() const
{
    return m_shaderData->glassInnerFilter();
}

/*!
    \qmlproperty QVector3D AlphaSphericalEnvMapProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
/*!
    \property AlphaSphericalEnvMapProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
QVector3D AlphaSphericalEnvMapProperties::glassOuterFilter() const
{
    return m_shaderData->glassOuterFilter();
}


} // namespace Kuesa

QT_END_NAMESPACE
