/*
    iromattemultproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    This file was auto-generated

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

#include "iromattemultproperties.h"
#include "iromattemultshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::IroMatteMultProperties
    \inheaderfile Kuesa/IroMatteMultProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteMultProperties holds the properties controlling the visual appearance of a IroMatteMultMaterial instance.
*/

/*!
    \qmltype IroMatteMultProperties
    \instantiates Kuesa::IroMatteMultProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteMultProperties holds the properties controlling the visual appearance of a IroMatteMultMaterial instance.
*/

IroMatteMultProperties::IroMatteMultProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroMatteMultShaderData(this))
    , m_matteMap(nullptr)
{
    QObject::connect(m_shaderData, &IroMatteMultShaderData::postVertexColorChanged, this, &IroMatteMultProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroMatteMultShaderData::postGainChanged, this, &IroMatteMultProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroMatteMultShaderData::usesMatteMapChanged, this, &IroMatteMultProperties::usesMatteMapChanged);
    QObject::connect(m_shaderData, &IroMatteMultShaderData::matteFilterChanged, this, &IroMatteMultProperties::matteFilterChanged);
    QObject::connect(m_shaderData, &IroMatteMultShaderData::matteGainChanged, this, &IroMatteMultProperties::matteGainChanged);
    QObject::connect(m_shaderData, &IroMatteMultShaderData::uvOffsetChanged, this, &IroMatteMultProperties::uvOffsetChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setMatteMap(new Empty2DTexture());
#endif
}

IroMatteMultProperties::~IroMatteMultProperties() = default;

Qt3DRender::QShaderData *IroMatteMultProperties::shaderData() const
{
    return m_shaderData;
}

void IroMatteMultProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroMatteMultProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroMatteMultProperties::setUsesMatteMap(bool usesMatteMap)
{
    m_shaderData->setUsesMatteMap(usesMatteMap);
}

void IroMatteMultProperties::setMatteFilter(const QVector3D &matteFilter)
{
    m_shaderData->setMatteFilter(matteFilter);
}

void IroMatteMultProperties::setMatteGain(float matteGain)
{
    m_shaderData->setMatteGain(matteGain);
}

void IroMatteMultProperties::setUvOffset(const QVector2D &uvOffset)
{
    m_shaderData->setUvOffset(uvOffset);
}

void IroMatteMultProperties::setMatteMap(Qt3DRender::QAbstractTexture *matteMap)
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
        d->registerDestructionHelper(m_matteMap, &IroMatteMultProperties::setMatteMap, m_matteMap);
    }
    emit matteMapChanged(m_matteMap);
}

/*!
    \qmlproperty float IroMatteMultProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroMatteMultProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroMatteMultProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroMatteMultProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property IroMatteMultProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float IroMatteMultProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool IroMatteMultProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
/*!
    \property IroMatteMultProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
bool IroMatteMultProperties::usesMatteMap() const
{
    return m_shaderData->usesMatteMap();
}

/*!
    \qmlproperty QVector3D IroMatteMultProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
/*!
    \property IroMatteMultProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
QVector3D IroMatteMultProperties::matteFilter() const
{
    return m_shaderData->matteFilter();
}

/*!
    \qmlproperty float IroMatteMultProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
/*!
    \property IroMatteMultProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
float IroMatteMultProperties::matteGain() const
{
    return m_shaderData->matteGain();
}

/*!
    \qmlproperty QVector2D IroMatteMultProperties::uvOffset
    Applies an offset to texture lookup.
*/
/*!
    \property IroMatteMultProperties::uvOffset
    Applies an offset to texture lookup.
*/
QVector2D IroMatteMultProperties::uvOffset() const
{
    return m_shaderData->uvOffset();
}

Qt3DRender::QAbstractTexture *IroMatteMultProperties::matteMap() const
{
    return m_matteMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
