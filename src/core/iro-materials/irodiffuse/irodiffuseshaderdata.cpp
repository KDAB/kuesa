/*
    irodiffuseshaderdata.cpp

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

#include "irodiffuseshaderdata_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {
IroDiffuseShaderData::IroDiffuseShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_normalScaling()
    , m_normalDisturb()
    , m_postVertexColor()
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
{
}

IroDiffuseShaderData::~IroDiffuseShaderData() = default;

QVector3D IroDiffuseShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D IroDiffuseShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float IroDiffuseShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float IroDiffuseShaderData::postGain() const
{
    return m_postGain;
}

float IroDiffuseShaderData::reflectionGain() const
{
    return m_reflectionGain;
}

QVector3D IroDiffuseShaderData::reflectionInnerFilter() const
{
    return m_reflectionInnerFilter;
}

QVector3D IroDiffuseShaderData::reflectionOuterFilter() const
{
    return m_reflectionOuterFilter;
}

bool IroDiffuseShaderData::usesReflectionMap() const
{
    return m_usesReflectionMap;
}

bool IroDiffuseShaderData::projectReflectionMap() const
{
    return m_projectReflectionMap;
}

QVector3D IroDiffuseShaderData::diffuseInnerFilter() const
{
    return m_diffuseInnerFilter;
}

QVector3D IroDiffuseShaderData::diffuseOuterFilter() const
{
    return m_diffuseOuterFilter;
}

float IroDiffuseShaderData::diffuseGain() const
{
    return m_diffuseGain;
}

bool IroDiffuseShaderData::usesDiffuseMap() const
{
    return m_usesDiffuseMap;
}

void IroDiffuseShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void IroDiffuseShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void IroDiffuseShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void IroDiffuseShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void IroDiffuseShaderData::setReflectionGain(float reflectionGain)
{
    if (m_reflectionGain == reflectionGain)
        return;
    m_reflectionGain = reflectionGain;
    emit reflectionGainChanged(reflectionGain);
}

void IroDiffuseShaderData::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    if (m_reflectionInnerFilter == reflectionInnerFilter)
        return;
    m_reflectionInnerFilter = reflectionInnerFilter;
    emit reflectionInnerFilterChanged(reflectionInnerFilter);
}

void IroDiffuseShaderData::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    if (m_reflectionOuterFilter == reflectionOuterFilter)
        return;
    m_reflectionOuterFilter = reflectionOuterFilter;
    emit reflectionOuterFilterChanged(reflectionOuterFilter);
}

void IroDiffuseShaderData::setUsesReflectionMap(bool usesReflectionMap)
{
    if (m_usesReflectionMap == usesReflectionMap)
        return;
    m_usesReflectionMap = usesReflectionMap;
    emit usesReflectionMapChanged(usesReflectionMap);
}

void IroDiffuseShaderData::setProjectReflectionMap(bool projectReflectionMap)
{
    if (m_projectReflectionMap == projectReflectionMap)
        return;
    m_projectReflectionMap = projectReflectionMap;
    emit projectReflectionMapChanged(projectReflectionMap);
}

void IroDiffuseShaderData::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    if (m_diffuseInnerFilter == diffuseInnerFilter)
        return;
    m_diffuseInnerFilter = diffuseInnerFilter;
    emit diffuseInnerFilterChanged(diffuseInnerFilter);
}

void IroDiffuseShaderData::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    if (m_diffuseOuterFilter == diffuseOuterFilter)
        return;
    m_diffuseOuterFilter = diffuseOuterFilter;
    emit diffuseOuterFilterChanged(diffuseOuterFilter);
}

void IroDiffuseShaderData::setDiffuseGain(float diffuseGain)
{
    if (m_diffuseGain == diffuseGain)
        return;
    m_diffuseGain = diffuseGain;
    emit diffuseGainChanged(diffuseGain);
}

void IroDiffuseShaderData::setUsesDiffuseMap(bool usesDiffuseMap)
{
    if (m_usesDiffuseMap == usesDiffuseMap)
        return;
    m_usesDiffuseMap = usesDiffuseMap;
    emit usesDiffuseMapChanged(usesDiffuseMap);
}

} // namespace Kuesa

QT_END_NAMESPACE
