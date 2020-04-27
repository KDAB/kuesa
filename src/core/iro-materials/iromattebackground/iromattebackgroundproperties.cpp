
/*
    iromattebackgroundproperties.cpp

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

#include "iromattebackgroundproperties.h"
#include "iromattebackgroundshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroMatteBackgroundProperties
    \inheaderfile Kuesa/IroMatteBackgroundProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteBackgroundProperties holds the properties controlling the visual appearance of a IroMatteBackgroundMaterial instance.
*/

/*!
    \qmltype IroMatteBackgroundProperties
    \instantiates Kuesa::IroMatteBackgroundProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteBackgroundProperties holds the properties controlling the visual appearance of a IroMatteBackgroundMaterial instance.
*/

IroMatteBackgroundProperties::IroMatteBackgroundProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroMatteBackgroundShaderData(this))
{
    QObject::connect(m_shaderData, &IroMatteBackgroundShaderData::postVertexColorChanged, this, &IroMatteBackgroundProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroMatteBackgroundShaderData::postGainChanged, this, &IroMatteBackgroundProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroMatteBackgroundShaderData::matteMapChanged, this, &IroMatteBackgroundProperties::matteMapChanged);
    QObject::connect(m_shaderData, &IroMatteBackgroundShaderData::usesMatteMapChanged, this, &IroMatteBackgroundProperties::usesMatteMapChanged);
    QObject::connect(m_shaderData, &IroMatteBackgroundShaderData::matteFilterChanged, this, &IroMatteBackgroundProperties::matteFilterChanged);
    QObject::connect(m_shaderData, &IroMatteBackgroundShaderData::matteGainChanged, this, &IroMatteBackgroundProperties::matteGainChanged);
    QObject::connect(m_shaderData, &IroMatteBackgroundShaderData::uvOffsetChanged, this, &IroMatteBackgroundProperties::uvOffsetChanged);

}

IroMatteBackgroundProperties::~IroMatteBackgroundProperties() = default;

Qt3DRender::QShaderData *IroMatteBackgroundProperties::shaderData() const
{
    return m_shaderData;
}

void IroMatteBackgroundProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroMatteBackgroundProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroMatteBackgroundProperties::setMatteMap(Qt3DRender::QAbstractTexture * matteMap)
{
    m_shaderData->setMatteMap(matteMap);
}

void IroMatteBackgroundProperties::setUsesMatteMap(bool usesMatteMap)
{
    m_shaderData->setUsesMatteMap(usesMatteMap);
}

void IroMatteBackgroundProperties::setMatteFilter(const QVector3D &matteFilter)
{
    m_shaderData->setMatteFilter(matteFilter);
}

void IroMatteBackgroundProperties::setMatteGain(float matteGain)
{
    m_shaderData->setMatteGain(matteGain);
}

void IroMatteBackgroundProperties::setUvOffset(const QVector2D &uvOffset)
{
    m_shaderData->setUvOffset(uvOffset);
}


/*!
    \qmlproperty float IroMatteBackgroundProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroMatteBackgroundProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroMatteBackgroundProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroMatteBackgroundProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
/*!
    \property IroMatteBackgroundProperties::postGain
    Specifies the factor to be applied to the vertex color.
*/
float IroMatteBackgroundProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * IroMatteBackgroundProperties::matteMap
    Specifies the matte map to use. It is expected to be in sRGB color space.
*/
/*!
    \property IroMatteBackgroundProperties::matteMap
    Specifies the matte map to use. It is expected to be in sRGB color space.
*/
Qt3DRender::QAbstractTexture * IroMatteBackgroundProperties::matteMap() const
{
    return m_shaderData->matteMap();
}

/*!
    \qmlproperty bool IroMatteBackgroundProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
/*!
    \property IroMatteBackgroundProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
bool IroMatteBackgroundProperties::usesMatteMap() const
{
    return m_shaderData->usesMatteMap();
}

/*!
    \qmlproperty QVector3D IroMatteBackgroundProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
/*!
    \property IroMatteBackgroundProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
QVector3D IroMatteBackgroundProperties::matteFilter() const
{
    return m_shaderData->matteFilter();
}

/*!
    \qmlproperty float IroMatteBackgroundProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
/*!
    \property IroMatteBackgroundProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
float IroMatteBackgroundProperties::matteGain() const
{
    return m_shaderData->matteGain();
}

/*!
    \qmlproperty QVector2D IroMatteBackgroundProperties::uvOffset
    Applies an offset to texture lookup.
*/
/*!
    \property IroMatteBackgroundProperties::uvOffset
    Applies an offset to texture lookup.
*/
QVector2D IroMatteBackgroundProperties::uvOffset() const
{
    return m_shaderData->uvOffset();
}


} // namespace Kuesa

QT_END_NAMESPACE
