
/*
    irodiffusealphashaderdata.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "irodiffusealphashaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

IroDiffuseAlphaShaderData::IroDiffuseAlphaShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_normalScaling()
    , m_normalDisturb()
    , m_postVertexColor()
    , m_postGain()
    , m_reflectionGain()
    , m_reflectionMap(nullptr)
    , m_reflectionInnerFilter()
    , m_reflectionOuterFilter()
    , m_usesReflectionMap()
    , m_reflectionInnerAlpha()
    , m_reflectionOuterAlpha()
    , m_alphaGain()
{}

IroDiffuseAlphaShaderData::~IroDiffuseAlphaShaderData() = default;

QVector3D IroDiffuseAlphaShaderData::normalScaling() const
{
    return m_normalScaling;
}

QVector2D IroDiffuseAlphaShaderData::normalDisturb() const
{
    return m_normalDisturb;
}

float IroDiffuseAlphaShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float IroDiffuseAlphaShaderData::postGain() const
{
    return m_postGain;
}

float IroDiffuseAlphaShaderData::reflectionGain() const
{
    return m_reflectionGain;
}

Qt3DRender::QAbstractTexture * IroDiffuseAlphaShaderData::reflectionMap() const
{
    return m_reflectionMap;
}

QVector3D IroDiffuseAlphaShaderData::reflectionInnerFilter() const
{
    return m_reflectionInnerFilter;
}

QVector3D IroDiffuseAlphaShaderData::reflectionOuterFilter() const
{
    return m_reflectionOuterFilter;
}

bool IroDiffuseAlphaShaderData::usesReflectionMap() const
{
    return m_usesReflectionMap;
}

float IroDiffuseAlphaShaderData::reflectionInnerAlpha() const
{
    return m_reflectionInnerAlpha;
}

float IroDiffuseAlphaShaderData::reflectionOuterAlpha() const
{
    return m_reflectionOuterAlpha;
}

float IroDiffuseAlphaShaderData::alphaGain() const
{
    return m_alphaGain;
}


void IroDiffuseAlphaShaderData::setNormalScaling(const QVector3D &normalScaling)
{
    if (m_normalScaling == normalScaling)
        return;
    m_normalScaling = normalScaling;
    emit normalScalingChanged(normalScaling);
}

void IroDiffuseAlphaShaderData::setNormalDisturb(const QVector2D &normalDisturb)
{
    if (m_normalDisturb == normalDisturb)
        return;
    m_normalDisturb = normalDisturb;
    emit normalDisturbChanged(normalDisturb);
}

void IroDiffuseAlphaShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void IroDiffuseAlphaShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void IroDiffuseAlphaShaderData::setReflectionGain(float reflectionGain)
{
    if (m_reflectionGain == reflectionGain)
        return;
    m_reflectionGain = reflectionGain;
    emit reflectionGainChanged(reflectionGain);
}

void IroDiffuseAlphaShaderData::setReflectionMap(Qt3DRender::QAbstractTexture * reflectionMap)
{
    if (m_reflectionMap == reflectionMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_reflectionMap != nullptr)
        d->unregisterDestructionHelper(m_reflectionMap);
    m_reflectionMap = reflectionMap;
    if (m_reflectionMap != nullptr) {
        if (m_reflectionMap->parent() == nullptr)
            m_reflectionMap->setParent(this);
        d->registerDestructionHelper(m_reflectionMap, &IroDiffuseAlphaShaderData::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void IroDiffuseAlphaShaderData::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    if (m_reflectionInnerFilter == reflectionInnerFilter)
        return;
    m_reflectionInnerFilter = reflectionInnerFilter;
    emit reflectionInnerFilterChanged(reflectionInnerFilter);
}

void IroDiffuseAlphaShaderData::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    if (m_reflectionOuterFilter == reflectionOuterFilter)
        return;
    m_reflectionOuterFilter = reflectionOuterFilter;
    emit reflectionOuterFilterChanged(reflectionOuterFilter);
}

void IroDiffuseAlphaShaderData::setUsesReflectionMap(bool usesReflectionMap)
{
    if (m_usesReflectionMap == usesReflectionMap)
        return;
    m_usesReflectionMap = usesReflectionMap;
    emit usesReflectionMapChanged(usesReflectionMap);
}

void IroDiffuseAlphaShaderData::setReflectionInnerAlpha(float reflectionInnerAlpha)
{
    if (m_reflectionInnerAlpha == reflectionInnerAlpha)
        return;
    m_reflectionInnerAlpha = reflectionInnerAlpha;
    emit reflectionInnerAlphaChanged(reflectionInnerAlpha);
}

void IroDiffuseAlphaShaderData::setReflectionOuterAlpha(float reflectionOuterAlpha)
{
    if (m_reflectionOuterAlpha == reflectionOuterAlpha)
        return;
    m_reflectionOuterAlpha = reflectionOuterAlpha;
    emit reflectionOuterAlphaChanged(reflectionOuterAlpha);
}

void IroDiffuseAlphaShaderData::setAlphaGain(float alphaGain)
{
    if (m_alphaGain == alphaGain)
        return;
    m_alphaGain = alphaGain;
    emit alphaGainChanged(alphaGain);
}



} // namespace Kuesa

QT_END_NAMESPACE
