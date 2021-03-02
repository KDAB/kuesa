
/*
    iromattealphaproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "iromattealphaproperties.h"
#include "iromattealphashaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::IroMatteAlphaProperties
    \inheaderfile Kuesa/IroMatteAlphaProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteAlphaProperties holds the properties controlling the visual appearance of a IroMatteAlphaMaterial instance.
*/

/*!
    \qmltype IroMatteAlphaProperties
    \instantiates Kuesa::IroMatteAlphaProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteAlphaProperties holds the properties controlling the visual appearance of a IroMatteAlphaMaterial instance.
*/

IroMatteAlphaProperties::IroMatteAlphaProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new IroMatteAlphaShaderData(this))
    , m_matteMap(nullptr)
{
    QObject::connect(m_shaderData, &IroMatteAlphaShaderData::postVertexColorChanged, this, &IroMatteAlphaProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &IroMatteAlphaShaderData::postGainChanged, this, &IroMatteAlphaProperties::postGainChanged);
    QObject::connect(m_shaderData, &IroMatteAlphaShaderData::usesMatteMapChanged, this, &IroMatteAlphaProperties::usesMatteMapChanged);
    QObject::connect(m_shaderData, &IroMatteAlphaShaderData::matteFilterChanged, this, &IroMatteAlphaProperties::matteFilterChanged);
    QObject::connect(m_shaderData, &IroMatteAlphaShaderData::matteGainChanged, this, &IroMatteAlphaProperties::matteGainChanged);
    QObject::connect(m_shaderData, &IroMatteAlphaShaderData::matteAlphaGainChanged, this, &IroMatteAlphaProperties::matteAlphaGainChanged);
    QObject::connect(m_shaderData, &IroMatteAlphaShaderData::uvOffsetChanged, this, &IroMatteAlphaProperties::uvOffsetChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setMatteMap(new Empty2DTexture());
#endif
}

IroMatteAlphaProperties::~IroMatteAlphaProperties() = default;

Qt3DRender::QShaderData *IroMatteAlphaProperties::shaderData() const
{
    return m_shaderData;
}

void IroMatteAlphaProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void IroMatteAlphaProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void IroMatteAlphaProperties::setUsesMatteMap(bool usesMatteMap)
{
    m_shaderData->setUsesMatteMap(usesMatteMap);
}

void IroMatteAlphaProperties::setMatteFilter(const QVector3D &matteFilter)
{
    m_shaderData->setMatteFilter(matteFilter);
}

void IroMatteAlphaProperties::setMatteGain(float matteGain)
{
    m_shaderData->setMatteGain(matteGain);
}

void IroMatteAlphaProperties::setMatteAlphaGain(float matteAlphaGain)
{
    m_shaderData->setMatteAlphaGain(matteAlphaGain);
}

void IroMatteAlphaProperties::setUvOffset(const QVector2D &uvOffset)
{
    m_shaderData->setUvOffset(uvOffset);
}

void IroMatteAlphaProperties::setMatteMap(Qt3DRender::QAbstractTexture *matteMap)
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
        d->registerDestructionHelper(m_matteMap, &IroMatteAlphaProperties::setMatteMap, m_matteMap);
    }
    emit matteMapChanged(m_matteMap);
}

/*!
    \qmlproperty float IroMatteAlphaProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property IroMatteAlphaProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float IroMatteAlphaProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty float IroMatteAlphaProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property IroMatteAlphaProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float IroMatteAlphaProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool IroMatteAlphaProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
/*!
    \property IroMatteAlphaProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
bool IroMatteAlphaProperties::usesMatteMap() const
{
    return m_shaderData->usesMatteMap();
}

/*!
    \qmlproperty QVector3D IroMatteAlphaProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
/*!
    \property IroMatteAlphaProperties::matteFilter
    Specifies the color filter to be applied on the matte map lookup.
*/
QVector3D IroMatteAlphaProperties::matteFilter() const
{
    return m_shaderData->matteFilter();
}

/*!
    \qmlproperty float IroMatteAlphaProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
/*!
    \property IroMatteAlphaProperties::matteGain
    Specifies the gain applied to the rgb components of the matte map lookup.
*/
float IroMatteAlphaProperties::matteGain() const
{
    return m_shaderData->matteGain();
}

/*!
    \qmlproperty float IroMatteAlphaProperties::matteAlphaGain
    Specifies the gain applied to the alpha of the matte map lookup.
*/
/*!
    \property IroMatteAlphaProperties::matteAlphaGain
    Specifies the gain applied to the alpha of the matte map lookup.
*/
float IroMatteAlphaProperties::matteAlphaGain() const
{
    return m_shaderData->matteAlphaGain();
}

/*!
    \qmlproperty QVector2D IroMatteAlphaProperties::uvOffset
    Applies an offset to texture lookup.
*/
/*!
    \property IroMatteAlphaProperties::uvOffset
    Applies an offset to texture lookup.
*/
QVector2D IroMatteAlphaProperties::uvOffset() const
{
    return m_shaderData->uvOffset();
}

Qt3DRender::QAbstractTexture *IroMatteAlphaProperties::matteMap() const
{
    return m_matteMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
