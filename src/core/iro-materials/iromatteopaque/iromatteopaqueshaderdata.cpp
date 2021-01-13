
/*
    iromatteopaqueshaderdata.cpp

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

#include "iromatteopaqueshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

IroMatteOpaqueShaderData::IroMatteOpaqueShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_postVertexColor()
    , m_postGain()
    , m_usesMatteMap()
    , m_matteFilter()
    , m_matteGain()
    , m_uvOffset()
{}

IroMatteOpaqueShaderData::~IroMatteOpaqueShaderData() = default;

float IroMatteOpaqueShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float IroMatteOpaqueShaderData::postGain() const
{
    return m_postGain;
}

bool IroMatteOpaqueShaderData::usesMatteMap() const
{
    return m_usesMatteMap;
}

QVector3D IroMatteOpaqueShaderData::matteFilter() const
{
    return m_matteFilter;
}

float IroMatteOpaqueShaderData::matteGain() const
{
    return m_matteGain;
}

QVector2D IroMatteOpaqueShaderData::uvOffset() const
{
    return m_uvOffset;
}


void IroMatteOpaqueShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void IroMatteOpaqueShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void IroMatteOpaqueShaderData::setUsesMatteMap(bool usesMatteMap)
{
    if (m_usesMatteMap == usesMatteMap)
        return;
    m_usesMatteMap = usesMatteMap;
    emit usesMatteMapChanged(usesMatteMap);
}

void IroMatteOpaqueShaderData::setMatteFilter(const QVector3D &matteFilter)
{
    if (m_matteFilter == matteFilter)
        return;
    m_matteFilter = matteFilter;
    emit matteFilterChanged(matteFilter);
}

void IroMatteOpaqueShaderData::setMatteGain(float matteGain)
{
    if (m_matteGain == matteGain)
        return;
    m_matteGain = matteGain;
    emit matteGainChanged(matteGain);
}

void IroMatteOpaqueShaderData::setUvOffset(const QVector2D &uvOffset)
{
    if (m_uvOffset == uvOffset)
        return;
    m_uvOffset = uvOffset;
    emit uvOffsetChanged(uvOffset);
}



} // namespace Kuesa

QT_END_NAMESPACE
