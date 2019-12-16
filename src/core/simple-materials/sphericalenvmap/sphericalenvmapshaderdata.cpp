
/*
    sphericalenvmapshaderdata.cpp

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

#include "sphericalenvmapshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

SphericalEnvMapShaderData::SphericalEnvMapShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_factors()
    , m_disturbation()
    , m_postVertexColor()
    , m_postGain()
    , m_semGain()
    , m_sem(nullptr)
{}

SphericalEnvMapShaderData::~SphericalEnvMapShaderData() = default;

QVector3D SphericalEnvMapShaderData::factors() const
{
    return m_factors;
}

QVector2D SphericalEnvMapShaderData::disturbation() const
{
    return m_disturbation;
}

float SphericalEnvMapShaderData::postVertexColor() const
{
    return m_postVertexColor;
}

float SphericalEnvMapShaderData::postGain() const
{
    return m_postGain;
}

float SphericalEnvMapShaderData::semGain() const
{
    return m_semGain;
}

Qt3DRender::QAbstractTexture * SphericalEnvMapShaderData::sem() const
{
    return m_sem;
}


void SphericalEnvMapShaderData::setFactors(const QVector3D &factors)
{
    if (m_factors == factors)
        return;
    m_factors = factors;
    emit factorsChanged(factors);
}

void SphericalEnvMapShaderData::setDisturbation(const QVector2D &disturbation)
{
    if (m_disturbation == disturbation)
        return;
    m_disturbation = disturbation;
    emit disturbationChanged(disturbation);
}

void SphericalEnvMapShaderData::setPostVertexColor(float postVertexColor)
{
    if (m_postVertexColor == postVertexColor)
        return;
    m_postVertexColor = postVertexColor;
    emit postVertexColorChanged(postVertexColor);
}

void SphericalEnvMapShaderData::setPostGain(float postGain)
{
    if (m_postGain == postGain)
        return;
    m_postGain = postGain;
    emit postGainChanged(postGain);
}

void SphericalEnvMapShaderData::setSemGain(float semGain)
{
    if (m_semGain == semGain)
        return;
    m_semGain = semGain;
    emit semGainChanged(semGain);
}

void SphericalEnvMapShaderData::setSem(Qt3DRender::QAbstractTexture * sem)
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
        d->registerDestructionHelper(m_sem, &SphericalEnvMapShaderData::setSem, m_sem);
    }
    emit semChanged(m_sem);
}



} // namespace Kuesa

QT_END_NAMESPACE
