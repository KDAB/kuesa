
/*
    sphericalenvmapproperties.cpp

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

#include "sphericalenvmapproperties.h"
#include "sphericalenvmapshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::SphericalEnvMapProperties
    \inheaderfile Kuesa/SphericalEnvMapProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::SphericalEnvMapProperties holds the properties controlling the visual appearance of a SphericalEnvMapMaterial instance
*/

/*!
    \qmltype SphericalEnvMapProperties
    \instantiates Kuesa::SphericalEnvMapProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::SphericalEnvMapProperties holds the properties controlling the visual appearance of a SphericalEnvMapMaterial instance
*/

SphericalEnvMapProperties::SphericalEnvMapProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new SphericalEnvMapShaderData(this))
{
    QObject::connect(m_shaderData, &SphericalEnvMapShaderData::factorsChanged, this, &SphericalEnvMapProperties::factorsChanged);
    QObject::connect(m_shaderData, &SphericalEnvMapShaderData::disturbationChanged, this, &SphericalEnvMapProperties::disturbationChanged);
    QObject::connect(m_shaderData, &SphericalEnvMapShaderData::postVertexColorChanged, this, &SphericalEnvMapProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &SphericalEnvMapShaderData::postGainChanged, this, &SphericalEnvMapProperties::postGainChanged);
    QObject::connect(m_shaderData, &SphericalEnvMapShaderData::semGainChanged, this, &SphericalEnvMapProperties::semGainChanged);
    QObject::connect(m_shaderData, &SphericalEnvMapShaderData::semChanged, this, &SphericalEnvMapProperties::semChanged);

}

SphericalEnvMapProperties::~SphericalEnvMapProperties() = default;

Qt3DRender::QShaderData *SphericalEnvMapProperties::shaderData() const
{
    return m_shaderData;
}

void SphericalEnvMapProperties::setFactors(const QVector3D &factors)
{
    m_shaderData->setFactors(factors);
}

void SphericalEnvMapProperties::setDisturbation(const QVector2D &disturbation)
{
    m_shaderData->setDisturbation(disturbation);
}

void SphericalEnvMapProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void SphericalEnvMapProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void SphericalEnvMapProperties::setSemGain(float semGain)
{
    m_shaderData->setSemGain(semGain);
}

void SphericalEnvMapProperties::setSem(Qt3DRender::QAbstractTexture * sem)
{
    m_shaderData->setSem(sem);
}


/*!
    \qmlproperty QVector3D SphericalEnvMapProperties::factors
    Specifies a factor to be applied to the normal.
*/
/*!
    \property SphericalEnvMapProperties::factors
    Specifies a factor to be applied to the normal.
*/
QVector3D SphericalEnvMapProperties::factors() const
{
    return m_shaderData->factors();
}

/*!
    \qmlproperty QVector2D SphericalEnvMapProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property SphericalEnvMapProperties::disturbation
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D SphericalEnvMapProperties::disturbation() const
{
    return m_shaderData->disturbation();
}

/*!
    \qmlproperty float SphericalEnvMapProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property SphericalEnvMapProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float SphericalEnvMapProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float SphericalEnvMapProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
/*!
    \property SphericalEnvMapProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
float SphericalEnvMapProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty float SphericalEnvMapProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property SphericalEnvMapProperties::semGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float SphericalEnvMapProperties::semGain() const
{
    return m_shaderData->semGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * SphericalEnvMapProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
/*!
    \property SphericalEnvMapProperties::sem
    Specifies the spherical environment map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * SphericalEnvMapProperties::sem() const
{
    return m_shaderData->sem();
}


} // namespace Kuesa

QT_END_NAMESPACE
