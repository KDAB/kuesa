
/*
    irodiffusehemishaderdata.cpp

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

#include "irodiffusehemishaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

IroDiffuseHemiShaderData::IroDiffuseHemiShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_normalScaling()
    , m_normalDisturb()
    , m_postVertexColor()
    , m_postHemiFilter()
    , m_postGain()
    , m_reflectionGain()
    , m_reflectionInnerFilter()
    , m_reflectionOuterFilter()
    , m_usesReflectionMap()
    , m_projectReflectionMap()
    , m_diffuseInnerFilter()
    , m_diffuseOuterFilter()
    , m_diffuseGain()
    , m_usesDiffuseMap()
    , m_gltfYUp()
{}

IroDiffuseHemiShaderData::~IroDiffuseHemiShaderData() = default;

QVector3D IroDiffuseHemiShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D IroDiffuseHemiShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float IroDiffuseHemiShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

QVector3D IroDiffuseHemiShaderData::postHemiFilter() const
{
    return m_postHemiFilter;
}

float IroDiffuseHemiShaderData::postGain() const
{
    return m_postGain;
}

float IroDiffuseHemiShaderData::reflectionGain() const
{
    return m_reflectionGain;
}

QVector3D IroDiffuseHemiShaderData::reflectionInnerFilter() const
{
    return m_reflectionInnerFilter;
}

QVector3D IroDiffuseHemiShaderData::reflectionOuterFilter() const
{
    return m_reflectionOuterFilter;
}

bool IroDiffuseHemiShaderData::usesReflectionMap() const
{
    return m_usesReflectionMap;
}

bool IroDiffuseHemiShaderData::projectReflectionMap() const
{
    return m_projectReflectionMap;
}

QVector3D IroDiffuseHemiShaderData::diffuseInnerFilter() const
{
    return m_diffuseInnerFilter;
}

QVector3D IroDiffuseHemiShaderData::diffuseOuterFilter() const
{
    return m_diffuseOuterFilter;
}

float IroDiffuseHemiShaderData::diffuseGain() const
{
    return m_diffuseGain;
}

bool IroDiffuseHemiShaderData::usesDiffuseMap() const
{
    return m_usesDiffuseMap;
}

bool IroDiffuseHemiShaderData::gltfYUp() const
{
    return m_gltfYUp;
}


void IroDiffuseHemiShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void IroDiffuseHemiShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void IroDiffuseHemiShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void IroDiffuseHemiShaderData::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    if (m_postHemiFilter == postHemiFilter)
        return;
    m_postHemiFilter = postHemiFilter;
    emit postHemiFilterChanged(postHemiFilter);
}

void IroDiffuseHemiShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void IroDiffuseHemiShaderData::setReflectionGain(float reflectionGain)
{
    if (m_reflectionGain == reflectionGain)
        return;
    m_reflectionGain = reflectionGain;
    emit reflectionGainChanged(reflectionGain);
}

void IroDiffuseHemiShaderData::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    if (m_reflectionInnerFilter == reflectionInnerFilter)
        return;
    m_reflectionInnerFilter = reflectionInnerFilter;
    emit reflectionInnerFilterChanged(reflectionInnerFilter);
}

void IroDiffuseHemiShaderData::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    if (m_reflectionOuterFilter == reflectionOuterFilter)
        return;
    m_reflectionOuterFilter = reflectionOuterFilter;
    emit reflectionOuterFilterChanged(reflectionOuterFilter);
}

void IroDiffuseHemiShaderData::setUsesReflectionMap(bool usesReflectionMap)
{
    if (m_usesReflectionMap == usesReflectionMap)
        return;
    m_usesReflectionMap = usesReflectionMap;
    emit usesReflectionMapChanged(usesReflectionMap);
}

void IroDiffuseHemiShaderData::setProjectReflectionMap(bool projectReflectionMap)
{
    if (m_projectReflectionMap == projectReflectionMap)
        return;
    m_projectReflectionMap = projectReflectionMap;
    emit projectReflectionMapChanged(projectReflectionMap);
}

void IroDiffuseHemiShaderData::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    if (m_diffuseInnerFilter == diffuseInnerFilter)
        return;
    m_diffuseInnerFilter = diffuseInnerFilter;
    emit diffuseInnerFilterChanged(diffuseInnerFilter);
}

void IroDiffuseHemiShaderData::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    if (m_diffuseOuterFilter == diffuseOuterFilter)
        return;
    m_diffuseOuterFilter = diffuseOuterFilter;
    emit diffuseOuterFilterChanged(diffuseOuterFilter);
}

void IroDiffuseHemiShaderData::setDiffuseGain(float diffuseGain)
{
    if (m_diffuseGain == diffuseGain)
        return;
    m_diffuseGain = diffuseGain;
    emit diffuseGainChanged(diffuseGain);
}

void IroDiffuseHemiShaderData::setUsesDiffuseMap(bool usesDiffuseMap)
{
    if (m_usesDiffuseMap == usesDiffuseMap)
        return;
    m_usesDiffuseMap = usesDiffuseMap;
    emit usesDiffuseMapChanged(usesDiffuseMap);
}

void IroDiffuseHemiShaderData::setGltfYUp(bool gltfYUp)
{
    if (m_gltfYUp == gltfYUp)
        return;
    m_gltfYUp = gltfYUp;
    emit gltfYUpChanged(gltfYUp);
}



} // namespace Kuesa

QT_END_NAMESPACE
