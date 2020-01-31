
/*
    iroglassaddproperties.cpp

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

#include "iroglassaddproperties.h"
#include "iroglassaddshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroGlassAddProperties
    \inheaderfile Kuesa/IroGlassAddProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroGlassAddProperties holds the properties controlling the visual appearance of a IroGlassAddMaterial instance
*/

/*!
    \qmltype IroGlassAddProperties
    \instantiates Kuesa::IroGlassAddProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroGlassAddProperties holds the properties controlling the visual appearance of a IroGlassAddMaterial instance
*/

IroGlassAddProperties::IroGlassAddProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroGlassAddShaderData(this))
{
    QObject::connect(m_shaderData, &IroGlassAddShaderData::factorsChanged, this, &IroGlassAddProperties::factorsChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::disturbationChanged, this, &IroGlassAddProperties::disturbationChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::postVertexColorChanged, this, &IroGlassAddProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::postGainChanged, this, &IroGlassAddProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::semGainChanged, this, &IroGlassAddProperties::semGainChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::semChanged, this, &IroGlassAddProperties::semChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::semInnerFilterChanged, this, &IroGlassAddProperties::semInnerFilterChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::semOuterFilterChanged, this, &IroGlassAddProperties::semOuterFilterChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::semInnerAlphaChanged, this, &IroGlassAddProperties::semInnerAlphaChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::semOuterAlphaChanged, this, &IroGlassAddProperties::semOuterAlphaChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::glassInnerFilterChanged, this, &IroGlassAddProperties::glassInnerFilterChanged);
    QObject::connect(m_shaderData, &IroGlassAddShaderData::glassOuterFilterChanged, this, &IroGlassAddProperties::glassOuterFilterChanged);

}

IroGlassAddProperties::~IroGlassAddProperties() = default;

Qt3DRender::QShaderData *IroGlassAddProperties::shaderData() const
{
    return m_shaderData;
}

void IroGlassAddProperties::setFactors(const QVector3D &factors)
{
    m_shaderData->setFactors(factors);
}

void IroGlassAddProperties::setDisturbation(const QVector2D &disturbation)
{
    m_shaderData->setDisturbation(disturbation);
}

void IroGlassAddProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroGlassAddProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroGlassAddProperties::setSemGain(float semGain)
{
    m_shaderData->setSemGain(semGain);
}

void IroGlassAddProperties::setSem(Qt3DRender::QAbstractTexture * sem)
{
    m_shaderData->setSem(sem);
}

void IroGlassAddProperties::setSemInnerFilter(const QVector3D &semInnerFilter)
{
    m_shaderData->setSemInnerFilter(semInnerFilter);
}

void IroGlassAddProperties::setSemOuterFilter(const QVector3D &semOuterFilter)
{
    m_shaderData->setSemOuterFilter(semOuterFilter);
}

void IroGlassAddProperties::setSemInnerAlpha(float semInnerAlpha)
{
    m_shaderData->setSemInnerAlpha(semInnerAlpha);
}

void IroGlassAddProperties::setSemOuterAlpha(float semOuterAlpha)
{
    m_shaderData->setSemOuterAlpha(semOuterAlpha);
}

void IroGlassAddProperties::setGlassInnerFilter(const QVector3D &glassInnerFilter)
{
    m_shaderData->setGlassInnerFilter(glassInnerFilter);
}

void IroGlassAddProperties::setGlassOuterFilter(const QVector3D &glassOuterFilter)
{
    m_shaderData->setGlassOuterFilter(glassOuterFilter);
}


/*!
    \qmlproperty QVector3D IroGlassAddProperties::factors
    Specifies a factor to be applied to the normal.
*/
/*!
    \property IroGlassAddProperties::factors
    Specifies a factor to be applied to the normal.
*/
QVector3D IroGlassAddProperties::factors() const
{
    return m_shaderData->factors();
}

/*!
    \qmlproperty QVector2D IroGlassAddProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property IroGlassAddProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D IroGlassAddProperties::disturbation() const
{
    return m_shaderData->disturbation();
}

/*!
    \qmlproperty float IroGlassAddProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroGlassAddProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroGlassAddProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroGlassAddProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
/*!
    \property IroGlassAddProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
float IroGlassAddProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty float IroGlassAddProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property IroGlassAddProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float IroGlassAddProperties::semGain() const
{
    return m_shaderData->semGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * IroGlassAddProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
/*!
    \property IroGlassAddProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * IroGlassAddProperties::sem() const
{
    return m_shaderData->sem();
}

/*!
    \qmlproperty QVector3D IroGlassAddProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroGlassAddProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroGlassAddProperties::semInnerFilter() const
{
    return m_shaderData->semInnerFilter();
}

/*!
    \qmlproperty QVector3D IroGlassAddProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroGlassAddProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D IroGlassAddProperties::semOuterFilter() const
{
    return m_shaderData->semOuterFilter();
}

/*!
    \qmlproperty float IroGlassAddProperties::semInnerAlpha
    Specifies the inner alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroGlassAddProperties::semInnerAlpha
    Specifies the inner alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
float IroGlassAddProperties::semInnerAlpha() const
{
    return m_shaderData->semInnerAlpha();
}

/*!
    \qmlproperty float IroGlassAddProperties::semOuterAlpha
    Specifies the outer alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property IroGlassAddProperties::semOuterAlpha
    Specifies the outer alpha filter to be applied based on fresnel on the spherical environment map lookup.
*/
float IroGlassAddProperties::semOuterAlpha() const
{
    return m_shaderData->semOuterAlpha();
}

/*!
    \qmlproperty QVector3D IroGlassAddProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
/*!
    \property IroGlassAddProperties::glassInnerFilter
    Specifies the glass inner color filter to be applied based on fresnel.
*/
QVector3D IroGlassAddProperties::glassInnerFilter() const
{
    return m_shaderData->glassInnerFilter();
}

/*!
    \qmlproperty QVector3D IroGlassAddProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
/*!
    \property IroGlassAddProperties::glassOuterFilter
    Specifies the glass outer color filter to be applied based on fresnel.
*/
QVector3D IroGlassAddProperties::glassOuterFilter() const
{
    return m_shaderData->glassOuterFilter();
}


} // namespace Kuesa

QT_END_NAMESPACE
