/*
    forwardrenderer.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "forwardrenderer.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <QWindow>
#include <QOffscreenSurface>
#include <Kuesa/abstractpostprocessingeffect.h>
#include "zfillrenderstage_p.h"
#include "opaquerenderstage_p.h"
#include "transparentrenderstage_p.h"
#include "kuesa_p.h"

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*!
 * \class ForwardRender
 * \brief Implements a simple forward rendering FrameGraph.
 * \inmodule Kuesa
 * \since 1.0
 * \inherits Qt3DRender::QFrameGraphNode
 *
 * This FrameGraph will select all Qt3D Entities that reference a Material with
 * Techniques having a FilterKey with the name "renderingStyle" set to the
 * string value "forward".
 *
 * The rendering will be made on a single viewport whose dimensions can be set
 * with normalized coordinates by calling \fn setViewportRect.
 *
 * The screen will be cleared with the color set by calling \fn setClearColor.
 * By default the clear color is black.
 *
 * Unless a call to \fn setClearBuffers has been made, only the color and depth
 * buffers will be cleared before the rest of the scene is rendered.
 *
 * The scene will be rendered from the camera set with \fn setCamera on the
 * window or offscreen surface set by calling \fn setRenderSurface.
 * Additionally \fn setExternalRenderTargetSize can be called to specify the
 * size of the rendering surface, when different from the size reported by the
 * surface, this can be the case with high DPI screens.
 *
 * Frustum culling of Entities will be performed by default to only render
 * entities that are visible within the camera's frustum. This can be disabled
 * by calling \fn setFrustumCulling.
 *
 * By default the rendering is performed so that:
 *
 * Opaque object are rendered first: Entities having a Material with a
 * compatible RenderPass that has a FilterKey KuesaDrawStage with a value set
 * to Opaque will be rendered.
 *
 * Transparent objects are rendered second: select all Entities having a
 * Material with a compatible RenderPass that has a FilterKey KuesaDrawStage
 * with a value set to Transparent will be rendered.
 *
 * Scenes containing materials with transparency need to be drawn from
 * back-to-front in relation to the Camera for the blending to work correctly.
 * This sorting can be expensive and is disabled by default. It can be enabled
 * by calling \fn setBackToFrontSorting.
 *
 * In some case, an optimization technique known as an early z-filling pass can
 * provide a significant performance gain. This is especially true for large
 * scenes using complex rendering materials. The idea consist in filling the
 * depth buffer with a low complexity shader so that subsequent passes
 * incorporating more detailed rendering techniques only render fragments for
 * the depth previously recorded. The optional z-fill pass can be enabled by
 * calling \fn setZFilling. All Entities having a Material with a compatible
 * RenderPass that has a FilterKey KuesaDrawStage with a value set to ZFill
 * will be rendered. This will take place before the Opaque and Transparent
 * pass. Only Opaque objects should be used to fill the depth buffer.
 */

/*!
 * \qmltype ForwardRender
 * \brief Implements a simple forward rendering FrameGraph.
 * \inqmlmodule Kuesa
 * \since 1.0
 * \instantiates Kuesa::ForwardRender
 *
 * This FrameGraph will select all Qt3D Entities that reference a Material with
 * Techniques having a FilterKey with the name "renderingStyle" set to the
 * string value "forward".
 *
 * The rendering will be made on a single viewport whose dimensions can be set
 * with normalized coordinates by setting the viewportRect property.
 *
 * The screen will be cleared with the color set with the clearColor property.
 * By default the clear color is black.
 *
 * Unless clearBuffers has been set appropriately, only the color and depth
 * buffers will be cleared before the rest of the scene is rendered.
 *
 * The scene will be rendered from the camera property on the
 * window or offscreen surface specified by the renderSurface property.
 * Additionally, setting externalRenderTargetSize will specify the
 * size of the rendering surface, when different from the size reported by the
 * surface, this can be the case with high DPI screens.
 *
 * Frustum culling of Entities will be performed by default to only render
 * entities that are visible within the camera's frustum. This can be disabled
 * by setting the frustumCulling property.
 *
 * By default the rendering is performed so that:
 *
 * Opaque object are rendered first: Entities having a Material with a
 * compatible RenderPass that has a FilterKey KuesaDrawStage with a value set
 * to Opaque will be rendered.
 *
 * Transparent objects are rendered second: select all Entities having a
 * Material with a compatible RenderPass that has a FilterKey KuesaDrawStage
 * with a value set to Transparent will be rendered.
 *
 * Scenes containing materials with transparency need to be drawn from
 * back-to-front in relation to the Camera for the blending to work correctly.
 * This sorting can be expensive and is disabled by default. It can be enabled
 * by setting the backToFrontSorting to true.
 *
 * In some case, an optimization technique known as an early z-filling pass can
 * provide a significant performance gain. This is especially true for large
 * scenes using complex rendering materials. The idea consist in filling the
 * depth buffer with a low complexity shader so that subsequent passes
 * incorporating more detailed rendering techniques only render fragments for
 * the depth previously recorded. The optional z-fill pass can be enabled by
 * setting the zFilling property to true. All Entities having a Material with
 * a compatible RenderPass that has a FilterKey KuesaDrawStage with a value set
 * to ZFill will be rendered. This will take place before the Opaque and Transparent
 * pass. Only Opaque objects should be used to fill the depth buffer.
 */

/*!
    \property Kuesa::ForwardRenderer::renderSurface

    Holds the surface where rendering will occur.
*/

/*!
    \qmlproperty Surface Kuesa::ForwardRenderer::renderSurface

    Holds the surface where rendering will occur.
*/

/*!
    \property Kuesa::ForwardRenderer::externalRenderTargetSize

    Holds the size of the external render target.
*/

/*!
    \qmlproperty size Kuesa::ForwardRenderer::externalRenderTargetSize

    Holds the size of the external render target.
*/

/*!
    \property Kuesa::ForwardRenderer::viewportRect

    Holds the rectangle within the current render surface where
    rendering will occur. Rectangle is in normalized coordinates.
*/

/*!
    \qmlproperty size Kuesa::ForwardRenderer::viewportRect

    Holds the rectangle within the current render surface where
    rendering will occur. Rectangle is in normalized coordinates.
*/

/*!
    \property Kuesa::ForwardRenderer::camera

    Holds the camera used to view the scene.
*/

/*!
    \qmlproperty Entity Kuesa::ForwardRenderer::camera

    Holds the camera used to view the scene.
*/

/*!
    \property Kuesa::ForwardRenderer::clearColor

    Holds the color used to clear the screen at the start of each frame.
*/

/*!
    \qmlproperty color Kuesa::ForwardRenderer::clearColor

    Holds the color used to clear the screen at the start of each frame.
*/

/*!
    \property Kuesa::ForwardRenderer::clearBuffers

    Holds which buffers will be cleared at the start of each frame.
*/

/*!
    \qmlproperty enumeration Kuesa::ForwardRenderer::clearBuffers

    Holds which buffers will be cleared at the start of each frame.
*/

/*!
    \property Kuesa::ForwardRenderer::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/

/*!
    \qmlproperty bool Kuesa::ForwardRenderer::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/

/*!
    \property Kuesa::ForwardRenderer::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::ForwardRenderer::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Disabled by default.
*/

/*!
    \property Kuesa::ForwardRenderer::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::ForwardRenderer::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/

/*!
    \qmlproperty list<AbstractPostProcessingEffect> Kuesa::ForwardRenderer::postProcessingEffects

    Holds the list of post processing effects.

    In essence this will complete the FrameGraph tree with a
    dedicated subtree provided by the effect.

    Lifetime of the subtree will be entirely managed by the ForwardRenderer.

    Be aware that registering several effects of the same type might have
    unexpected behavior. It is advised against unless explicitly documented in
    the effect.

    The FrameGraph tree is reconfigured upon replacing the list of effects.
*/

ForwardRenderer::ForwardRenderer(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(parent)
    , m_techniqueFilter(new Qt3DRender::QTechniqueFilter())
    , m_viewport(new Qt3DRender::QViewport())
    , m_cameraSelector(new Qt3DRender::QCameraSelector())
    , m_clearBuffers(new Qt3DRender::QClearBuffers())
    , m_surfaceSelector(new Qt3DRender::QRenderSurfaceSelector())
    , m_noDrawClearBuffer(new Qt3DRender::QNoDraw())
    , m_frustumCulling(new Qt3DRender::QFrustumCulling())
    , m_backToFrontSorting(false)
    , m_zfilling(false)
    , m_renderToTextureRootNode(nullptr)
    , m_effectsRootNode(nullptr)
    , m_renderStageRootNode(nullptr)
{
    m_renderTargets[0] = nullptr;
    m_renderTargets[1] = nullptr;

    auto filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));
    m_techniqueFilter->addMatch(filterKey);

    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    connect(m_cameraSelector, &Qt3DRender::QCameraSelector::cameraChanged, this, &ForwardRenderer::cameraChanged);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged, this, &ForwardRenderer::clearColorChanged);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::buffersChanged, this, &ForwardRenderer::clearBuffersChanged);
    connect(m_viewport, &Qt3DRender::QViewport::normalizedRectChanged, this, &ForwardRenderer::viewportRectChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::externalRenderTargetSizeChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::renderSurfaceChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::handleSurfaceChange);
    connect(m_frustumCulling, &Qt3DRender::QFrustumCulling::enabledChanged, this, &ForwardRenderer::frustumCullingChanged);

    // Reconfigure FrameGraph
    reconfigureFrameGraph();
    // Reconfigure Stages
    reconfigureStages();
}

ForwardRenderer::~ForwardRenderer()
{
    // unparent the effect subtrees or they'll be deleted twice
    for (auto &framegraph : qAsConst(m_effectFGSubtrees))
        framegraph->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    m_effectFGSubtrees.clear();
    qDeleteAll(m_renderStages);
    m_renderStages.clear();
}

/*!
 * Returns the camera used to render the scene.
 */
Qt3DCore::QEntity *ForwardRenderer::camera() const
{
    return m_cameraSelector->camera();
}

/*!
 * Returns the color used to clear the screen at the start of each frame.
 */
QColor ForwardRenderer::clearColor() const
{
    return m_clearBuffers->clearColor();
}

/*!
 * Returns which buffers are cleared at the start of each frame.
 */
Qt3DRender::QClearBuffers::BufferType ForwardRenderer::clearBuffers() const
{
    return m_clearBuffers->buffers();
}

/*!
 * Returns whether frustum culling is enabled or not.
 */
bool ForwardRenderer::frustumCulling() const
{
    return m_frustumCulling->isEnabled();
}

/*!
 * Returns whether alpha blending support is enabled or not.
 */
bool ForwardRenderer::backToFrontSorting() const
{
    return m_backToFrontSorting;
}

/*!
 * Returns whether zfill passes are enabled or not.
 */
bool ForwardRenderer::zFilling() const
{
    return m_zfilling;
}

/*!
 * Registers a new post processing effect \a effect with the ForwardRenderer
 * FrameGraph. In essence this will complete the FrameGraph tree with a
 * dedicated subtree provided by the effect.
 *
 * Lifetime of the subtree will be entirely managed by the ForwardRenderer.
 *
 * Be aware that registering several effects of the same type might have
 * unexpected behavior. It is advised against unless explicitly documented in
 * the effect.
 *
 * The FrameGraph tree is reconfigured upon insertion of a new effect.
 */
void ForwardRenderer::addPostProcessingEffect(AbstractPostProcessingEffect *effect)
{
    if (m_postProcessingEffects.contains(effect))
        return;

    // Add effect to vector of registered effects
    m_postProcessingEffects.push_back(effect);

    // Handle destruction of effect
    QObject::connect(effect,
                     &Qt3DCore::QNode::nodeDestroyed,
                     this,
                     [this, effect]() { removePostProcessingEffect(effect); });

    // Add FrameGraph subtree to dedicated subtree of effects
    auto effectFGSubtree = effect->frameGraphSubTree();
    if (!effectFGSubtree.isNull()) {
        // Register FrameGraph Subtree
        m_effectFGSubtrees.insert(effect, effectFGSubtree);

        // Reconfigure FrameGraph Tree
        reconfigureFrameGraph();
        reconfigureStages();
    }
}

/*!
 * Unregisters \a effect from the current ForwardRenderer's FrameGraph. This
 * will destroy the FrameGraph subtree associated with the effect.
 *
 * The FrameGraph tree is reconfigured upon removal of an effect.
 */
void ForwardRenderer::removePostProcessingEffect(AbstractPostProcessingEffect *effect)
{
    if (!m_postProcessingEffects.contains(effect))
        return;

    // Remove effect entry
    m_postProcessingEffects.removeAll(effect);

    // unparent FG subtree associated with Effect.
    m_effectFGSubtrees.take(effect)->setParent(static_cast<Qt3DCore::QNode *>(nullptr));

    // Reconfigure FrameGraph Tree
    reconfigureFrameGraph();
    reconfigureStages();
}

/*!
 * Returns all registered effects
 */
QVector<AbstractPostProcessingEffect *> ForwardRenderer::postProcessingEffects() const
{
    return m_postProcessingEffects;
}

/*!
 * \internal
 *
 * Returns the root pointer of the FrameGraph subtree registered by the \a
 * effect. nullptr will be returned if no subtree has been registered or if the
 * effect is invalid.
 */
AbstractPostProcessingEffect::FrameGraphNodePtr ForwardRenderer::frameGraphSubtreeForPostProcessingEffect(AbstractPostProcessingEffect *effect) const
{
    return m_effectFGSubtrees.value(effect, nullptr);
}

/*!
 * Returns the external render target size.
 */
QSize ForwardRenderer::externalRenderTargetSize() const
{
    return m_surfaceSelector->externalRenderTargetSize();
}

/*!
 * Returns the surface where rendering will occur.
 */
QObject *ForwardRenderer::renderSurface() const
{
    return m_surfaceSelector->surface();
}

/*!
 * Returns the viewport rectangle in normalized coordinates.
 */
QRectF ForwardRenderer::viewportRect() const
{
    return m_viewport->normalizedRect();
}

/*!
 * Sets the camera used to view the scene.
 */
void ForwardRenderer::setCamera(Qt3DCore::QEntity *camera)
{
    m_cameraSelector->setCamera(camera);
}

/*!
 * Sets the color used to clear the screen at the start of each frame.
 */
void ForwardRenderer::setClearColor(const QColor &clearColor)
{
    m_clearBuffers->setClearColor(clearColor);
}

/*!
 * Sets which buffers will be cleared at the start of each frame.
 */
void ForwardRenderer::setClearBuffers(Qt3DRender::QClearBuffers::BufferType clearBuffers)
{
    m_clearBuffers->setBuffers(clearBuffers);
}

/*!
 * Activates culling of geometries which lie outside of the view frustum.
 */
void ForwardRenderer::setFrustumCulling(bool frustumCulling)
{
    m_frustumCulling->setEnabled(frustumCulling);
}

/*!
 * Activates back-to-front sorting which may be required for correct alpha
 * blending rendering.
 */
void ForwardRenderer::setBackToFrontSorting(bool backToFrontSorting)
{
    if (m_backToFrontSorting != backToFrontSorting) {
        m_backToFrontSorting = backToFrontSorting;
        Q_EMIT backToFrontSortingChanged(m_backToFrontSorting);
        reconfigureStages();
    }
}

/*!
    Activates multi-pass zFilling support.

    If activated, opaque objects in the scene will be rendered first with a
    simple fragment shader to fill the depth buffer. Then opaque objects will
    be rendered again with the normal shader and finally transparent objects
    will be rendered (back to front if back to front sorting is enabled).
*/
void ForwardRenderer::setZFilling(bool zfilling)
{
    if (m_zfilling != zfilling) {
        m_zfilling = zfilling;
        Q_EMIT zFillingChanged(zfilling);
        reconfigureStages();
    }
}

/*!
 * \internal
 *
 * Updates all the off-screen rendering textures whenever the render surface
 * size changes
 */
void ForwardRenderer::updateTextureSizes()
{
    const QSize targetSize = currentSurfaceSize();
    QVector<Qt3DRender::QRenderTargetOutput *> outputs;
    for (auto target : m_renderTargets) {
        if (target != nullptr)
            outputs += target->outputs();
    }
    for (auto output : outputs)
        output->texture()->setSize(targetSize.width(), targetSize.height());
    for (auto effect : m_postProcessingEffects)
        effect->setSceneSize(targetSize);
}

/*!
 * \internal
 *
 * Connects to the appropriate signal for the surface type to know when
 * know when the surface size has changed so offscreen buffers can be resized.
 */
void ForwardRenderer::handleSurfaceChange()
{
    auto surface = m_surfaceSelector->surface();
    Q_ASSERT(surface);

    // disconnect any existing connections
    for (auto connection : qAsConst(m_resizeConnections))
        disconnect(connection);
    m_resizeConnections.clear();

    // surface should only be a QWindow or QOffscreenSurface. Have to downcast since QSurface isn't QObject
    if (auto window = qobject_cast<QWindow *>(surface)) {
        m_resizeConnections.push_back(connect(window, &QWindow::widthChanged, this, &ForwardRenderer::updateTextureSizes));
        m_resizeConnections.push_back(connect(window, &QWindow::heightChanged, this, &ForwardRenderer::updateTextureSizes));
    } else if (qobject_cast<QOffscreenSurface *>(surface)) {
        m_resizeConnections.push_back(connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::updateTextureSizes));
    } else {
        qCWarning(kuesa) << Q_FUNC_INFO << "Unexpected surface type for surface " << surface;
    }
    updateTextureSizes();
}

/*!
 * \internal
 *
 * Rebuild FrameGraph tree based on selected effects in the correct order.
 */
void ForwardRenderer::reconfigureFrameGraph()
{
    // Based on the effect types, reorder elements from the FrameGraph in the correct order
    // e.g We want Bloom to happen after DoF...
    //     We may need to render the scene into a texture first ...

    m_techniqueFilter->setParent(this);
    m_surfaceSelector->setParent(m_techniqueFilter);
    m_viewport->setParent(m_surfaceSelector);
    m_clearBuffers->setParent(m_viewport);
    m_noDrawClearBuffer->setParent(m_clearBuffers);
    m_cameraSelector->setParent(m_viewport);
    m_frustumCulling->setParent(m_cameraSelector);

    // Temporarily reparent effect subtrees and other nodes, then delete the node that held the last
    // subtree framegraph including any render target selectors.
    // It's easier just to re-create the tree below
    for (AbstractPostProcessingEffect *effect : qAsConst(m_postProcessingEffects))
        m_effectFGSubtrees.value(effect)->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    for (AbstractRenderStage *stage : qAsConst(m_renderStages))
        stage->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    delete m_effectsRootNode;
    m_effectsRootNode = nullptr;

    // delete the extra nodes to render main scene to texture.  Will recreate if needed
    delete m_renderToTextureRootNode;
    m_renderToTextureRootNode = nullptr;

    // renderStageRoot if we have no FX is the frustumCullingNode
    m_renderStageRootNode = m_frustumCulling;

    delete m_renderTargets[0];
    delete m_renderTargets[1];
    m_renderTargets[0] = nullptr;
    m_renderTargets[1] = nullptr;

    // Configure effects
    if (!m_postProcessingEffects.empty()) {
        if (!m_renderTargets[0]) {
            // create a render target for main scene
            m_renderTargets[0] = createRenderTarget(true);
        }
        if (m_postProcessingEffects.size() > 1 && !m_renderTargets[1]) {
            m_renderTargets[1] = createRenderTarget(false);
            // create a secondary render target to do ping-pong when we have
            // more than 1 fx
        }

        // create a subtree under m_frustumCulling to render the main scene into a texture
        m_renderToTextureRootNode = new Qt3DRender::QFrameGraphNode(m_frustumCulling);
        m_renderToTextureRootNode->setObjectName(QStringLiteral("KuesaMainScene"));

        // need to exclude the effects layers from being drawn in the main scene
        auto mainSceneFilter = new Qt3DRender::QLayerFilter(m_renderToTextureRootNode);
        mainSceneFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);
        auto sceneTargetSelector = new Qt3DRender::QRenderTargetSelector(mainSceneFilter);
        sceneTargetSelector->setTarget(m_renderTargets[0]);

        auto clearScreen = new Qt3DRender::QClearBuffers(sceneTargetSelector);
        clearScreen->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
        clearScreen->setClearColor(m_clearBuffers->clearColor());
        connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged,
                clearScreen, &Qt3DRender::QClearBuffers::setClearColor);
        new Qt3DRender::QNoDraw(clearScreen);

        // If we have FX, renderStageRoot is the sceneTargetSelector
        m_renderStageRootNode = sceneTargetSelector;

        // create a node to hold all the effect subtrees, under the main viewport. They don't need camera, alpha, frustum, etc.
        m_effectsRootNode = new Qt3DRender::QFrameGraphNode(m_viewport);
        m_effectsRootNode->setObjectName(QStringLiteral("KuesaPostProcessingEffects"));

        // Gather the different effect types
        const auto targetSize = currentSurfaceSize();
        int previousRenderTargetIndex = 0;
        for (int effectNo = 0; effectNo < m_postProcessingEffects.count(); ++effectNo) {
            auto effect = m_postProcessingEffects[effectNo];
            const int currentRenderTargetIndex = 1 - previousRenderTargetIndex;

            // determine which render target we used for previous effect.  It holds the input texture for current effect
            auto previousRenderTarget = m_renderTargets[previousRenderTargetIndex];
            effect->setInputTexture(findRenderTargetTexture(previousRenderTarget, Qt3DRender::QRenderTargetOutput::Color0));
            effect->setSceneSize(targetSize);

            // add the layers from the effect to block them from being rendered in the main scene
            const auto &layers = effect->layers();
            for (auto layer : layers)
                mainSceneFilter->addLayer(layer);

            // Create a render target selector for all but the last effect to create the input texture for the next effect
            Qt3DCore::QNode *effectParentNode = m_effectsRootNode;
            if (effectNo < m_postProcessingEffects.count() - 1) {
                auto selector = new Qt3DRender::QRenderTargetSelector(effectParentNode);
                selector->setObjectName(QStringLiteral("Effect %1").arg(effectNo));
                selector->setTarget(m_renderTargets[currentRenderTargetIndex]);
                effectParentNode = selector;
            }

            // add the effect subtree to our framegraph
            m_effectFGSubtrees.value(effect)->setParent(effectParentNode);

            previousRenderTargetIndex = currentRenderTargetIndex;
        }
    }

    const bool blocked = blockNotifications(true);
    emit frameGraphTreeReconfigured();
    blockNotifications(blocked);
}

void ForwardRenderer::reconfigureStages()
{
    bool requiresReordering = false;

    // First time we are created
    if (m_renderStages.empty()) {
        auto opaqueStage = new OpaqueRenderStage();
        auto transparentStage = new TransparentRenderStage();

        opaqueStage->setZFilling(m_zfilling);
        transparentStage->setBackToFrontSorting(m_backToFrontSorting);

        if (m_zfilling) {
            auto zFillingState = new ZFillRenderStage();
            m_renderStages.push_back(zFillingState);
        }
        m_renderStages.push_back(opaqueStage);
        m_renderStages.push_back(transparentStage);
    }

    // Handle change of FG (in case we had no FX before and FX were added)
    // Or in case we have never inserted our stages before
    {
        requiresReordering |= (m_renderStages.last()->parent() != m_renderStageRootNode);
    }

    // Handle ZFilling change
    {
        const bool hasZFillingStage = m_renderStages.size() == 3;
        // We have zFilling and hasZFilling stages differ
        // either we have zFilling and it should now be disabled
        // or we don't have zFilling and it should be enabled
        if (m_zfilling != hasZFillingStage) {
            if (m_zfilling) { // We need to enable zFilling
                auto zFillingState = new ZFillRenderStage();
                m_renderStages.prepend(zFillingState);
            } else { // We need to disable zFilling
                delete m_renderStages.takeFirst();
            }

            auto opaqueStage = static_cast<OpaqueRenderStage *>(m_renderStages.at(m_renderStages.size() - 2));
            opaqueStage->setZFilling(m_zfilling);

            requiresReordering = true;
        }
    }

    // Handle BackToFront sorting change
    {
        // This only affects the TransparentRenderStage
        TransparentRenderStage *transparentRenderStage = static_cast<TransparentRenderStage *>(m_renderStages.last());
        const bool alphaBlendingIsEnabled = transparentRenderStage->backToFrontSorting();

        if (m_backToFrontSorting != alphaBlendingIsEnabled)
            transparentRenderStage->setBackToFrontSorting(m_backToFrontSorting);
    }

    // Reorder/Insert node into FG
    if (requiresReordering) {
        // Remove stage from FG tree
        for (AbstractRenderStage *stage : qAsConst(m_renderStages))
            stage->setParent(Q_NODE_NULLPTR);
        // Add stages into FG tree
        for (AbstractRenderStage *stage : qAsConst(m_renderStages))
            stage->setParent(m_renderStageRootNode);
    };
}

/*!
 * \internal
 *
 * Helper function to create a QRenderTarget with the correct texture formats
 * and sizes.
 */
Qt3DRender::QRenderTarget *ForwardRenderer::createRenderTarget(bool includeDepth)
{
    auto renderTarget = new Qt3DRender::QRenderTarget(this);
    auto colorTexture = new Qt3DRender::QTexture2D;
    colorTexture->setFormat(Qt3DRender::QAbstractTexture::RGBA8_UNorm);
    colorTexture->setGenerateMipMaps(false);

    const auto targetSize = currentSurfaceSize();
    colorTexture->setSize(targetSize.width(), targetSize.height());
    auto colorOutput = new Qt3DRender::QRenderTargetOutput;

    colorOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    colorOutput->setTexture(colorTexture);
    renderTarget->addOutput(colorOutput);

    if (includeDepth) {
        auto depthTexture = new Qt3DRender::QTexture2D;
        depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
        depthTexture->setSize(targetSize.width(), targetSize.height());
        depthTexture->setGenerateMipMaps(false);
        auto depthOutput = new Qt3DRender::QRenderTargetOutput;
        depthOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
        depthOutput->setTexture(depthTexture);
        renderTarget->addOutput(depthOutput);
    }

    return renderTarget;
}

/*!
 * \internal
 *
 * Helper function to retrieve the texture for the specified attachment point
 * in a QRenderTarget
 */
Qt3DRender::QAbstractTexture *ForwardRenderer::findRenderTargetTexture(Qt3DRender::QRenderTarget *target,
                                                                       Qt3DRender::QRenderTargetOutput::AttachmentPoint attachmentPoint)
{
    auto outputs = target->outputs();
    auto attachment = std::find_if(outputs.begin(), outputs.end(), [attachmentPoint](Qt3DRender::QRenderTargetOutput *output) {
        return output->attachmentPoint() == attachmentPoint;
    });
    return attachment == outputs.end() ? nullptr : (*attachment)->texture();
}

QVector<AbstractRenderStage *> ForwardRenderer::renderStages() const
{
    return m_renderStages;
}

/*!
 * \internal
 *
 * Helper functionto return the current render surface size
 */
QSize ForwardRenderer::currentSurfaceSize() const
{
    QSize size;
    auto surface = m_surfaceSelector->surface();

    if (auto window = qobject_cast<QWindow *>(surface))
        size = window->size();
    else if (qobject_cast<QOffscreenSurface *>(surface))
        size = m_surfaceSelector->externalRenderTargetSize();
    else
        qCWarning(kuesa) << Q_FUNC_INFO << "Unexpected surface type for surface " << surface;

    return size;
}

/*!
 * Sets the size of the external render target.
 */
void ForwardRenderer::setExternalRenderTargetSize(const QSize &externalRenderTargetSize)
{
    m_surfaceSelector->setExternalRenderTargetSize(externalRenderTargetSize);
}

/*!
 * Sets the surface where rendering will occur. This can be an offscreen surface or
 * a window surface.
 */
void ForwardRenderer::setRenderSurface(QObject *renderSurface)
{
    m_surfaceSelector->setSurface(renderSurface);
}

/*!
 * Sets the dimensions of the viewport where rendering will occur.  Dimensions are in
 * normalized coordinates.
 */
void ForwardRenderer::setViewportRect(const QRectF &viewportRect)
{
    m_viewport->setNormalizedRect(viewportRect);
}

QT_END_NAMESPACE
