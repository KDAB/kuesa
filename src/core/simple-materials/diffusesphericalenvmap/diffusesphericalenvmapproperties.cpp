
/*
    diffusesphericalenvmapproperties.cpp

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

#include "diffusesphericalenvmapproperties.h"
#include "diffusesphericalenvmapshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::DiffuseSphericalEnvMapProperties
    \inheaderfile Kuesa/DiffuseSphericalEnvMapProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::DiffuseSphericalEnvMapProperties holds the properties controlling the visual appearance of a DiffuseSphericalEnvMapMaterial instance
*/

/*!
    \qmltype DiffuseSphericalEnvMapProperties
    \instantiates Kuesa::DiffuseSphericalEnvMapProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::DiffuseSphericalEnvMapProperties holds the properties controlling the visual appearance of a DiffuseSphericalEnvMapMaterial instance
*/

DiffuseSphericalEnvMapProperties::DiffuseSphericalEnvMapProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new DiffuseSphericalEnvMapShaderData(this))
{
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::factorsChanged, this, &DiffuseSphericalEnvMapProperties::factorsChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::disturbationChanged, this, &DiffuseSphericalEnvMapProperties::disturbationChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::postVertexColorChanged, this, &DiffuseSphericalEnvMapProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::postGainChanged, this, &DiffuseSphericalEnvMapProperties::postGainChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::semGainChanged, this, &DiffuseSphericalEnvMapProperties::semGainChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::semChanged, this, &DiffuseSphericalEnvMapProperties::semChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::semInnerFilterChanged, this, &DiffuseSphericalEnvMapProperties::semInnerFilterChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::semOuterFilterChanged, this, &DiffuseSphericalEnvMapProperties::semOuterFilterChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::diffuseInnerFilterChanged, this, &DiffuseSphericalEnvMapProperties::diffuseInnerFilterChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::diffuseOuterFilterChanged, this, &DiffuseSphericalEnvMapProperties::diffuseOuterFilterChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::diffuseMapChanged, this, &DiffuseSphericalEnvMapProperties::diffuseMapChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::diffuseFactorChanged, this, &DiffuseSphericalEnvMapProperties::diffuseFactorChanged);
    QObject::connect(m_shaderData, &DiffuseSphericalEnvMapShaderData::usesDiffuseMapChanged, this, &DiffuseSphericalEnvMapProperties::usesDiffuseMapChanged);

}

DiffuseSphericalEnvMapProperties::~DiffuseSphericalEnvMapProperties() = default;

Qt3DRender::QShaderData *DiffuseSphericalEnvMapProperties::shaderData() const
{
    return m_shaderData;
}

void DiffuseSphericalEnvMapProperties::setFactors(const QVector3D &factors)
{
    m_shaderData->setFactors(factors);
}

void DiffuseSphericalEnvMapProperties::setDisturbation(const QVector2D &disturbation)
{
    m_shaderData->setDisturbation(disturbation);
}

void DiffuseSphericalEnvMapProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void DiffuseSphericalEnvMapProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void DiffuseSphericalEnvMapProperties::setSemGain(float semGain)
{
    m_shaderData->setSemGain(semGain);
}

void DiffuseSphericalEnvMapProperties::setSem(Qt3DRender::QAbstractTexture * sem)
{
    m_shaderData->setSem(sem);
}

void DiffuseSphericalEnvMapProperties::setSemInnerFilter(const QVector3D &semInnerFilter)
{
    m_shaderData->setSemInnerFilter(semInnerFilter);
}

void DiffuseSphericalEnvMapProperties::setSemOuterFilter(const QVector3D &semOuterFilter)
{
    m_shaderData->setSemOuterFilter(semOuterFilter);
}

void DiffuseSphericalEnvMapProperties::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    m_shaderData->setDiffuseInnerFilter(diffuseInnerFilter);
}

void DiffuseSphericalEnvMapProperties::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    m_shaderData->setDiffuseOuterFilter(diffuseOuterFilter);
}

void DiffuseSphericalEnvMapProperties::setDiffuseMap(Qt3DRender::QAbstractTexture * diffuseMap)
{
    m_shaderData->setDiffuseMap(diffuseMap);
}

void DiffuseSphericalEnvMapProperties::setDiffuseFactor(const QVector3D &diffuseFactor)
{
    m_shaderData->setDiffuseFactor(diffuseFactor);
}

void DiffuseSphericalEnvMapProperties::setUsesDiffuseMap(bool usesDiffuseMap)
{
    m_shaderData->setUsesDiffuseMap(usesDiffuseMap);
}


/*!
    \qmlproperty QVector3D DiffuseSphericalEnvMapProperties::factors
    Specifies a factor to be applied to the normal.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::factors
    Specifies a factor to be applied to the normal.
*/
QVector3D DiffuseSphericalEnvMapProperties::factors() const
{
    return m_shaderData->factors();
}

/*!
    \qmlproperty QVector2D DiffuseSphericalEnvMapProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property DiffuseSphericalEnvMapProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D DiffuseSphericalEnvMapProperties::disturbation() const
{
    return m_shaderData->disturbation();
}

/*!
    \qmlproperty float DiffuseSphericalEnvMapProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float DiffuseSphericalEnvMapProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float DiffuseSphericalEnvMapProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
float DiffuseSphericalEnvMapProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty float DiffuseSphericalEnvMapProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float DiffuseSphericalEnvMapProperties::semGain() const
{
    return m_shaderData->semGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * DiffuseSphericalEnvMapProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * DiffuseSphericalEnvMapProperties::sem() const
{
    return m_shaderData->sem();
}

/*!
    \qmlproperty QVector3D DiffuseSphericalEnvMapProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::semInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D DiffuseSphericalEnvMapProperties::semInnerFilter() const
{
    return m_shaderData->semInnerFilter();
}

/*!
    \qmlproperty QVector3D DiffuseSphericalEnvMapProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::semOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D DiffuseSphericalEnvMapProperties::semOuterFilter() const
{
    return m_shaderData->semOuterFilter();
}

/*!
    \qmlproperty QVector3D DiffuseSphericalEnvMapProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::diffuseInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D DiffuseSphericalEnvMapProperties::diffuseInnerFilter() const
{
    return m_shaderData->diffuseInnerFilter();
}

/*!
    \qmlproperty QVector3D DiffuseSphericalEnvMapProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::diffuseOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the diffuse map lookup.
*/
QVector3D DiffuseSphericalEnvMapProperties::diffuseOuterFilter() const
{
    return m_shaderData->diffuseOuterFilter();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * DiffuseSphericalEnvMapProperties::diffuseMap
    Specifies the diffuse map to use. It is expected to be in sRGB color space.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::diffuseMap
    Specifies the diffuse map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * DiffuseSphericalEnvMapProperties::diffuseMap() const
{
    return m_shaderData->diffuseMap();
}

/*!
    \qmlproperty QVector3D DiffuseSphericalEnvMapProperties::diffuseFactor
    Specifies the diffuse color to use in case no diffuse map was specified.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::diffuseFactor
    Specifies the diffuse color to use in case no diffuse map was specified.
*/
QVector3D DiffuseSphericalEnvMapProperties::diffuseFactor() const
{
    return m_shaderData->diffuseFactor();
}

/*!
    \qmlproperty bool DiffuseSphericalEnvMapProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
/*!
    \property DiffuseSphericalEnvMapProperties::usesDiffuseMap
    Specifies whether we use a diffuse map or not.
*/
bool DiffuseSphericalEnvMapProperties::usesDiffuseMap() const
{
    return m_shaderData->usesDiffuseMap();
}


} // namespace Kuesa

QT_END_NAMESPACE
