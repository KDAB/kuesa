/*
    shadowmapstages.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include "shadowmapstages_p.h"

#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DCore/private/qnode_p.h>

#include <Kuesa/shadowmap.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qrendertargetselector.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*!
    \internal

    ShadowMapRenderPass represents a pass to render shadowmap for a single light

    This takes care of setting up the render target, setting a camera to render from
    the point of view of the light, and holds the actual ScenePass to perform the
    rendering of skinned and non-skinned meshes
 */
ShadowMapRenderPass::ShadowMapRenderPass(Qt3DRender::QFrameGraphNode *parent)
    : QFrameGraphNode(parent)
    , m_shadowMapRenderPass(new ScenePass(ScenePass::ShadowMap))
    , m_lightCamera(new Qt3DRender::QCamera(this))
    , m_cameraSelector(new Qt3DRender::QCameraSelector)
    , m_depthOutput(new Qt3DRender::QRenderTargetOutput)
{
    setObjectName(QStringLiteral("Shadow Map"));

    auto sceneTargetSelector = new Qt3DRender::QRenderTargetSelector;
    auto renderTarget = new Qt3DRender::QRenderTarget(this);
    m_depthOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
    m_depthOutput->setFace(Qt3DRender::QAbstractTexture::AllFaces);
    renderTarget->addOutput(m_depthOutput);
    sceneTargetSelector->setTarget(renderTarget);

    m_clearBuffers = new Qt3DRender::QClearBuffers;
    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    auto noDrawClear = new Qt3DRender::QNoDraw;

    m_cubeMapMatrixParam = new Qt3DRender::QParameter(QStringLiteral("lightViewProjectionMatrices[0]"), QVariant{});
    addParameter(m_cubeMapMatrixParam);

    m_lightIndexParam = new Qt3DRender::QParameter(QStringLiteral("lightIndex"), QVariant{});
    addParameter(m_lightIndexParam);

    m_cameraSelector->setCamera(m_lightCamera);

    // set up framegraph hierarchy
    sceneTargetSelector->setParent(this);
    m_clearBuffers->setParent(sceneTargetSelector);
    noDrawClear->setParent(m_clearBuffers);
    m_cameraSelector->setParent(sceneTargetSelector);
    m_shadowMapRenderPass->setParent(m_cameraSelector);
}

ShadowMapRenderPass::~ShadowMapRenderPass() = default;

void ShadowMapRenderPass::setFrustumCulling(bool frustumCulling)
{
    m_shadowMapRenderPass->setFrustumCulling(frustumCulling);
}

void ShadowMapRenderPass::setSkinning(bool skinning)
{
    m_shadowMapRenderPass->setSkinning(skinning);
}

void ShadowMapRenderPass::addParameter(Qt3DRender::QParameter *parameter)
{
    m_shadowMapRenderPass->addParameter(parameter);
}

void ShadowMapRenderPass::removeParameter(Qt3DRender::QParameter *parameter)
{
    m_shadowMapRenderPass->removeParameter(parameter);
}

void ShadowMapRenderPass::setShadowMap(ShadowMapPtr shadowMap)
{
    if (m_shadowMap == shadowMap)
        return;

    if (m_shadowMap)
        disconnect(m_shadowMap.data(), &ShadowMap::lightViewProjectionChanged, this, &ShadowMapRenderPass::updateLightViewProjection);
    connect(shadowMap.data(), &ShadowMap::lightViewProjectionChanged, this, &ShadowMapRenderPass::updateLightViewProjection);

    m_shadowMap = shadowMap;
    m_shadowMapRenderPass->setCubeShadowMap(m_shadowMap->usesCubeMap());

    m_depthOutput->setTexture(shadowMap->depthTexture());
    m_depthOutput->setLayer(m_shadowMap->depthTextureLayer());

    m_lightIndexParam->setValue(m_shadowMap->depthTextureLayer());

    // clear the buffer except for cubemaps that aren't the first cubemap
    // the entire cubemap array is bound so clearing will clear all cubemaps in array.
    m_clearBuffers->setEnabled(!m_shadowMap->usesCubeMap() || m_shadowMap->depthTextureLayer() == 0);

    updateLightViewProjection();
}

void ShadowMapRenderPass::updateLightViewProjection()
{
    if (m_shadowMap->usesCubeMap()) {
        m_cubeMapMatrixParam->setValue(m_shadowMap->cubeMapProjectionMatrixList());
    } else {
        const auto shadowMapCamera = m_shadowMap->lightCamera();
        m_lightCamera->setProjectionType(shadowMapCamera->projectionType());
        m_lightCamera->setPosition(shadowMapCamera->position());
        m_lightCamera->setViewCenter(shadowMapCamera->viewCenter());
        m_lightCamera->setUpVector(shadowMapCamera->upVector());
        m_lightCamera->setNearPlane(shadowMapCamera->nearPlane());
        m_lightCamera->setFarPlane(shadowMapCamera->farPlane());
        m_lightCamera->setLeft(shadowMapCamera->left());
        m_lightCamera->setRight(shadowMapCamera->right());
        m_lightCamera->setBottom(shadowMapCamera->bottom());
        m_lightCamera->setTop(shadowMapCamera->top());
        m_lightCamera->setFieldOfView(shadowMapCamera->fieldOfView());
    }
}

/*!
    \internal

    ShadowMapStages wraps all the render passes needed to render shadows for multiple lights

    This keeps track of how many shadowmaps are required and creates render passes for each
    shadowmap. It also manages any layers which are common to all render passes.
 */
ShadowMapStages::ShadowMapStages(Qt3DRender::QFrameGraphNode *parent)
    : SceneFeaturedRenderStageBase(parent)
{
    m_layerFilter = new Qt3DRender::QLayerFilter;
    m_reflectiveEnabledParameter = new Qt3DRender::QParameter(QStringLiteral("kuesa_isReflective"), QVariant(false), this);
    m_reflectivePlaneParameter = new Qt3DRender::QParameter(QStringLiteral("kuesa_reflectionPlane"), QVector4D(), this);
}

ShadowMapStages::~ShadowMapStages()
{
    // Destroy layer filter if parentless
    if (!m_layerFilter->parent())
        delete m_layerFilter;
}

void ShadowMapStages::reconfigure(const Features features)
{
    const bool hasLayers = m_layerFilter->layers().size() > 0;
    const bool useSkinning = bool(features & Skinning);
    const bool useFrustumCulling = bool(features & FrustumCulling);

    // Find stages parent root
    Qt3DRender::QFrameGraphNode *stageRoot = nullptr;
    if (hasLayers) {
        // If we have layers, then we parent the layer filter
        // and add the stages as children of it
        stageRoot = m_layerFilter;
        m_layerFilter->setParent(this);
    } else {
        // Otherwise, we unparent the layer filter and
        // parent the stages to the this
        stageRoot = this;
        m_layerFilter->setParent(Q_NODE_NULLPTR);
    }

    const int oldSize = m_shadowMapPasses.size();
    const auto passesToCreate = m_shadowMaps.size() - oldSize;
    m_shadowMapPasses.resize(m_shadowMaps.size());

    for (int i = 0; i < passesToCreate; ++i) {
        auto shadowMapPass = ShadowMapNodePtr::create();
        m_shadowMapPasses[oldSize + i] = shadowMapPass;
        shadowMapPass->addParameter(m_reflectiveEnabledParameter);
        shadowMapPass->addParameter(m_reflectivePlaneParameter);
    }

    for (int shadowMapNo = 0; shadowMapNo < m_shadowMaps.size(); ++shadowMapNo) {
        const auto &shadowMap = m_shadowMaps[shadowMapNo];
        auto &shadowMapRenderPass = m_shadowMapPasses[shadowMapNo];

        // reparent to new stage root, which might have changed if layers were added/removed
        shadowMapRenderPass->setParent(stageRoot);

        // Set features on stages which will update accordingly
        shadowMapRenderPass->setShadowMap(shadowMap);
        shadowMapRenderPass->setFrustumCulling(useFrustumCulling);
        shadowMapRenderPass->setSkinning(useSkinning);
    }
}

void ShadowMapStages::addLayer(Qt3DRender::QLayer *layer)
{
    m_layerFilter->addLayer(layer);

    // We want to make sure layer is removed from SceneStages if destroyed
    auto d = Qt3DCore::QNodePrivate::get(this);
    QVector<Qt3DRender::QLayer *> fakeLayers;
    d->registerDestructionHelper(layer, &SceneStages::removeLayer, fakeLayers);

    // Reconfigure if we had no layers previously
    if (m_layerFilter->layers().size() == 1)
        reconfigure(SceneFeaturedRenderStageBase::features());
}

void ShadowMapStages::removeLayer(Qt3DRender::QLayer *layer)
{
    m_layerFilter->removeLayer(layer);

    auto d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(layer);

    // Reconfigure if we have no more layers previously
    if (m_layerFilter->layers().size() == 0)
        reconfigure(SceneFeaturedRenderStageBase::features());
}

QVector<Qt3DRender::QLayer *> ShadowMapStages::layers() const
{
    return m_layerFilter->layers();
}

void ShadowMapStages::setShadowMaps(const QVector<ShadowMapPtr> &activeShadowMaps)
{
    if (m_shadowMaps == activeShadowMaps)
        return;

    m_shadowMaps = activeShadowMaps;
    emit shadowMapsChanged(m_shadowMaps);

    reconfigure(SceneFeaturedRenderStageBase::features());
}

QVector<ShadowMapPtr> ShadowMapStages::shadowMaps() const
{
    return m_shadowMaps;
}

QT_END_NAMESPACE
