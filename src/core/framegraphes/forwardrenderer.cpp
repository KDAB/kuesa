/*
    forwardrenderer.cpp

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
#include <QScreen>
#include <QOffscreenSurface>
#include <Kuesa/abstractpostprocessingeffect.h>
#include "zfillrenderstage_p.h"
#include "opaquerenderstage_p.h"
#include "transparentrenderstage_p.h"
#include "tonemappingandgammacorrectioneffect.h"
#include "kuesa_p.h"
#include <cmath>

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*!
 * \class Kuesa::ForwardRenderer
 * \brief Implements a simple forward rendering FrameGraph.
 * \inmodule Kuesa
 * \since Kuesa 1.0
 * \inherits Qt3DRender::QFrameGraphNode
 *
 * This FrameGraph will select all Qt3D Entities that reference a Material with
 * Techniques having a FilterKey with the name "renderingStyle" set to the
 * string value "forward".
 *
 * The rendering will be made on a single viewport whose dimensions can be set
 * with normalized coordinates by calling ForwardRender::setViewportRect.
 *
 * The screen will be cleared with the color set by calling
 * ForwardRender::setClearColor. By default the clear color is black.
 *
 * Unless a call to ForwardRender::setClearBuffers has been made, only the
 * color and depth buffers will be cleared before the rest of the scene is
 * rendered.
 *
 * The scene will be rendered from the camera set with ForwardRender::setCamera
 * on the window or offscreen surface set by calling
 * ForwardRender::setRenderSurface. Additionally
 * ForwardRender::setExternalRenderTargetSize can be called to specify the size
 * of the rendering surface, when different from the size reported by the
 * surface, this can be the case with high DPI screens.
 *
 * Frustum culling of Entities will be performed by default to only render
 * entities that are visible within the camera's frustum. This can be disabled
 * by calling ForwardRender::setFrustumCulling. Please note frustum culling is
 * not performed on skinned meshes.
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
 * by calling ForwardRender::setBackToFrontSorting.
 *
 * In some case, an optimization technique known as an early z-filling pass can
 * provide a significant performance gain. This is especially true for large
 * scenes using complex rendering materials. The idea consist in filling the
 * depth buffer with a low complexity shader so that subsequent passes
 * incorporating more detailed rendering techniques only render fragments for
 * the depth previously recorded. The optional z-fill pass can be enabled by
 * calling ForwardRender::setZFilling. All Entities having a Material with a
 * compatible RenderPass that has a FilterKey KuesaDrawStage with a value set
 * to ZFill will be rendered. This will take place before the Opaque and
 * Transparent pass. Only Opaque objects should be used to fill the depth
 * buffer.
 *
 * The pipeline steps, from PBR materials to post processing effects are
 * working in linear color space. This ForwardRenderer adds a final gamma
 * correction to perform the final conversion to sRGB, through the use of the
 * GammaCorrectionEffect, as the last step of this pipeline.
 */

/*!
 * \qmltype ForwardRenderer
 * \brief Implements a simple forward rendering FrameGraph.
 * \inqmlmodule Kuesa
 * \since Kuesa 1.0
 * \instantiates Kuesa::ForwardRenderer
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
 * by setting the frustumCulling property. Please note frustum culling is not
 * performed on skinned meshes.
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
 *
 * The pipeline steps, from PBR materials to post processing effects are
 * working in linear color space. This ForwardRenderer adds a final gamma
 * correction to perform the final conversion to sRGB, through the use of the
 * GammaCorrectionEffect, as the last step of this pipeline.
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

/*!
    \property Kuesa::ForwardRenderer::gamma

    Holds the gamma value to use for gamma correction conversion
    that brings linear colors to sRGB colors.
    \default 2.2
    \since Kuesa 1.1
 */

/*!
    \qmlproperty real Kuesa::ForwardRenderer::gamma

    Holds the gamma value to use for gamma correction conversion
    that brings linear colors to sRGB colors.
    \default 2.2
    \since Kuesa 1.1
 */

/*!
    \property Kuesa::ForwardRenderer::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
    \since Kuesa 1.1
 */

/*!
    \qmlproperty real Kuesa::ForwardRenderer::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
    \since Kuesa 1.1
 */

/*!
    \property Kuesa::ForwardRenderer::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \since Kuesa 1.1
    \default ToneMappingAndGammaCorrectionEffect.None
 */

/*!
    \qmlproperty ToneMappingAndGammaCorrectionEffect.ToneMapping Kuesa::ForwardRenderer::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \since Kuesa 1.1
    \default ToneMappingAndGammaCorrectionEffect.None
 */



ForwardRenderer::ForwardRenderer(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(parent)
    , m_noFrustumCullingTechniqueFilter(new Qt3DRender::QTechniqueFilter())
    , m_frustumCullingTechniqueFilter(new Qt3DRender::QTechniqueFilter())
    , m_viewport(new Qt3DRender::QViewport())
    , m_cameraSelector(new Qt3DRender::QCameraSelector())
    , m_clearBuffers(new Qt3DRender::QClearBuffers())
    , m_surfaceSelector(new Qt3DRender::QRenderSurfaceSelector())
    , m_noDrawClearBuffer(new Qt3DRender::QNoDraw())
    , m_frustumCulling(new Qt3DRender::QFrustumCulling())
    , m_backToFrontSorting(true)
    , m_zfilling(false)
    , m_renderToTextureRootNode(nullptr)
    , m_effectsRootNode(nullptr)
    , m_gammaCorrectionFX(new ToneMappingAndGammaCorrectionEffect())
{
    m_renderTargets[0] = nullptr;
    m_renderTargets[1] = nullptr;

    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        m_frustumCullingTechniqueFilter->addMatch(filterKey);
        m_noFrustumCullingTechniqueFilter->addMatch(filterKey);
    }
    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("allowCulling"));
        filterKey->setValue(true);
        m_frustumCullingTechniqueFilter->addMatch(filterKey);
    }
    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("allowCulling"));
        filterKey->setValue(false);
        m_noFrustumCullingTechniqueFilter->addMatch(filterKey);
    }

    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    connect(m_cameraSelector, &Qt3DRender::QCameraSelector::cameraChanged, this, &ForwardRenderer::cameraChanged);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged, this, &ForwardRenderer::clearColorChanged);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::buffersChanged, this, &ForwardRenderer::clearBuffersChanged);
    connect(m_viewport, &Qt3DRender::QViewport::normalizedRectChanged, this, &ForwardRenderer::viewportRectChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::externalRenderTargetSizeChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::renderSurfaceChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::handleSurfaceChange);
    connect(m_frustumCulling, &Qt3DRender::QFrustumCulling::enabledChanged, this, &ForwardRenderer::frustumCullingChanged);
    connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::gammaChanged, this, &ForwardRenderer::gammaChanged);
    connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::exposureChanged, this, &ForwardRenderer::exposureChanged);
    connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::toneMappingAlgorithmChanged, this, &ForwardRenderer::toneMappingAlgorithmChanged);

    {
        // Add internal post FX to the pipeline
        m_internalPostProcessingEffects.push_back(m_gammaCorrectionFX);
        m_effectFGSubtrees.insert(m_gammaCorrectionFX, m_gammaCorrectionFX->frameGraphSubTree());
    }

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
    m_sceneStages.clear();
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
    Exposure correction factor used before the linear to sRGB conversion.

    \default 0.0
*/
float ForwardRenderer::exposure() const
{
    return m_gammaCorrectionFX->exposure();
}

/*!
    Gamma correction value used for the linear to sRGB conversion.
    \since Kuesa 1.1
*/
float ForwardRenderer::gamma() const
{
    return m_gammaCorrectionFX->gamma();
}

/*!
    Returns the tone mapping algorithm used by the shader.
    \since Kuesa 1.1
 */
ToneMappingAndGammaCorrectionEffect::ToneMapping ForwardRenderer::toneMappingAlgorithm() const
{
    return m_gammaCorrectionFX->toneMappingAlgorithm();
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
    if (m_userPostProcessingEffects.contains(effect))
        return;

    // Add effect to vector of registered effects
    m_userPostProcessingEffects.push_back(effect);

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
    if (!m_userPostProcessingEffects.contains(effect))
        return;

    // Remove effect entry
    m_userPostProcessingEffects.removeAll(effect);

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
    return m_userPostProcessingEffects;
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
 * Sets the \a camera used to view the scene.
 */
void ForwardRenderer::setCamera(Qt3DCore::QEntity *camera)
{
    m_cameraSelector->setCamera(camera);
}

/*!
 * Sets the \a clearColor used to clear the screen at the start of each frame.
 */
void ForwardRenderer::setClearColor(const QColor &clearColor)
{
    // Convert QColor from sRGB to Linear
    const QColor linearColor = QColor::fromRgbF(powf(clearColor.redF(), 2.2f),
                                                powf(clearColor.greenF(), 2.2f),
                                                powf(clearColor.blueF(), 2.2f),
                                                clearColor.alphaF());
    m_clearBuffers->setClearColor(linearColor);
}

/*!
 * Sets which \a clearBuffers will be cleared at the start of each frame.
 */
void ForwardRenderer::setClearBuffers(Qt3DRender::QClearBuffers::BufferType clearBuffers)
{
    m_clearBuffers->setBuffers(clearBuffers);
}

/*!
 * Activates \a frustumCulling of geometries which lie outside of the view frustum.
 */
void ForwardRenderer::setFrustumCulling(bool frustumCulling)
{
    m_frustumCulling->setEnabled(frustumCulling);
}

/*!
 * Activates or desactivates back-to-front sorting which may be required for
 * correct alpha blending rendering with \a backToFrontSorting. This is true by
 * default.
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
    Activates multi-pass \a zFilling support.

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
    Sets the \a gamma value to use for gamma correction conversion
    that brings linear colors to sRGB colors.
    \default 2.2
 */
void ForwardRenderer::setGamma(float gamma)
{
    m_gammaCorrectionFX->setGamma(gamma);
}

/*!
    Sets the \a exposure value to use for exposure correction conversion
    \default 0
 */
void ForwardRenderer::setExposure(float exposure)
{
    m_gammaCorrectionFX->setExposure(exposure);
}

/*!
    Sets the tone mapping algorithm to \a algorithm,
    \since Kuesa 1.1
*/
void ForwardRenderer::setToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm)
{
    m_gammaCorrectionFX->setToneMappingAlgorithm(toneMappingAlgorithm);
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
    for (auto effect : m_userPostProcessingEffects)
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

    m_surfaceSelector->setParent(this);
    m_viewport->setParent(m_surfaceSelector);
    m_clearBuffers->setParent(m_viewport);
    m_noDrawClearBuffer->setParent(m_clearBuffers);

    // Camera selector hold the root FG node to render the gltf Scene
    // It might get reparented to something else than m_viewport
    // in case rendering into FBO is required
    m_cameraSelector->setParent(m_viewport);

    // Perform Frustum culling only on non skinned meshes
    m_frustumCulling->setParent(m_cameraSelector);
    m_frustumCullingTechniqueFilter->setParent(m_frustumCulling);

    // Skinned Meshes are not checked against frustum culling as the skinning
    // could actually make them still be in the view frustum even if their
    // transform technically makes them out of sight
    m_noFrustumCullingTechniqueFilter->setParent(m_cameraSelector);

    // Root Nodes into which we have to perform the Render Stages to draw the glTF Scene
    m_renderStageRootNodes = { m_frustumCullingTechniqueFilter, m_noFrustumCullingTechniqueFilter };

    // Temporarily reparent effect subtrees and other nodes, then delete the node that held the last
    // subtree framegraph including any render target selectors.
    // It's easier just to re-create the tree below
    for (AbstractPostProcessingEffect *effect : qAsConst(m_userPostProcessingEffects))
        m_effectFGSubtrees.value(effect)->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    for (AbstractPostProcessingEffect *effect : qAsConst(m_internalPostProcessingEffects)) {
        m_effectFGSubtrees.value(effect)->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    }
    for (SceneStages &stage : m_sceneStages)
        stage.setParent(Q_NODE_NULLPTR);
    delete m_effectsRootNode;
    m_effectsRootNode = nullptr;

    // delete the extra nodes to render main scene to texture.  Will recreate if needed
    delete m_renderToTextureRootNode;
    m_renderToTextureRootNode = nullptr;

    delete m_renderTargets[0];
    delete m_renderTargets[1];
    m_renderTargets[0] = nullptr;
    m_renderTargets[1] = nullptr;

    Qt3DRender::QAbstractTexture *depthTex = nullptr;

    // Configure effects
    const bool hasFX = !m_userPostProcessingEffects.empty() || !m_internalPostProcessingEffects.empty();
    if (hasFX) {
        if (!m_renderTargets[0]) {
            // create a render target for main scene
            m_renderTargets[0] = createRenderTarget(true);
        }
        const int userFXCount = m_userPostProcessingEffects.size();
        const int totalFXCount = userFXCount + m_internalPostProcessingEffects.size();
        if (totalFXCount > 1 && !m_renderTargets[1]) {
            m_renderTargets[1] = createRenderTarget(false);
            // create a secondary render target to do ping-pong when we have
            // more than 1 fx
        }

        const auto &target0outputs = m_renderTargets[0]->outputs();
        if (!target0outputs.empty())
            depthTex = target0outputs.back()->texture();

        // Remove cameraSelector subtree
        m_cameraSelector->setParent(Q_NODE_NULLPTR);

        // create a subtree under m_viewport to render the main scene into a texture
        // Effectively taking the place of where the CameraSelector node was in the
        // tree
        m_renderToTextureRootNode = new Qt3DRender::QFrameGraphNode(m_viewport);
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

        // Reparent cameraSelectror to sceneTargetSelector
        m_cameraSelector->setParent(sceneTargetSelector);

        // create a node to hold all the effect subtrees, under the main viewport. They don't need camera, alpha, frustum, etc.
        m_effectsRootNode = new Qt3DRender::QFrameGraphNode(m_viewport);
        m_effectsRootNode->setObjectName(QStringLiteral("KuesaPostProcessingEffects"));

        // Gather the different effect types
        const auto targetSize = currentSurfaceSize();
        int previousRenderTargetIndex = 0;
        for (int effectNo = 0; effectNo < totalFXCount; ++effectNo) {
            AbstractPostProcessingEffect *effect = nullptr;
            if (effectNo < userFXCount)
                effect = m_userPostProcessingEffects[effectNo];
            else
                effect = m_internalPostProcessingEffects[effectNo - userFXCount];

            const int currentRenderTargetIndex = 1 - previousRenderTargetIndex;

            // determine which render target we used for previous effect.  It holds the input texture for current effect
            auto previousRenderTarget = m_renderTargets[previousRenderTargetIndex];
            effect->setInputTexture(findRenderTargetTexture(previousRenderTarget, Qt3DRender::QRenderTargetOutput::Color0));
            effect->setDepthTexture(depthTex);
            effect->setCamera(this->camera());
            effect->setSceneSize(targetSize);

            // add the layers from the effect to block them from being rendered in the main scene
            const auto &layers = effect->layers();
            for (auto layer : layers)
                mainSceneFilter->addLayer(layer);

            // Create a render target selector for all but the last effect to create the input texture for the next effect
            Qt3DCore::QNode *effectParentNode = m_effectsRootNode;
            if (effectNo < totalFXCount - 1) {
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

// Stages are the FrameGraph sub branches required for the rendering
// of the actual gltf scene
void ForwardRenderer::reconfigureStages()
{
//    bool requiresReordering = false;

    if (m_sceneStages.size() != m_renderStageRootNodes.size()) {
        m_sceneStages.resize(m_renderStageRootNodes.size());

        for (SceneStages &sceneStage : m_sceneStages)
            sceneStage.init();
    }

    if (m_sceneStages.empty())
        return;

    // Update SceneStages
    // Takes care of reparenting of stages internally
    for (int i = 0, m = m_sceneStages.size(); i < m; ++i) {
        SceneStages &sceneStage = m_sceneStages[i];
        sceneStage.setZFilling(m_zfilling);
        sceneStage.setBackToFrontSorting(m_backToFrontSorting);
        sceneStage.setParent(m_renderStageRootNodes.at(i));
    }
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
    // We need to use 16 based format as our content is HDR linear
    // which we will eventually exposure correct, tone map to LDR and
    // gamma correct
    // This requires support for extension OES_texture_float on ES2 platforms
    colorTexture->setFormat(Qt3DRender::QAbstractTexture::RGBA16F);
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
        size = window->size() * window->screen()->devicePixelRatio();
    else if (qobject_cast<QOffscreenSurface *>(surface))
        size = m_surfaceSelector->externalRenderTargetSize();
    else
        qCWarning(kuesa) << Q_FUNC_INFO << "Unexpected surface type for surface " << surface;

    return size;
}

/*!
 * Sets the size of the external render target to \a externalRenderTargetSize.
 */
void ForwardRenderer::setExternalRenderTargetSize(const QSize &externalRenderTargetSize)
{
    m_surfaceSelector->setExternalRenderTargetSize(externalRenderTargetSize);
}

/*!
 * Sets the surface where rendering will occur to \a renderSurface. This can be
 * an offscreen surface or a window surface.
 */
void ForwardRenderer::setRenderSurface(QObject *renderSurface)
{
    m_surfaceSelector->setSurface(renderSurface);
}

/*!
 * Sets the dimensions of the viewport where rendering will occur to \a
 * viewportRect. Dimensions are in normalized coordinates.
 */
void ForwardRenderer::setViewportRect(const QRectF &viewportRect)
{
    m_viewport->setNormalizedRect(viewportRect);
}

/*!
    \internal
 */
ForwardRenderer::SceneStages::SceneStages()
    : m_opaqueStage(nullptr)
    , m_transparentStage(nullptr)
    , m_zFillStage(nullptr)
{
}

ForwardRenderer::SceneStages::~SceneStages()
{
    delete m_opaqueStage;
    delete m_transparentStage;
    delete m_zFillStage;
}

/*!
    \internal
 */
void ForwardRenderer::SceneStages::init()
{
    m_opaqueStage = new OpaqueRenderStage();
    m_transparentStage = new TransparentRenderStage();

    QObject::connect(m_opaqueStage, &Qt3DCore::QNode::nodeDestroyed,
                     [this] { m_opaqueStage = nullptr; });
    QObject::connect(m_transparentStage, &Qt3DCore::QNode::nodeDestroyed,
                     [this] { m_transparentStage = nullptr; });
}

/*!
    \internal
 */
void ForwardRenderer::SceneStages::setZFilling(bool zFilling)
{
    const bool hasZillStage = m_zFillStage != nullptr;
    if (hasZillStage != zFilling) {
        if (hasZillStage) {
            QObject::disconnect(m_zFillDestroyedConnection);
            delete m_zFillStage;
            m_zFillStage = nullptr;
        } else {
            m_zFillStage = new ZFillRenderStage();
            m_zFillDestroyedConnection = QObject::connect(m_zFillStage, &Qt3DCore::QNode::nodeDestroyed,
                                                          [this] { m_zFillStage = nullptr; });
        }
        m_opaqueStage->setZFilling(zFilling);
        QNode *oldParent = parent();
        // Unparent
        setParent(Q_NODE_NULLPTR);
        // Reparent (this forces proper Render Stage order if one stage was
        // removed or added)
        setParent(oldParent);
    }
}

/*!
    \internal
 */
void ForwardRenderer::SceneStages::setBackToFrontSorting(bool backToFrontSorting)
{
    if (backToFrontSorting != m_transparentStage->backToFrontSorting())
        m_transparentStage->setBackToFrontSorting(backToFrontSorting);
}

/*!
    \internal
 */
bool ForwardRenderer::SceneStages::zFilling()
{
    return m_zFillStage != nullptr;
}

/*!
    \internal
 */
bool ForwardRenderer::SceneStages::backToFrontSorting()
{
    return m_transparentStage->backToFrontSorting();
}

/*!
    \internal
 */
void ForwardRenderer::SceneStages::setParent(Qt3DCore::QNode *parentNode)
{
    if (parentNode == parent())
        return;

    if (zFilling())
        m_zFillStage->setParent(parentNode);
    m_opaqueStage->setParent(parentNode);
    m_transparentStage->setParent(parentNode);
}

/*!
    \internal
 */
Qt3DCore::QNode *ForwardRenderer::SceneStages::parent() const
{
    return m_opaqueStage->parentNode();
}

QT_END_NAMESPACE
