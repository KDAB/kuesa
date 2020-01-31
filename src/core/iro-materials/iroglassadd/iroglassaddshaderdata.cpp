
/*
    iroglassaddshaderdata.cpp

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

#include "iroglassaddshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

IroGlassAddShaderData::IroGlassAddShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_factors()
    , m_disturbation()
    , m_postVertexColor()
    , m_postGain()
    , m_semGain()
    , m_sem(nullptr)
    , m_semInnerFilter()
    , m_semOuterFilter()
    , m_semInnerAlpha()
    , m_semOuterAlpha()
    , m_glassInnerFilter()
    , m_glassOuterFilter()
{}

IroGlassAddShaderData::~IroGlassAddShaderData() = default;

QVector3D IroGlassAddShaderData::factors() const
{
    return m_factors;
}

QVector2D IroGlassAddShaderData::disturbation() const
{
    return m_disturbation;
}

float IroGlassAddShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float IroGlassAddShaderData::postGain() const
{
    return m_postGain;
}

float IroGlassAddShaderData::semGain() const
{
    return m_semGain;
}

Qt3DRender::QAbstractTexture * IroGlassAddShaderData::sem() const
{
    return m_sem;
}

QVector3D IroGlassAddShaderData::semInnerFilter() const
{
    return m_semInnerFilter;
}

QVector3D IroGlassAddShaderData::semOuterFilter() const
{
    return m_semOuterFilter;
}

float IroGlassAddShaderData::semInnerAlpha() const
{
    return m_semInnerAlpha;
}

float IroGlassAddShaderData::semOuterAlpha() const
{
    return m_semOuterAlpha;
}

QVector3D IroGlassAddShaderData::glassInnerFilter() const
{
    return m_glassInnerFilter;
}

QVector3D IroGlassAddShaderData::glassOuterFilter() const
{
    return m_glassOuterFilter;
}


void IroGlassAddShaderData::setFactors(const QVector3D &factors)
{
    if (m_factors == factors)
        return;
    m_factors = factors;
    emit factorsChanged(factors);
}

void IroGlassAddShaderData::setDisturbation(const QVector2D &disturbation)
{
    if (m_disturbation == disturbation)
        return;
    m_disturbation = disturbation;
    emit disturbationChanged(disturbation);
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

void IroGlassAddShaderData::setSemGain(float semGain)
{
    if (m_semGain == semGain)
        return;
    m_semGain = semGain;
    emit semGainChanged(semGain);
}

void IroGlassAddShaderData::setSem(Qt3DRender::QAbstractTexture * sem)
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
        d->registerDestructionHelper(m_sem, &IroGlassAddShaderData::setSem, m_sem);
    }
    emit semChanged(m_sem);
}

void IroGlassAddShaderData::setSemInnerFilter(const QVector3D &semInnerFilter)
{
    if (m_semInnerFilter == semInnerFilter)
        return;
    m_semInnerFilter = semInnerFilter;
    emit semInnerFilterChanged(semInnerFilter);
}

void IroGlassAddShaderData::setSemOuterFilter(const QVector3D &semOuterFilter)
{
    if (m_semOuterFilter == semOuterFilter)
        return;
    m_semOuterFilter = semOuterFilter;
    emit semOuterFilterChanged(semOuterFilter);
}

void IroGlassAddShaderData::setSemInnerAlpha(float semInnerAlpha)
{
    if (m_semInnerAlpha == semInnerAlpha)
        return;
    m_semInnerAlpha = semInnerAlpha;
    emit semInnerAlphaChanged(semInnerAlpha);
}

void IroGlassAddShaderData::setSemOuterAlpha(float semOuterAlpha)
{
    if (m_semOuterAlpha == semOuterAlpha)
        return;
    m_semOuterAlpha = semOuterAlpha;
    emit semOuterAlphaChanged(semOuterAlpha);
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



} // namespace Kuesa

QT_END_NAMESPACE
