
/*
    iromatteopaqueproperties.cpp

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

#include "iromatteopaqueproperties.h"
#include "iromatteopaqueshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroMatteOpaqueProperties
    \inheaderfile Kuesa/IroMatteOpaqueProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteOpaqueProperties holds the properties controlling the visual appearance of a IroMatteOpaqueMaterial instance.
*/

/*!
    \qmltype IroMatteOpaqueProperties
    \instantiates Kuesa::IroMatteOpaqueProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteOpaqueProperties holds the properties controlling the visual appearance of a IroMatteOpaqueMaterial instance.
*/

IroMatteOpaqueProperties::IroMatteOpaqueProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroMatteOpaqueShaderData(this))
    , m_matteMap(nullptr)
{
    QObject::connect(m_shaderData, &IroMatteOpaqueShaderData::postVertexColorChanged, this, &IroMatteOpaqueProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroMatteOpaqueShaderData::postGainChanged, this, &IroMatteOpaqueProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroMatteOpaqueShaderData::usesMatteMapChanged, this, &IroMatteOpaqueProperties::usesMatteMapChanged);
    QObject::connect(m_shaderData, &IroMatteOpaqueShaderData::matteFilterChanged, this, &IroMatteOpaqueProperties::matteFilterChanged);
    QObject::connect(m_shaderData, &IroMatteOpaqueShaderData::matteGainChanged, this, &IroMatteOpaqueProperties::matteGainChanged);
    QObject::connect(m_shaderData, &IroMatteOpaqueShaderData::uvOffsetChanged, this, &IroMatteOpaqueProperties::uvOffsetChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setMatteMap(new Empty2DTexture());
#endif
}

IroMatteOpaqueProperties::~IroMatteOpaqueProperties() = default;

Qt3DRender::QShaderData *IroMatteOpaqueProperties::shaderData() const
{
    return m_shaderData;
}

void IroMatteOpaqueProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroMatteOpaqueProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroMatteOpaqueProperties::setUsesMatteMap(bool usesMatteMap)
{
    m_shaderData->setUsesMatteMap(usesMatteMap);
}

void IroMatteOpaqueProperties::setMatteFilter(const QVector3D &matteFilter)
{
    m_shaderData->setMatteFilter(matteFilter);
}

void IroMatteOpaqueProperties::setMatteGain(float matteGain)
{
    m_shaderData->setMatteGain(matteGain);
}

void IroMatteOpaqueProperties::setUvOffset(const QVector2D &uvOffset)
{
    m_shaderData->setUvOffset(uvOffset);
}

void IroMatteOpaqueProperties::setMatteMap(Qt3DRender::QAbstractTexture * matteMap)
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
        d->registerDestructionHelper(m_matteMap, &IroMatteOpaqueProperties::setMatteMap, m_matteMap);
    }
    emit matteMapChanged(m_matteMap);
}


/*!
    \qmlproperty float IroMatteOpaqueProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroMatteOpaqueProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroMatteOpaqueProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroMatteOpaqueProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property IroMatteOpaqueProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float IroMatteOpaqueProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool IroMatteOpaqueProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
/*!
    \property IroMatteOpaqueProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
bool IroMatteOpaqueProperties::usesMatteMap() const
{
    return m_shaderData->usesMatteMap();
}

/*!
    \qmlproperty QVector3D IroMatteOpaqueProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
/*!
    \property IroMatteOpaqueProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
QVector3D IroMatteOpaqueProperties::matteFilter() const
{
    return m_shaderData->matteFilter();
}

/*!
    \qmlproperty float IroMatteOpaqueProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
/*!
    \property IroMatteOpaqueProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
float IroMatteOpaqueProperties::matteGain() const
{
    return m_shaderData->matteGain();
}

/*!
    \qmlproperty QVector2D IroMatteOpaqueProperties::uvOffset
    Applies an offset to texture lookup.
*/
/*!
    \property IroMatteOpaqueProperties::uvOffset
    Applies an offset to texture lookup.
*/
QVector2D IroMatteOpaqueProperties::uvOffset() const
{
    return m_shaderData->uvOffset();
}

Qt3DRender::QAbstractTexture * IroMatteOpaqueProperties::matteMap() const
{
    return m_matteMap;
}


} // namespace Kuesa

QT_END_NAMESPACE
