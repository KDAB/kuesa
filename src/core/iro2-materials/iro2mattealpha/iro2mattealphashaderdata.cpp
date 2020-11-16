
/*
    iro2mattealphashaderdata.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Nicolas Guichard <nicolas.guichard@kdab.com>

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

#include "iro2mattealphashaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

Iro2MatteAlphaShaderData::Iro2MatteAlphaShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_usesMatteMap()
    , m_matteGain()
    , m_matteInnerFilter()
    , m_matteOuterFilter()
    , m_uvOffset()
    , m_innerAlpha()
    , m_outerAlpha()
    , m_alphaGain()
    , m_usesNormalMap()
    , m_normalMapGain()
    , m_normalScaling()
    , m_normalDisturb()
    , m_postVertexColor()
    , m_postHemiFilter()
    , m_postGain()
    , m_gltfYUp()
{}

Iro2MatteAlphaShaderData::~Iro2MatteAlphaShaderData() = default;

bool Iro2MatteAlphaShaderData::usesMatteMap() const
{
    return m_usesMatteMap;
}

float Iro2MatteAlphaShaderData::matteGain() const
{
    return m_matteGain;
}

QVector3D Iro2MatteAlphaShaderData::matteInnerFilter() const
{
    return m_matteInnerFilter;
}

QVector3D Iro2MatteAlphaShaderData::matteOuterFilter() const
{
    return m_matteOuterFilter;
}

QVector2D Iro2MatteAlphaShaderData::uvOffset() const
{
    return m_uvOffset;
}

float Iro2MatteAlphaShaderData::innerAlpha() const
{
    return m_innerAlpha;
}

float Iro2MatteAlphaShaderData::outerAlpha() const
{
    return m_outerAlpha;
}

float Iro2MatteAlphaShaderData::alphaGain() const
{
    return m_alphaGain;
}

bool Iro2MatteAlphaShaderData::usesNormalMap() const
{
    return m_usesNormalMap;
}

float Iro2MatteAlphaShaderData::normalMapGain() const
{
    return m_normalMapGain;
}

QVector3D Iro2MatteAlphaShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D Iro2MatteAlphaShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float Iro2MatteAlphaShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

QVector3D Iro2MatteAlphaShaderData::postHemiFilter() const
{
    return m_postHemiFilter;
}

float Iro2MatteAlphaShaderData::postGain() const
{
    return m_postGain;
}

bool Iro2MatteAlphaShaderData::gltfYUp() const
{
    return m_gltfYUp;
}


void Iro2MatteAlphaShaderData::setUsesMatteMap(bool usesMatteMap)
{
    if (m_usesMatteMap == usesMatteMap)
        return;
    m_usesMatteMap = usesMatteMap;
    emit usesMatteMapChanged(usesMatteMap);
}

void Iro2MatteAlphaShaderData::setMatteGain(float matteGain)
{
    if (m_matteGain == matteGain)
        return;
    m_matteGain = matteGain;
    emit matteGainChanged(matteGain);
}

void Iro2MatteAlphaShaderData::setMatteInnerFilter(const QVector3D &matteInnerFilter)
{
    if (m_matteInnerFilter == matteInnerFilter)
        return;
    m_matteInnerFilter = matteInnerFilter;
    emit matteInnerFilterChanged(matteInnerFilter);
}

void Iro2MatteAlphaShaderData::setMatteOuterFilter(const QVector3D &matteOuterFilter)
{
    if (m_matteOuterFilter == matteOuterFilter)
        return;
    m_matteOuterFilter = matteOuterFilter;
    emit matteOuterFilterChanged(matteOuterFilter);
}

void Iro2MatteAlphaShaderData::setUvOffset(const QVector2D &uvOffset)
{
    if (m_uvOffset == uvOffset)
        return;
    m_uvOffset = uvOffset;
    emit uvOffsetChanged(uvOffset);
}

void Iro2MatteAlphaShaderData::setInnerAlpha(float innerAlpha)
{
    if (m_innerAlpha == innerAlpha)
        return;
    m_innerAlpha = innerAlpha;
    emit innerAlphaChanged(innerAlpha);
}

void Iro2MatteAlphaShaderData::setOuterAlpha(float outerAlpha)
{
    if (m_outerAlpha == outerAlpha)
        return;
    m_outerAlpha = outerAlpha;
    emit outerAlphaChanged(outerAlpha);
}

void Iro2MatteAlphaShaderData::setAlphaGain(float alphaGain)
{
    if (m_alphaGain == alphaGain)
        return;
    m_alphaGain = alphaGain;
    emit alphaGainChanged(alphaGain);
}

void Iro2MatteAlphaShaderData::setUsesNormalMap(bool usesNormalMap)
{
    if (m_usesNormalMap == usesNormalMap)
        return;
    m_usesNormalMap = usesNormalMap;
    emit usesNormalMapChanged(usesNormalMap);
}

void Iro2MatteAlphaShaderData::setNormalMapGain(float normalMapGain)
{
    if (m_normalMapGain == normalMapGain)
        return;
    m_normalMapGain = normalMapGain;
    emit normalMapGainChanged(normalMapGain);
}

void Iro2MatteAlphaShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void Iro2MatteAlphaShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void Iro2MatteAlphaShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void Iro2MatteAlphaShaderData::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    if (m_postHemiFilter == postHemiFilter)
        return;
    m_postHemiFilter = postHemiFilter;
    emit postHemiFilterChanged(postHemiFilter);
}

void Iro2MatteAlphaShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void Iro2MatteAlphaShaderData::setGltfYUp(bool gltfYUp)
{
    if (m_gltfYUp == gltfYUp)
        return;
    m_gltfYUp = gltfYUp;
    emit gltfYUpChanged(gltfYUp);
}



} // namespace Kuesa

QT_END_NAMESPACE
