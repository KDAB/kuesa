
/*
    iro2diffusesemshaderdata.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "iro2diffusesemshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

Iro2DiffuseSemShaderData::Iro2DiffuseSemShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_usesDiffuseMap()
    , m_diffuseGain()
    , m_diffuseInnerFilter()
    , m_diffuseOuterFilter()
    , m_usesReflectionMap()
    , m_reflectionGain()
    , m_reflectionInnerFilter()
    , m_reflectionOuterFilter()
    , m_usesNormalMap()
    , m_normalMapGain()
    , m_normalScaling()
    , m_normalDisturb()
    , m_postVertexColor()
    , m_postHemiFilter()
    , m_postGain()
    , m_gltfYUp()
{}

Iro2DiffuseSemShaderData::~Iro2DiffuseSemShaderData() = default;

bool Iro2DiffuseSemShaderData::usesDiffuseMap() const
{
    return m_usesDiffuseMap;
}

float Iro2DiffuseSemShaderData::diffuseGain() const
{
    return m_diffuseGain;
}

QVector3D Iro2DiffuseSemShaderData::diffuseInnerFilter() const
{
    return m_diffuseInnerFilter;
}

QVector3D Iro2DiffuseSemShaderData::diffuseOuterFilter() const
{
    return m_diffuseOuterFilter;
}

bool Iro2DiffuseSemShaderData::usesReflectionMap() const
{
    return m_usesReflectionMap;
}

float Iro2DiffuseSemShaderData::reflectionGain() const
{
    return m_reflectionGain;
}

QVector3D Iro2DiffuseSemShaderData::reflectionInnerFilter() const
{
    return m_reflectionInnerFilter;
}

QVector3D Iro2DiffuseSemShaderData::reflectionOuterFilter() const
{
    return m_reflectionOuterFilter;
}

bool Iro2DiffuseSemShaderData::usesNormalMap() const
{
    return m_usesNormalMap;
}

float Iro2DiffuseSemShaderData::normalMapGain() const
{
    return m_normalMapGain;
}

QVector3D Iro2DiffuseSemShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D Iro2DiffuseSemShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float Iro2DiffuseSemShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

QVector3D Iro2DiffuseSemShaderData::postHemiFilter() const
{
    return m_postHemiFilter;
}

float Iro2DiffuseSemShaderData::postGain() const
{
    return m_postGain;
}

bool Iro2DiffuseSemShaderData::gltfYUp() const
{
    return m_gltfYUp;
}


void Iro2DiffuseSemShaderData::setUsesDiffuseMap(bool usesDiffuseMap)
{
    if (m_usesDiffuseMap == usesDiffuseMap)
        return;
    m_usesDiffuseMap = usesDiffuseMap;
    emit usesDiffuseMapChanged(usesDiffuseMap);
}

void Iro2DiffuseSemShaderData::setDiffuseGain(float diffuseGain)
{
    if (m_diffuseGain == diffuseGain)
        return;
    m_diffuseGain = diffuseGain;
    emit diffuseGainChanged(diffuseGain);
}

void Iro2DiffuseSemShaderData::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    if (m_diffuseInnerFilter == diffuseInnerFilter)
        return;
    m_diffuseInnerFilter = diffuseInnerFilter;
    emit diffuseInnerFilterChanged(diffuseInnerFilter);
}

void Iro2DiffuseSemShaderData::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    if (m_diffuseOuterFilter == diffuseOuterFilter)
        return;
    m_diffuseOuterFilter = diffuseOuterFilter;
    emit diffuseOuterFilterChanged(diffuseOuterFilter);
}

void Iro2DiffuseSemShaderData::setUsesReflectionMap(bool usesReflectionMap)
{
    if (m_usesReflectionMap == usesReflectionMap)
        return;
    m_usesReflectionMap = usesReflectionMap;
    emit usesReflectionMapChanged(usesReflectionMap);
}

void Iro2DiffuseSemShaderData::setReflectionGain(float reflectionGain)
{
    if (m_reflectionGain == reflectionGain)
        return;
    m_reflectionGain = reflectionGain;
    emit reflectionGainChanged(reflectionGain);
}

void Iro2DiffuseSemShaderData::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    if (m_reflectionInnerFilter == reflectionInnerFilter)
        return;
    m_reflectionInnerFilter = reflectionInnerFilter;
    emit reflectionInnerFilterChanged(reflectionInnerFilter);
}

void Iro2DiffuseSemShaderData::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    if (m_reflectionOuterFilter == reflectionOuterFilter)
        return;
    m_reflectionOuterFilter = reflectionOuterFilter;
    emit reflectionOuterFilterChanged(reflectionOuterFilter);
}

void Iro2DiffuseSemShaderData::setUsesNormalMap(bool usesNormalMap)
{
    if (m_usesNormalMap == usesNormalMap)
        return;
    m_usesNormalMap = usesNormalMap;
    emit usesNormalMapChanged(usesNormalMap);
}

void Iro2DiffuseSemShaderData::setNormalMapGain(float normalMapGain)
{
    if (m_normalMapGain == normalMapGain)
        return;
    m_normalMapGain = normalMapGain;
    emit normalMapGainChanged(normalMapGain);
}

void Iro2DiffuseSemShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void Iro2DiffuseSemShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void Iro2DiffuseSemShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void Iro2DiffuseSemShaderData::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    if (m_postHemiFilter == postHemiFilter)
        return;
    m_postHemiFilter = postHemiFilter;
    emit postHemiFilterChanged(postHemiFilter);
}

void Iro2DiffuseSemShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void Iro2DiffuseSemShaderData::setGltfYUp(bool gltfYUp)
{
    if (m_gltfYUp == gltfYUp)
        return;
    m_gltfYUp = gltfYUp;
    emit gltfYUpChanged(gltfYUp);
}



} // namespace Kuesa

QT_END_NAMESPACE
