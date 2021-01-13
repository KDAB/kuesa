
/*
    iro2planarreflectionsemshaderdata.cpp

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

#include "iro2planarreflectionsemshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

Iro2PlanarReflectionSemShaderData::Iro2PlanarReflectionSemShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
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
    , m_postGain()
    , m_gltfYUp()
{}

Iro2PlanarReflectionSemShaderData::~Iro2PlanarReflectionSemShaderData() = default;

float Iro2PlanarReflectionSemShaderData::reflectionGain() const
{
    return m_reflectionGain;
}

QVector3D Iro2PlanarReflectionSemShaderData::reflectionInnerFilter() const
{
    return m_reflectionInnerFilter;
}

QVector3D Iro2PlanarReflectionSemShaderData::reflectionOuterFilter() const
{
    return m_reflectionOuterFilter;
}

float Iro2PlanarReflectionSemShaderData::innerAlpha() const
{
    return m_innerAlpha;
}

float Iro2PlanarReflectionSemShaderData::outerAlpha() const
{
    return m_outerAlpha;
}

float Iro2PlanarReflectionSemShaderData::alphaGain() const
{
    return m_alphaGain;
}

bool Iro2PlanarReflectionSemShaderData::usesNormalMap() const
{
    return m_usesNormalMap;
}

float Iro2PlanarReflectionSemShaderData::normalMapGain() const
{
    return m_normalMapGain;
}

QVector3D Iro2PlanarReflectionSemShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D Iro2PlanarReflectionSemShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float Iro2PlanarReflectionSemShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float Iro2PlanarReflectionSemShaderData::postGain() const
{
    return m_postGain;
}

bool Iro2PlanarReflectionSemShaderData::gltfYUp() const
{
    return m_gltfYUp;
}


void Iro2PlanarReflectionSemShaderData::setReflectionGain(float reflectionGain)
{
    if (m_reflectionGain == reflectionGain)
        return;
    m_reflectionGain = reflectionGain;
    emit reflectionGainChanged(reflectionGain);
}

void Iro2PlanarReflectionSemShaderData::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    if (m_reflectionInnerFilter == reflectionInnerFilter)
        return;
    m_reflectionInnerFilter = reflectionInnerFilter;
    emit reflectionInnerFilterChanged(reflectionInnerFilter);
}

void Iro2PlanarReflectionSemShaderData::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    if (m_reflectionOuterFilter == reflectionOuterFilter)
        return;
    m_reflectionOuterFilter = reflectionOuterFilter;
    emit reflectionOuterFilterChanged(reflectionOuterFilter);
}

void Iro2PlanarReflectionSemShaderData::setInnerAlpha(float innerAlpha)
{
    if (m_innerAlpha == innerAlpha)
        return;
    m_innerAlpha = innerAlpha;
    emit innerAlphaChanged(innerAlpha);
}

void Iro2PlanarReflectionSemShaderData::setOuterAlpha(float outerAlpha)
{
    if (m_outerAlpha == outerAlpha)
        return;
    m_outerAlpha = outerAlpha;
    emit outerAlphaChanged(outerAlpha);
}

void Iro2PlanarReflectionSemShaderData::setAlphaGain(float alphaGain)
{
    if (m_alphaGain == alphaGain)
        return;
    m_alphaGain = alphaGain;
    emit alphaGainChanged(alphaGain);
}

void Iro2PlanarReflectionSemShaderData::setUsesNormalMap(bool usesNormalMap)
{
    if (m_usesNormalMap == usesNormalMap)
        return;
    m_usesNormalMap = usesNormalMap;
    emit usesNormalMapChanged(usesNormalMap);
}

void Iro2PlanarReflectionSemShaderData::setNormalMapGain(float normalMapGain)
{
    if (m_normalMapGain == normalMapGain)
        return;
    m_normalMapGain = normalMapGain;
    emit normalMapGainChanged(normalMapGain);
}

void Iro2PlanarReflectionSemShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void Iro2PlanarReflectionSemShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void Iro2PlanarReflectionSemShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void Iro2PlanarReflectionSemShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void Iro2PlanarReflectionSemShaderData::setGltfYUp(bool gltfYUp)
{
    if (m_gltfYUp == gltfYUp)
        return;
    m_gltfYUp = gltfYUp;
    emit gltfYUpChanged(gltfYUp);
}



} // namespace Kuesa

QT_END_NAMESPACE
