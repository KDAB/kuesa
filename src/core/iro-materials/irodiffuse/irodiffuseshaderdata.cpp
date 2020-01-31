
/*
    irodiffuseshaderdata.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "irodiffuseshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

IroDiffuseShaderData::IroDiffuseShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_factors()
    , m_disturbation()
    , m_postVertexColor()
    , m_postGain()
    , m_semGain()
    , m_sem(nullptr)
    , m_semInnerFilter()
    , m_semOuterFilter()
    , m_diffuseInnerFilter()
    , m_diffuseOuterFilter()
    , m_diffuseMap(nullptr)
    , m_diffuseFactor()
    , m_usesDiffuseMap()
{}

IroDiffuseShaderData::~IroDiffuseShaderData() = default;

QVector3D IroDiffuseShaderData::factors() const
{
    return m_factors;
}

QVector2D IroDiffuseShaderData::disturbation() const
{
    return m_disturbation;
}

float IroDiffuseShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float IroDiffuseShaderData::postGain() const
{
    return m_postGain;
}

float IroDiffuseShaderData::semGain() const
{
    return m_semGain;
}

Qt3DRender::QAbstractTexture * IroDiffuseShaderData::sem() const
{
    return m_sem;
}

QVector3D IroDiffuseShaderData::semInnerFilter() const
{
    return m_semInnerFilter;
}

QVector3D IroDiffuseShaderData::semOuterFilter() const
{
    return m_semOuterFilter;
}

QVector3D IroDiffuseShaderData::diffuseInnerFilter() const
{
    return m_diffuseInnerFilter;
}

QVector3D IroDiffuseShaderData::diffuseOuterFilter() const
{
    return m_diffuseOuterFilter;
}

Qt3DRender::QAbstractTexture * IroDiffuseShaderData::diffuseMap() const
{
    return m_diffuseMap;
}

QVector3D IroDiffuseShaderData::diffuseFactor() const
{
    return m_diffuseFactor;
}

bool IroDiffuseShaderData::usesDiffuseMap() const
{
    return m_usesDiffuseMap;
}


void IroDiffuseShaderData::setFactors(const QVector3D &factors)
{
    if (m_factors == factors)
        return;
    m_factors = factors;
    emit factorsChanged(factors);
}

void IroDiffuseShaderData::setDisturbation(const QVector2D &disturbation)
{
    if (m_disturbation == disturbation)
        return;
    m_disturbation = disturbation;
    emit disturbationChanged(disturbation);
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

void IroDiffuseShaderData::setSemGain(float semGain)
{
    if (m_semGain == semGain)
        return;
    m_semGain = semGain;
    emit semGainChanged(semGain);
}

void IroDiffuseShaderData::setSem(Qt3DRender::QAbstractTexture * sem)
{
    if (m_sem == sem)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_sem != nullptr)
        d->unregisterDestructionHelper(m_sem);
    m_sem = sem;
    if (m_sem != nullptr) {
        if (m_sem->parent() == nullptr)
            m_sem->setParent(this);
        d->registerDestructionHelper(m_sem, &IroDiffuseShaderData::setSem, m_sem);
    }
    emit semChanged(m_sem);
}

void IroDiffuseShaderData::setSemInnerFilter(const QVector3D &semInnerFilter)
{
    if (m_semInnerFilter == semInnerFilter)
        return;
    m_semInnerFilter = semInnerFilter;
    emit semInnerFilterChanged(semInnerFilter);
}

void IroDiffuseShaderData::setSemOuterFilter(const QVector3D &semOuterFilter)
{
    if (m_semOuterFilter == semOuterFilter)
        return;
    m_semOuterFilter = semOuterFilter;
    emit semOuterFilterChanged(semOuterFilter);
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

void IroDiffuseShaderData::setDiffuseMap(Qt3DRender::QAbstractTexture * diffuseMap)
{
    if (m_diffuseMap == diffuseMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_diffuseMap != nullptr)
        d->unregisterDestructionHelper(m_diffuseMap);
    m_diffuseMap = diffuseMap;
    if (m_diffuseMap != nullptr) {
        if (m_diffuseMap->parent() == nullptr)
            m_diffuseMap->setParent(this);
        d->registerDestructionHelper(m_diffuseMap, &IroDiffuseShaderData::setDiffuseMap, m_diffuseMap);
    }
    emit diffuseMapChanged(m_diffuseMap);
}

void IroDiffuseShaderData::setDiffuseFactor(const QVector3D &diffuseFactor)
{
    if (m_diffuseFactor == diffuseFactor)
        return;
    m_diffuseFactor = diffuseFactor;
    emit diffuseFactorChanged(diffuseFactor);
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
