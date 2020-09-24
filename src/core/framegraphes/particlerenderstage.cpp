/*
    particlerenderstage.cpp

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

#include "particlerenderstage_p.h"

#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QMemoryBarrier>
#include <Qt3DRender/QDispatchCompute>

QT_BEGIN_NAMESPACE

namespace Kuesa {

ParticleRenderStage::ParticleRenderStage(Qt3DRender::QFrameGraphNode *parent)
    : AbstractRenderStage(parent)
{
    setObjectName(QStringLiteral("KuesaParticleRenderStage"));

    const auto addFilterKey = [](auto *parent, const QString &name, const QVariant &value) {
        auto filterKey = new Qt3DRender::QFilterKey(parent);
        filterKey->setName(name);
        filterKey->setValue(value);
        parent->addMatch(filterKey);
    };

    // emit
    {
        auto passFilter = new Qt3DRender::QRenderPassFilter(this);
        addFilterKey(passFilter, QStringLiteral("KuesaComputeStage"), QStringLiteral("ParticleEmit"));

        auto *dispatchCompute = new Qt3DRender::QDispatchCompute(passFilter);
        dispatchCompute->setWorkGroupX(1);
        dispatchCompute->setWorkGroupY(1);
        dispatchCompute->setWorkGroupZ(1);
    }

    // simulate
    {
        auto passFilter = new Qt3DRender::QRenderPassFilter(this);
        addFilterKey(passFilter, QStringLiteral("KuesaComputeStage"), QStringLiteral("ParticleSimulate"));

        auto *memoryBarrier = new Qt3DRender::QMemoryBarrier(passFilter);
        memoryBarrier->setWaitOperations(Qt3DRender::QMemoryBarrier::ShaderStorage);

        auto *dispatchCompute = new Qt3DRender::QDispatchCompute(memoryBarrier);
        dispatchCompute->setWorkGroupX(1);
        dispatchCompute->setWorkGroupY(1);
        dispatchCompute->setWorkGroupZ(1);
    }

    // sort
    {
        auto passFilter = new Qt3DRender::QRenderPassFilter(this);
        addFilterKey(passFilter, QStringLiteral("KuesaComputeStage"), QStringLiteral("ParticleSort"));

        auto *memoryBarrier = new Qt3DRender::QMemoryBarrier(passFilter);
        memoryBarrier->setWaitOperations(Qt3DRender::QMemoryBarrier::ShaderStorage);

        auto *dispatchCompute = new Qt3DRender::QDispatchCompute(memoryBarrier);
        dispatchCompute->setWorkGroupX(1);
        dispatchCompute->setWorkGroupY(1);
        dispatchCompute->setWorkGroupZ(1);
    }

    // render
    {
        auto passFilter = new Qt3DRender::QRenderPassFilter(this);
        addFilterKey(passFilter, QStringLiteral("KuesaDrawStage"), QStringLiteral("ParticleRender"));

        auto *memoryBarrier = new Qt3DRender::QMemoryBarrier(passFilter);
        memoryBarrier->setWaitOperations(Qt3DRender::QMemoryBarrier::Operations(Qt3DRender::QMemoryBarrier::VertexAttributeArray | Qt3DRender::QMemoryBarrier::ShaderStorage));
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
