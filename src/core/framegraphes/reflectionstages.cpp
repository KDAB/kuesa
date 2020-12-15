/*
    reflectionstages.cpp

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

#include "reflectionstages_p.h"

#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qrendertarget.h>
#include <QVector4D>

#include "zfillrenderstage_p.h"
#include "opaquerenderstage_p.h"
#include "transparentrenderstage_p.h"
#include "framegraphutils_p.h"

QT_BEGIN_NAMESPACE

using namespace Kuesa;

ReflectionStages::ReflectionStages(Qt3DRender::QFrameGraphNode *parent)
    : SceneStages(parent)
{
    // Enable Reflections on the SceneStages
    m_reflectiveEnabledParameter->setValue(true);

    m_renderTargetSelector = new Qt3DRender::QRenderTargetSelector(this);
    auto clearDepth = new Qt3DRender::QClearBuffers(m_renderTargetSelector);
    clearDepth->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    new Qt3DRender::QNoDraw(clearDepth);

    // Move regular SceneStage subtree root to be a child of the RenderTargetSelector
    m_cameraSelector->setParent(m_renderTargetSelector);

    // Create Render Target
    Qt3DRender::QRenderTarget *target = FrameGraphUtils::createRenderTarget(FrameGraphUtils::IncludeDepth,
                                                                            m_renderTargetSelector,
                                                                            {512, 512});
    m_renderTargetSelector->setTarget(target);

    // Force initial configuration
    reconfigure(SceneFeaturedRenderStageBase::features());
}

ReflectionStages::~ReflectionStages()
{
}

void ReflectionStages::reconfigure(const Features features)
{
    // We force disable frustum culling
    Features editedFeatures = features;
    editedFeatures.setFlag(FrustumCulling, false);

    // Rebuild FG hierarchy based on set features
    SceneStages::reconfigure(editedFeatures);
}

void ReflectionStages::setReflectivePlaneEquation(const QVector4D &planeEquation)
{
    m_reflectivePlaneParameter->setValue(planeEquation);
}

void ReflectionStages::setReflectionTextureSize(const QSize &size)
{
    Qt3DRender::QRenderTarget *oldTarget = m_renderTargetSelector->target();
    Qt3DRender::QRenderTarget *target = FrameGraphUtils::createRenderTarget(FrameGraphUtils::IncludeDepth,
                                                                            m_renderTargetSelector,
                                                                            size);
    m_renderTargetSelector->setTarget(target);
    emit reflectionTextureChanged(reflectionTexture());

    if (oldTarget)
        oldTarget->deleteLater();
}

Qt3DRender::QAbstractTexture *ReflectionStages::reflectionTexture() const
{
    return FrameGraphUtils::findRenderTargetTexture(m_renderTargetSelector->target(),
                                                    Qt3DRender::QRenderTargetOutput::Color0);
}

QT_END_NAMESPACE
