/*
    iro2alphasemshaderdata.cpp

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

#include "iro2alphasemshaderdata_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {
Iro2AlphaSemShaderData::Iro2AlphaSemShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_usesReflectionMap()
    , m_reflectionGain()
    , m_reflectionInnerFilter()
    , m_reflectionOuterFilter()
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
{
}

Iro2AlphaSemShaderData::~Iro2AlphaSemShaderData() = default;

bool Iro2AlphaSemShaderData::usesReflectionMap() const
{
    return m_usesReflectionMap;
}

float Iro2AlphaSemShaderData::reflectionGain() const
{
    return m_reflectionGain;
}

QVector3D Iro2AlphaSemShaderData::reflectionInnerFilter() const
{
    return m_reflectionInnerFilter;
}

QVector3D Iro2AlphaSemShaderData::reflectionOuterFilter() const
{
    return m_reflectionOuterFilter;
}

float Iro2AlphaSemShaderData::innerAlpha() const
{
    return m_innerAlpha;
}

float Iro2AlphaSemShaderData::outerAlpha() const
{
    return m_outerAlpha;
}

float Iro2AlphaSemShaderData::alphaGain() const
{
    return m_alphaGain;
}

bool Iro2AlphaSemShaderData::usesNormalMap() const
{
    return m_usesNormalMap;
}

float Iro2AlphaSemShaderData::normalMapGain() const
{
    return m_normalMapGain;
}

QVector3D Iro2AlphaSemShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D Iro2AlphaSemShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float Iro2AlphaSemShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

QVector3D Iro2AlphaSemShaderData::postHemiFilter() const
{
    return m_postHemiFilter;
}

float Iro2AlphaSemShaderData::postGain() const
{
    return m_postGain;
}

bool Iro2AlphaSemShaderData::gltfYUp() const
{
    return m_gltfYUp;
}

void Iro2AlphaSemShaderData::setUsesReflectionMap(bool usesReflectionMap)
{
    if (m_usesReflectionMap == usesReflectionMap)
        return;
    m_usesReflectionMap = usesReflectionMap;
    emit usesReflectionMapChanged(usesReflectionMap);
}

void Iro2AlphaSemShaderData::setReflectionGain(float reflectionGain)
{
    if (m_reflectionGain == reflectionGain)
        return;
    m_reflectionGain = reflectionGain;
    emit reflectionGainChanged(reflectionGain);
}

void Iro2AlphaSemShaderData::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    if (m_reflectionInnerFilter == reflectionInnerFilter)
        return;
    m_reflectionInnerFilter = reflectionInnerFilter;
    emit reflectionInnerFilterChanged(reflectionInnerFilter);
}

void Iro2AlphaSemShaderData::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    if (m_reflectionOuterFilter == reflectionOuterFilter)
        return;
    m_reflectionOuterFilter = reflectionOuterFilter;
    emit reflectionOuterFilterChanged(reflectionOuterFilter);
}

void Iro2AlphaSemShaderData::setInnerAlpha(float innerAlpha)
{
    if (m_innerAlpha == innerAlpha)
        return;
    m_innerAlpha = innerAlpha;
    emit innerAlphaChanged(innerAlpha);
}

void Iro2AlphaSemShaderData::setOuterAlpha(float outerAlpha)
{
    if (m_outerAlpha == outerAlpha)
        return;
    m_outerAlpha = outerAlpha;
    emit outerAlphaChanged(outerAlpha);
}

void Iro2AlphaSemShaderData::setAlphaGain(float alphaGain)
{
    if (m_alphaGain == alphaGain)
        return;
    m_alphaGain = alphaGain;
    emit alphaGainChanged(alphaGain);
}

void Iro2AlphaSemShaderData::setUsesNormalMap(bool usesNormalMap)
{
    if (m_usesNormalMap == usesNormalMap)
        return;
    m_usesNormalMap = usesNormalMap;
    emit usesNormalMapChanged(usesNormalMap);
}

void Iro2AlphaSemShaderData::setNormalMapGain(float normalMapGain)
{
    if (m_normalMapGain == normalMapGain)
        return;
    m_normalMapGain = normalMapGain;
    emit normalMapGainChanged(normalMapGain);
}

void Iro2AlphaSemShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void Iro2AlphaSemShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void Iro2AlphaSemShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void Iro2AlphaSemShaderData::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    if (m_postHemiFilter == postHemiFilter)
        return;
    m_postHemiFilter = postHemiFilter;
    emit postHemiFilterChanged(postHemiFilter);
}

void Iro2AlphaSemShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void Iro2AlphaSemShaderData::setGltfYUp(bool gltfYUp)
{
    if (m_gltfYUp == gltfYUp)
        return;
    m_gltfYUp = gltfYUp;
    emit gltfYUpChanged(gltfYUp);
}

} // namespace Kuesa

QT_END_NAMESPACE
