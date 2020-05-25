
/*
    iromatteaddproperties.cpp

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

#include "iromatteaddproperties.h"
#include "iromatteaddshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroMatteAddProperties
    \inheaderfile Kuesa/IroMatteAddProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteAddProperties holds the properties controlling the visual appearance of a IroMatteAddMaterial instance.
*/

/*!
    \qmltype IroMatteAddProperties
    \instantiates Kuesa::IroMatteAddProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteAddProperties holds the properties controlling the visual appearance of a IroMatteAddMaterial instance.
*/

IroMatteAddProperties::IroMatteAddProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroMatteAddShaderData(this))
{
    QObject::connect(m_shaderData, &IroMatteAddShaderData::postVertexColorChanged, this, &IroMatteAddProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroMatteAddShaderData::postGainChanged, this, &IroMatteAddProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroMatteAddShaderData::matteMapChanged, this, &IroMatteAddProperties::matteMapChanged);
    QObject::connect(m_shaderData, &IroMatteAddShaderData::usesMatteMapChanged, this, &IroMatteAddProperties::usesMatteMapChanged);
    QObject::connect(m_shaderData, &IroMatteAddShaderData::matteFilterChanged, this, &IroMatteAddProperties::matteFilterChanged);
    QObject::connect(m_shaderData, &IroMatteAddShaderData::matteGainChanged, this, &IroMatteAddProperties::matteGainChanged);
    QObject::connect(m_shaderData, &IroMatteAddShaderData::uvOffsetChanged, this, &IroMatteAddProperties::uvOffsetChanged);

}

IroMatteAddProperties::~IroMatteAddProperties() = default;

Qt3DRender::QShaderData *IroMatteAddProperties::shaderData() const
{
    return m_shaderData;
}

void IroMatteAddProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroMatteAddProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroMatteAddProperties::setMatteMap(Qt3DRender::QAbstractTexture * matteMap)
{
    m_shaderData->setMatteMap(matteMap);
}

void IroMatteAddProperties::setUsesMatteMap(bool usesMatteMap)
{
    m_shaderData->setUsesMatteMap(usesMatteMap);
}

void IroMatteAddProperties::setMatteFilter(const QVector3D &matteFilter)
{
    m_shaderData->setMatteFilter(matteFilter);
}

void IroMatteAddProperties::setMatteGain(float matteGain)
{
    m_shaderData->setMatteGain(matteGain);
}

void IroMatteAddProperties::setUvOffset(const QVector2D &uvOffset)
{
    m_shaderData->setUvOffset(uvOffset);
}


/*!
    \qmlproperty float IroMatteAddProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroMatteAddProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroMatteAddProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroMatteAddProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property IroMatteAddProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float IroMatteAddProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * IroMatteAddProperties::matteMap
    Specifies the matte map to use. It is expected to be in sRGB color space.
*/
/*!
    \property IroMatteAddProperties::matteMap
    Specifies the matte map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * IroMatteAddProperties::matteMap() const
{
    return m_shaderData->matteMap();
}

/*!
    \qmlproperty bool IroMatteAddProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
/*!
    \property IroMatteAddProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
bool IroMatteAddProperties::usesMatteMap() const
{
    return m_shaderData->usesMatteMap();
}

/*!
    \qmlproperty QVector3D IroMatteAddProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
/*!
    \property IroMatteAddProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
QVector3D IroMatteAddProperties::matteFilter() const
{
    return m_shaderData->matteFilter();
}

/*!
    \qmlproperty float IroMatteAddProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
/*!
    \property IroMatteAddProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
float IroMatteAddProperties::matteGain() const
{
    return m_shaderData->matteGain();
}

/*!
    \qmlproperty QVector2D IroMatteAddProperties::uvOffset
    Applies an offset to texture lookup.
*/
/*!
    \property IroMatteAddProperties::uvOffset
    Applies an offset to texture lookup.
*/
QVector2D IroMatteAddProperties::uvOffset() const
{
    return m_shaderData->uvOffset();
}


} // namespace Kuesa

QT_END_NAMESPACE
