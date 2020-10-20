
/*
    iromatteskyboxproperties.cpp

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

#include "iromatteskyboxproperties.h"
#include "iromatteskyboxshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroMatteSkyboxProperties
    \inheaderfile Kuesa/IroMatteSkyboxProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteSkyboxProperties holds the properties controlling the visual appearance of a IroMatteSkyboxMaterial instance.
*/

/*!
    \qmltype IroMatteSkyboxProperties
    \instantiates Kuesa::IroMatteSkyboxProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteSkyboxProperties holds the properties controlling the visual appearance of a IroMatteSkyboxMaterial instance.
*/

IroMatteSkyboxProperties::IroMatteSkyboxProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroMatteSkyboxShaderData(this))
    , m_matteMap(nullptr)
{
    QObject::connect(m_shaderData, &IroMatteSkyboxShaderData::postVertexColorChanged, this, &IroMatteSkyboxProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroMatteSkyboxShaderData::postGainChanged, this, &IroMatteSkyboxProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroMatteSkyboxShaderData::usesMatteMapChanged, this, &IroMatteSkyboxProperties::usesMatteMapChanged);
    QObject::connect(m_shaderData, &IroMatteSkyboxShaderData::matteFilterChanged, this, &IroMatteSkyboxProperties::matteFilterChanged);
    QObject::connect(m_shaderData, &IroMatteSkyboxShaderData::matteGainChanged, this, &IroMatteSkyboxProperties::matteGainChanged);
    QObject::connect(m_shaderData, &IroMatteSkyboxShaderData::uvOffsetChanged, this, &IroMatteSkyboxProperties::uvOffsetChanged);

    setMatteMap(new Empty2DTexture());
}

IroMatteSkyboxProperties::~IroMatteSkyboxProperties() = default;

Qt3DRender::QShaderData *IroMatteSkyboxProperties::shaderData() const
{
    return m_shaderData;
}

void IroMatteSkyboxProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroMatteSkyboxProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroMatteSkyboxProperties::setUsesMatteMap(bool usesMatteMap)
{
    m_shaderData->setUsesMatteMap(usesMatteMap);
}

void IroMatteSkyboxProperties::setMatteFilter(const QVector3D &matteFilter)
{
    m_shaderData->setMatteFilter(matteFilter);
}

void IroMatteSkyboxProperties::setMatteGain(float matteGain)
{
    m_shaderData->setMatteGain(matteGain);
}

void IroMatteSkyboxProperties::setUvOffset(const QVector2D &uvOffset)
{
    m_shaderData->setUvOffset(uvOffset);
}

void IroMatteSkyboxProperties::setMatteMap(Qt3DRender::QAbstractTexture * matteMap)
{
    if (m_matteMap == matteMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_matteMap != nullptr)
        d->unregisterDestructionHelper(m_matteMap);
    m_matteMap = matteMap;
    if (m_matteMap != nullptr) {
        if (m_matteMap->parent() == nullptr)
            m_matteMap->setParent(this);
        d->registerDestructionHelper(m_matteMap, &IroMatteSkyboxProperties::setMatteMap, m_matteMap);
    }
    emit matteMapChanged(m_matteMap);
}


/*!
    \qmlproperty float IroMatteSkyboxProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroMatteSkyboxProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroMatteSkyboxProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroMatteSkyboxProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property IroMatteSkyboxProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float IroMatteSkyboxProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool IroMatteSkyboxProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
/*!
    \property IroMatteSkyboxProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
bool IroMatteSkyboxProperties::usesMatteMap() const
{
    return m_shaderData->usesMatteMap();
}

/*!
    \qmlproperty QVector3D IroMatteSkyboxProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
/*!
    \property IroMatteSkyboxProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
QVector3D IroMatteSkyboxProperties::matteFilter() const
{
    return m_shaderData->matteFilter();
}

/*!
    \qmlproperty float IroMatteSkyboxProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
/*!
    \property IroMatteSkyboxProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
float IroMatteSkyboxProperties::matteGain() const
{
    return m_shaderData->matteGain();
}

/*!
    \qmlproperty QVector2D IroMatteSkyboxProperties::uvOffset
    Applies an offset to texture lookup.
*/
/*!
    \property IroMatteSkyboxProperties::uvOffset
    Applies an offset to texture lookup.
*/
QVector2D IroMatteSkyboxProperties::uvOffset() const
{
    return m_shaderData->uvOffset();
}

Qt3DRender::QAbstractTexture * IroMatteSkyboxProperties::matteMap() const
{
    return m_matteMap;
}


} // namespace Kuesa

QT_END_NAMESPACE
