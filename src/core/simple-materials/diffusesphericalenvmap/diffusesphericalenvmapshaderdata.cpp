
/*
    diffusesphericalenvmapshaderdata.cpp

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

#include "diffusesphericalenvmapshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

DiffuseSphericalEnvMapShaderData::DiffuseSphericalEnvMapShaderData(Qt3DCore::QNode *parent)
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

DiffuseSphericalEnvMapShaderData::~DiffuseSphericalEnvMapShaderData() = default;

QVector3D DiffuseSphericalEnvMapShaderData::factors() const
{
    return m_factors;
}

QVector2D DiffuseSphericalEnvMapShaderData::disturbation() const
{
    return m_disturbation;
}

float DiffuseSphericalEnvMapShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float DiffuseSphericalEnvMapShaderData::postGain() const
{
    return m_postGain;
}

float DiffuseSphericalEnvMapShaderData::semGain() const
{
    return m_semGain;
}

Qt3DRender::QAbstractTexture * DiffuseSphericalEnvMapShaderData::sem() const
{
    return m_sem;
}

QVector3D DiffuseSphericalEnvMapShaderData::semInnerFilter() const
{
    return m_semInnerFilter;
}

QVector3D DiffuseSphericalEnvMapShaderData::semOuterFilter() const
{
    return m_semOuterFilter;
}

QVector3D DiffuseSphericalEnvMapShaderData::diffuseInnerFilter() const
{
    return m_diffuseInnerFilter;
}

QVector3D DiffuseSphericalEnvMapShaderData::diffuseOuterFilter() const
{
    return m_diffuseOuterFilter;
}

Qt3DRender::QAbstractTexture * DiffuseSphericalEnvMapShaderData::diffuseMap() const
{
    return m_diffuseMap;
}

QVector3D DiffuseSphericalEnvMapShaderData::diffuseFactor() const
{
    return m_diffuseFactor;
}

bool DiffuseSphericalEnvMapShaderData::usesDiffuseMap() const
{
    return m_usesDiffuseMap;
}


void DiffuseSphericalEnvMapShaderData::setFactors(const QVector3D &factors)
{
    if (m_factors == factors)
        return;
    m_factors = factors;
    emit factorsChanged(factors);
}

void DiffuseSphericalEnvMapShaderData::setDisturbation(const QVector2D &disturbation)
{
    if (m_disturbation == disturbation)
        return;
    m_disturbation = disturbation;
    emit disturbationChanged(disturbation);
}

void DiffuseSphericalEnvMapShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void DiffuseSphericalEnvMapShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void DiffuseSphericalEnvMapShaderData::setSemGain(float semGain)
{
    if (m_semGain == semGain)
        return;
    m_semGain = semGain;
    emit semGainChanged(semGain);
}

void DiffuseSphericalEnvMapShaderData::setSem(Qt3DRender::QAbstractTexture * sem)
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
        d->registerDestructionHelper(m_sem, &DiffuseSphericalEnvMapShaderData::setSem, m_sem);
    }
    emit semChanged(m_sem);
}

void DiffuseSphericalEnvMapShaderData::setSemInnerFilter(const QVector3D &semInnerFilter)
{
    if (m_semInnerFilter == semInnerFilter)
        return;
    m_semInnerFilter = semInnerFilter;
    emit semInnerFilterChanged(semInnerFilter);
}

void DiffuseSphericalEnvMapShaderData::setSemOuterFilter(const QVector3D &semOuterFilter)
{
    if (m_semOuterFilter == semOuterFilter)
        return;
    m_semOuterFilter = semOuterFilter;
    emit semOuterFilterChanged(semOuterFilter);
}

void DiffuseSphericalEnvMapShaderData::setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter)
{
    if (m_diffuseInnerFilter == diffuseInnerFilter)
        return;
    m_diffuseInnerFilter = diffuseInnerFilter;
    emit diffuseInnerFilterChanged(diffuseInnerFilter);
}

void DiffuseSphericalEnvMapShaderData::setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter)
{
    if (m_diffuseOuterFilter == diffuseOuterFilter)
        return;
    m_diffuseOuterFilter = diffuseOuterFilter;
    emit diffuseOuterFilterChanged(diffuseOuterFilter);
}

void DiffuseSphericalEnvMapShaderData::setDiffuseMap(Qt3DRender::QAbstractTexture * diffuseMap)
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
        d->registerDestructionHelper(m_diffuseMap, &DiffuseSphericalEnvMapShaderData::setDiffuseMap, m_diffuseMap);
    }
    emit diffuseMapChanged(m_diffuseMap);
}

void DiffuseSphericalEnvMapShaderData::setDiffuseFactor(const QVector3D &diffuseFactor)
{
    if (m_diffuseFactor == diffuseFactor)
        return;
    m_diffuseFactor = diffuseFactor;
    emit diffuseFactorChanged(diffuseFactor);
}

void DiffuseSphericalEnvMapShaderData::setUsesDiffuseMap(bool usesDiffuseMap)
{
    if (m_usesDiffuseMap == usesDiffuseMap)
        return;
    m_usesDiffuseMap = usesDiffuseMap;
    emit usesDiffuseMapChanged(usesDiffuseMap);
}



} // namespace Kuesa

QT_END_NAMESPACE
