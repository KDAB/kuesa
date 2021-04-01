/*
    iromattebackgroundshaderdata.cpp

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

#include "iromattebackgroundshaderdata_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {
IroMatteBackgroundShaderData::IroMatteBackgroundShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_postVertexColor()
    , m_postGain()
    , m_usesMatteMap()
    , m_matteFilter()
    , m_matteGain()
    , m_uvOffset()
{
}

IroMatteBackgroundShaderData::~IroMatteBackgroundShaderData() = default;

float IroMatteBackgroundShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float IroMatteBackgroundShaderData::postGain() const
{
    return m_postGain;
}

bool IroMatteBackgroundShaderData::usesMatteMap() const
{
    return m_usesMatteMap;
}

QVector3D IroMatteBackgroundShaderData::matteFilter() const
{
    return m_matteFilter;
}

float IroMatteBackgroundShaderData::matteGain() const
{
    return m_matteGain;
}

QVector2D IroMatteBackgroundShaderData::uvOffset() const
{
    return m_uvOffset;
}

void IroMatteBackgroundShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void IroMatteBackgroundShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void IroMatteBackgroundShaderData::setUsesMatteMap(bool usesMatteMap)
{
    if (m_usesMatteMap == usesMatteMap)
        return;
    m_usesMatteMap = usesMatteMap;
    emit usesMatteMapChanged(usesMatteMap);
}

void IroMatteBackgroundShaderData::setMatteFilter(const QVector3D &matteFilter)
{
    if (m_matteFilter == matteFilter)
        return;
    m_matteFilter = matteFilter;
    emit matteFilterChanged(matteFilter);
}

void IroMatteBackgroundShaderData::setMatteGain(float matteGain)
{
    if (m_matteGain == matteGain)
        return;
    m_matteGain = matteGain;
    emit matteGainChanged(matteGain);
}

void IroMatteBackgroundShaderData::setUvOffset(const QVector2D &uvOffset)
{
    if (m_uvOffset == uvOffset)
        return;
    m_uvOffset = uvOffset;
    emit uvOffsetChanged(uvOffset);
}

} // namespace Kuesa

QT_END_NAMESPACE
