
/*
    iroglassaddshaderdata.cpp

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

#include "iroglassaddshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

IroGlassAddShaderData::IroGlassAddShaderData(Qt3DCore::QNode *parent)
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
    , m_glassInnerFilter()
    , m_glassOuterFilter()
    , m_diffuseInnerFilter()
    , m_diffuseOuterFilter()
{}

IroGlassAddShaderData::~IroGlassAddShaderData() = default;

QVector3D IroGlassAddShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D IroGlassAddShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float IroGlassAddShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float IroGlassAddShaderData::postGain() const
{
    return m_postGain;
}

float IroGlassAddShaderData::reflectionGain() const
{
    return m_reflectionGain;
}

QVector3D IroGlassAddShaderData::reflectionInnerFilter() const
{
    return m_reflectionInnerFilter;
}

QVector3D IroGlassAddShaderData::reflectionOuterFilter() const
{
    return m_reflectionOuterFilter;
}

bool IroGlassAddShaderData::usesReflectionMap() const
{
    return m_usesReflectionMap;
}

bool IroGlassAddShaderData::projectReflectionMap() const
{
    return m_projectReflectionMap;
}

QVector3D IroGlassAddShaderData::glassInnerFilter() const
{
    return m_glassInnerFilter;
}

QVector3D IroGlassAddShaderData::glassOuterFilter() const
{
    return m_glassOuterFilter;
}

QVector3D IroGlassAddShaderData::diffuseInnerFilter() const
{
    return m_diffuseInnerFilter;
}

QVector3D IroGlassAddShaderData::diffuseOuterFilter() const
{
    return m_diffuseOuterFilter;
}


void IroGlassAddShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void IroGlassAddShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void IroGlassAddShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void IroGlassAddShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void IroGlassAddShaderData::setReflectionGain(float reflectionGain)
{
    if (m_reflectionGain == reflectionGain)
        return;
    m_reflectionGain = reflectionGain;
    emit reflectionGainChanged(reflectionGain);
}

void IroGlassAddShaderData::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    if (m_reflectionInnerFilter == reflectionInnerFilter)
        return;
    m_reflectionInnerFilter = reflectionInnerFilter;
    emit reflectionInnerFilterChanged(reflectionInnerFilter);
}

void IroGlassAddShaderData::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    if (m_reflectionOuterFilter == reflectionOuterFilter)
        return;
    m_reflectionOuterFilter = reflectionOuterFilter;
    emit reflectionOuterFilterChanged(reflectionOuterFilter);
}

void IroGlassAddShaderData::setUsesReflectionMap(bool usesReflectionMap)
{
    if (m_usesReflectionMap == usesReflectionMap)
        return;
    m_usesReflectionMap = usesReflectionMap;
    emit usesReflectionMapChanged(usesReflectionMap);
}

void IroGlassAddShaderData::setProjectReflectionMap(bool projectReflectionMap)
{
    if (m_projectReflectionMap == projectReflectionMap)
        return;
    m_projectReflectionMap = projectReflectionMap;
    emit projectReflectionMapChanged(projectReflectionMap);
}

void IroGlassAddShaderData::setGlassInnerFilter(const QVector3D &glassInnerFilter)
{
    if (m_glassInnerFilter == glassInnerFilter)
        return;
    m_glassInnerFilter = glassInnerFilter;
    emit glassInnerFilterChanged(glassInnerFilter);
}

void IroGlassAddShaderData::setGlassOuterFilter(const QVector3D &glassOuterFilter)
{
    if (m_glassOuterFilter == glassOuterFilter)
        return;
    m_glassOuterFilter = glassOuterFilter;
    emit glassOuterFilterChanged(glassOuterFilter);
}

void IroGlassAddShaderData::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    if (m_diffuseInnerFilter == diffuseInnerFilter)
        return;
    m_diffuseInnerFilter = diffuseInnerFilter;
    emit diffuseInnerFilterChanged(diffuseInnerFilter);
}

void IroGlassAddShaderData::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    if (m_diffuseOuterFilter == diffuseOuterFilter)
        return;
    m_diffuseOuterFilter = diffuseOuterFilter;
    emit diffuseOuterFilterChanged(diffuseOuterFilter);
}



} // namespace Kuesa

QT_END_NAMESPACE
