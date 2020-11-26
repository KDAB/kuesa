/*
    forwardrenderer.cpp

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
#include <Qt3DRender/qblitframebuffer.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qparameter.h>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <Qt3DRender/qdebugoverlay.h>
#include <private/qrhi_p.h>
#endif
#include <QWindow>
#include <QScreen>

#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/particles.h>
#include "particlerenderstage_p.h"
#include "tonemappingandgammacorrectioneffect.h"
#include "fboresolver_p.h"
#include "kuesa_p.h"
#include <cmath>

#include <Qt3DCore/private/qnode_p.h>
#include <private/scenestages_p.h>
#include <private/reflectionstages_p.h>
#include <private/effectsstages_p.h>
#include <private/framegraphutils_p.h>
#include <Qt3DRender/private/qframegraphnode_p.h>

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
 * The rendering will be made on a main viewport whose dimensions can be set
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
 * The pipeline steps, from materials to post processing effects are working in
 * linear color space. This ForwardRenderer adds a final gamma correction to
 * perform the final conversion to sRGB, through the use of the
 * GammaCorrectionEffect, as the last step of this pipeline.
 *
 * \badcode
 * #include <Qt3DExtras/Qt3DWindow>
 * #include <Qt3DRender/QCamera>
 * #include <ForwardRenderer>
 * #include <SceneEntity>
 * #include <BloomEffect>
 *
 * Qt3DExtras::Qt3DWindow win;
 * Kuesa::SceneEntity *root = new Kuesa::SceneEntity();
 * Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();
 * Kuesa::BloomEffect *bloomEffect = new Kuesa::BloomEffect();
 * Qt3DRender::QCamera *camera = new Qt3DRender::QCamera();
 *
 * frameGraph->setCamera(camera);
 * frameGraph->setFrustumCulling(true);
 * frameGraph->setClearColor(QColor(Qt::red));
 * frameGraph->addPostProcessingEffect(bloomEffect);
 *
 * win->setRootEntity(root);
 * win->setActiveFrameGraph(forwardRenderer);
 *
 * ...
 * \endcode
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
 * The pipeline steps, from materials to post processing effects are working in
 * linear color space. This ForwardRenderer adds a final gamma correction to
 * perform the final conversion to sRGB, through the use of the
 * GammaCorrectionEffect, as the last step of this pipeline.
 *
 * \badcode
 * import Kuesa 1.1 as Kuesa
 * import Qt3D.Core 2.12
 * import Qt3D.Render 2.12
 *
 * Kuesa.SceneEnity {
 *     id: root
 *
 *     Camera { id: sceneCamera }
 *
 *     components: [
 *         RenderSettings {
 *              Kuesa.ForwardRenderer {
 *                  clearColor: "red"
 *                  camera: sceneCamera
 *                  frustumCulling: true
 *                  backToFrontSorting: true
 *                  postProcessingEffects: [
 *                      BloomEffect {
 *                          threshold: 0.5
 *                          blurPassCount: 2
 *                      }
 *                  ]
 *              }
 *         }
 *     ]
 *    ...
 * }
 * \endcode
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
    \property Kuesa::ForwardRenderer::gamma

    Holds the gamma value to use for gamma correction that brings linear colors
    to sRGB colors.
    \default 2.2
    \since Kuesa 1.1
 */

/*!
    \qmlproperty real Kuesa::ForwardRenderer::gamma

    Holds the gamma value to use for gamma correction that brings linear colors
    to sRGB colors.
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
    \default ToneMappingAndGammaCorrectionEffect::None
 */

/*!
    \qmlproperty ToneMappingAndGammaCorrectionEffect.ToneMapping Kuesa::ForwardRenderer::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \since Kuesa 1.1
    \default ToneMappingAndGammaCorrectionEffect.None
 */

/*!
    \property bool Kuesa::ForwardRenderer::usesStencilMask

    Enables/disables stencil buffers. If true, stencil operations be used during the render phase to modify the stencil buffer.
    The resulting stencil buffer can later be used to apply post process effect to only part of the scene

    \since Kuesa 1.3
    \default False
 */

/*!
    \qmlproperty bool Kuesa::ForwardRenderer::usesStencilMask

    Enables/disables stencil buffers. If true, stencil operations be used during the render phase to modify the stencil buffer.
    The resulting stencil buffer can later be used to apply post process effect to only part of the scene

    \since Kuesa 1.3
    \default False
 */

ForwardRenderer::ForwardRenderer(Qt3DCore::QNode *parent)
    : View(parent)
    , m_surfaceSelector(new Qt3DRender::QRenderSurfaceSelector())
    , m_effectsViewport(new Qt3DRender::QViewport())
    , m_stagesRoot(new Qt3DRender::QFrameGraphNode())
    , m_clearBuffers(new Qt3DRender::QClearBuffers())
    , m_clearRT0(new Qt3DRender::QClearBuffers())
    , m_internalFXStages(EffectsStagesPtr::create())
    , m_renderToTextureRootNode(new Qt3DRender::QRenderTargetSelector())
    , m_mainSceneLayerFilter(new Qt3DRender::QLayerFilter())
    , m_multisampleTarget(nullptr)
    , m_blitFramebufferNodeFromMSToFBO0(nullptr)
    , m_blitFramebufferNodeFromFBO0ToFBO1(nullptr)
    , m_msaaResolver(nullptr)
    , m_rt0rt1Resolver(nullptr)
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    , m_debugOverlay(new Qt3DRender::QDebugOverlay)
#endif
    , m_usesStencilMask(false)
    , m_gammaCorrectionFX(new ToneMappingAndGammaCorrectionEffect())
{
    m_effectsViewport->setObjectName(QLatin1String("KuesaPostProcessing"));
    m_stagesRoot->setObjectName(QLatin1String("KuesaStagesRoot"));
    m_renderToTextureRootNode->setObjectName(QLatin1String("KuesaMainSceneRenderToTexture"));
    m_mainSceneLayerFilter->setObjectName(QLatin1String("MainSceneLayerFilter"));

    m_mainSceneLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);

    m_renderTargets[0] = nullptr;
    m_renderTargets[1] = nullptr;

    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthStencilBuffer);
    new Qt3DRender::QNoDraw(m_clearBuffers);

    m_clearRT0->setBuffers(Qt3DRender::QClearBuffers::ColorDepthStencilBuffer);
    new Qt3DRender::QNoDraw(m_clearRT0);


    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged, m_clearRT0, &Qt3DRender::QClearBuffers::setClearColor);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged, this, &ForwardRenderer::clearColorChanged);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::buffersChanged, this, &ForwardRenderer::clearBuffersChanged);
    connect(m_effectsViewport, &Qt3DRender::QViewport::normalizedRectChanged, this, &ForwardRenderer::viewportRectChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::renderSurfaceChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::handleSurfaceChange);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::externalRenderTargetSizeChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::updateTextureSizes);
    connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::gammaChanged, this, &ForwardRenderer::gammaChanged);
    connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::exposureChanged, this, &ForwardRenderer::exposureChanged);
    connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::toneMappingAlgorithmChanged, this, &ForwardRenderer::toneMappingAlgorithmChanged);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setEnabled(false);
    // Add a NoDraw to the debug overlay FG branch to ensure nothing but the
    // overlay will be drawn
    new Qt3DRender::QNoDraw(m_debugOverlay);
    connect(m_debugOverlay, &Qt3DRender::QDebugOverlay::enabledChanged, this, &ForwardRenderer::showDebugOverlayChanged);
#endif

    {
        m_internalFXStages->setObjectName(QLatin1String("InternalFXStage"));
        // Add internal post FX to the pipeline
        m_internalFXStages->addEffect(m_gammaCorrectionFX);
    }

    rebuildFGTree();
}

ForwardRenderer::~ForwardRenderer()
{
}

/*!
 * Returns the color used to clear the screen at the start of each frame. The
 * color is returned in sRGB color space.
 */
QColor ForwardRenderer::clearColor() const
{
    const QColor linearColor = m_clearBuffers->clearColor();
    const float oneOverGamma = 1.0f / 2.2f;
    return QColor::fromRgbF(powf(linearColor.redF(), oneOverGamma),
                            powf(linearColor.greenF(), oneOverGamma),
                            powf(linearColor.blueF(), oneOverGamma),
                            linearColor.alphaF());
}

/*!
 * Returns which buffers are cleared at the start of each frame.
 */
Qt3DRender::QClearBuffers::BufferType ForwardRenderer::clearBuffers() const
{
    return m_clearBuffers->buffers();
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
    \default 2.2
    \since Kuesa 1.1
*/
float ForwardRenderer::gamma() const
{
    return m_gammaCorrectionFX->gamma();
}

bool ForwardRenderer::showDebugOverlay() const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return m_debugOverlay->isEnabled();
#else
    return false;
#endif
}

/*!
    Returns the tone mapping algorithm used by the shader.
    \default ToneMappingAndGammaCorrectionEffect::None
    \since Kuesa 1.1
 */
ToneMappingAndGammaCorrectionEffect::ToneMapping ForwardRenderer::toneMappingAlgorithm() const
{
    return m_gammaCorrectionFX->toneMappingAlgorithm();
}

bool ForwardRenderer::usesStencilMask() const
{
    return m_usesStencilMask;
}

/*!
 * Returns the surface where rendering will occur.
 */
QObject *ForwardRenderer::renderSurface() const
{
    return m_surfaceSelector->surface();
}

/*!
 * Sets the \a clearColor used to clear the screen at the start of each frame.
 * The color is expected to be in sRGB color space.
 */
void ForwardRenderer::setClearColor(const QColor &clearColor)
{
    // Convert QColor from sRGB to Linear
    const QColor linearColor = QColor::fromRgbF(std::pow(clearColor.redF(), 2.2),
                                                std::pow(clearColor.greenF(), 2.2),
                                                std::pow(clearColor.blueF(), 2.2),
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
    Sets the \a gamma value to use for gamma correction that brings linear
    colors to sRGB colors.
    \default 2.2
 */
void ForwardRenderer::setGamma(float gamma)
{
    m_gammaCorrectionFX->setGamma(gamma);
}

/*!
    Sets the \a exposure value to use for exposure correction
    \default 0
 */
void ForwardRenderer::setExposure(float exposure)
{
    m_gammaCorrectionFX->setExposure(exposure);
}

/*!
    Sets the tone mapping algorithm to \a toneMappingAlgorithm.
    \since Kuesa 1.1
*/
void ForwardRenderer::setToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm)
{
    m_gammaCorrectionFX->setToneMappingAlgorithm(toneMappingAlgorithm);
}

void ForwardRenderer::setShowDebugOverlay(bool showDebugOverlay)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setEnabled(showDebugOverlay);
#else
    Q_UNUSED(showDebugOverlay)
#endif
}

/*!
    Allows to use stencil buffer during the render phase. The resulting
    stencil buffer is then accesible from the post processing effects.
    This allows to apply post process effects only to part of the scene.

    \since Kuesa 1.3
*/
void ForwardRenderer::setUsesStencilMask(bool usesStencilMask)
{
    if (usesStencilMask == m_usesStencilMask)
        return;
    m_usesStencilMask = usesStencilMask;
    scheduleFGTreeRebuild();
    emit usesStencilMaskChanged(m_usesStencilMask);
}

void ForwardRenderer::addView(View *view)
{
    if (!view->parent())
        view->setParent(this);

    auto d = Qt3DCore::QNodePrivate::get(this);
    d->registerDestructionHelper(view, &ForwardRenderer::removeView, m_views);
    m_views.push_back(view);
    reconfigureFrameGraph();
}

void ForwardRenderer::removeView(View *view)
{
    auto it = std::find(std::begin(m_views), std::end(m_views), view);
    if (it != std::end(m_views)) {
        auto d = Qt3DCore::QNodePrivate::get(this);
        d->unregisterDestructionHelper(view);
        m_views.erase(it);
        reconfigureFrameGraph();
    }
}

/*!
    Dumps the FrameGraph tree to the console. Can be really convenient for
    debug or troubleshooting purposes.
 */
void ForwardRenderer::dump()
{
    qDebug() << Qt3DRender::QFrameGraphNodePrivate::get(this)->dumpFrameGraph();
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
    if (m_multisampleTarget != nullptr) {
        outputs += m_multisampleTarget->outputs();
        const QRect blitRect(QPoint(), targetSize);
        if (m_blitFramebufferNodeFromMSToFBO0) {
            m_blitFramebufferNodeFromMSToFBO0->setSourceRect(blitRect);
            m_blitFramebufferNodeFromMSToFBO0->setDestinationRect(blitRect);
        }
    }

    if (m_blitFramebufferNodeFromFBO0ToFBO1) {
        const QRect blitRect(QPoint(), targetSize);
        m_blitFramebufferNodeFromFBO0ToFBO1->setSourceRect(blitRect);
        m_blitFramebufferNodeFromFBO0ToFBO1->setDestinationRect(blitRect);
    }

    for (auto output : qAsConst(outputs))
        output->texture()->setSize(targetSize.width(), targetSize.height());

    // TO DO: Do that recursively for effects in the child views
    for (auto effect : m_fxs)
        effect->setWindowSize(targetSize);

    for (const View *v : m_views)
        v->m_fxStages->setWindowSize(targetSize);
}

/*!
 * \internal
 *
 * Connects to the appropriate signal for the surface type to know when
 * know when the surface size has changed so offscreen buffers can be resized.
 */
void ForwardRenderer::handleSurfaceChange()
{
    auto currentSurface = m_surfaceSelector->surface();
    Q_ASSERT(currentSurface);

    // disconnect any existing connections
    for (auto connection : qAsConst(m_resizeConnections))
        disconnect(connection);
    m_resizeConnections.clear();

    // surface should only be a QWindow or QOffscreenSurface. Have to downcast since QSurface isn't QObject
    if (auto window = qobject_cast<QWindow *>(currentSurface)) {
        m_resizeConnections.push_back(connect(window, &QWindow::widthChanged, this, &ForwardRenderer::updateTextureSizes));
        m_resizeConnections.push_back(connect(window, &QWindow::heightChanged, this, &ForwardRenderer::updateTextureSizes));
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
    // I)
    // The idea is to render the Scenes for this main View and subViews
    // into a texture

    // II)
    // Perform blit to non MS texture if using MSAA

    // III)
    // Execute Local Effects for each View
    //    This might mean copying RT0 to RT1 as we need to make sure
    //    that even if we perform the FX for a subpart part of the entire viewport,
    //    we want content for the areas outside the viewport to be populated
    // Execute Global Effects
    // Render to Screen

    m_clearBuffers->setParent(Q_NODE_NULLPTR);
    m_clearRT0->setParent(Q_NODE_NULLPTR);

    m_effectsViewport->setParent(Q_NODE_NULLPTR);
    m_stagesRoot->setParent(Q_NODE_NULLPTR);
    m_renderToTextureRootNode->setParent(Q_NODE_NULLPTR);
    m_mainSceneLayerFilter->setParent(Q_NODE_NULLPTR);

    // Clear all layers on mainSceneFilter
    const QVector<Qt3DRender::QLayer *> fxLayers = m_mainSceneLayerFilter->layers();
    for (Qt3DRender::QLayer *l : fxLayers)
        m_mainSceneLayerFilter->removeLayer(l);

    // Unparent stages for the ForwardRenderer View
    m_sceneStages->setParent(Q_NODE_NULLPTR);
    m_reflectionStages->setParent(Q_NODE_NULLPTR);

    // Unparent previous Scene Stages
    for (Qt3DCore::QNode *c : m_stagesRoot->childNodes())
        c->setParent(Q_NODE_NULLPTR);

    // Unparent Previous Effects Stages
    for (Qt3DCore::QNode *c : m_effectsViewport->childNodes())
        c->setParent(Q_NODE_NULLPTR);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setParent(Q_NODE_NULLPTR);
#endif

    // 1) Clear Screen
    m_surfaceSelector->setParent(this);
    m_clearBuffers->setParent(m_surfaceSelector);


    // 2) Render SceneStages into Texture

    // 2.1 Clear FBO Texture
    m_renderToTextureRootNode->setParent(m_surfaceSelector);
    m_clearRT0->setParent(m_renderToTextureRootNode);

    // 2.2) Parent scenes stages to layer filter that discards effects

    // We need to exclude the effects layers from being drawn in the main scene
    m_mainSceneLayerFilter->setParent(m_renderToTextureRootNode);
    m_stagesRoot->setParent(m_mainSceneLayerFilter);

    if (m_views.empty()) {
        View::reconfigureFrameGraphHelper(m_stagesRoot);
    } else {
        for (View *v : m_views)
            v->setParent(m_stagesRoot);
    }

    // Count User FX
    std::vector<View *> fxViews = m_views;
    if (fxViews.empty())
        fxViews.push_back(this);
    size_t userFXCount = 0;

    for (View *v : fxViews)
        userFXCount += v->m_fxs.size();

    // 2.3) Set up RenderTargets and optional Blits to RT[0] if using MSAA
    //      and blit between RT[0] and RT[1] to copy stencil mask
    const size_t totalFXCount = userFXCount + m_internalFXStages->effects().size();
    // Configure effects and create or recreate Render Targets
    setupRenderTargets(m_renderToTextureRootNode, totalFXCount);


    // 3) Set up Post Processing Effects
    Qt3DRender::QAbstractTexture *depthTex = nullptr;
    const auto &target0outputs = m_renderTargets[0]->outputs();
    // Output #0 is the color
    // Output #1 is the depth
    if (!target0outputs.empty())
        depthTex = target0outputs[1]->texture();

    // create a node to hold all the effect subtrees, under the main viewport. They don't need camera, alpha, frustum, etc.
    m_effectsViewport->setParent(m_surfaceSelector);

    auto setUpEffectStage = [&] (EffectsStagesPtr &stage,
            Qt3DRender::QRenderTarget *rtA,
            Qt3DRender::QRenderTarget *rtB,
            bool blitRts = true,
            bool presentLastFXToScreen = false) {
        // Don't add the effect stage it if has no effects
        if (stage->effects().size() == 0)
            return;

        const QSize surfaceSize = currentSurfaceSize();
        stage->setWindowSize(surfaceSize);
        stage->setDepthTexture(depthTex);
        stage->setRenderTargets(rtA, rtB);
        stage->setPresentToScreen(presentLastFXToScreen);
        stage->setParent(m_effectsViewport);
        stage->setBlitFinalRT(blitRts);

        // Exclude fx layers from the main scene
        const std::vector<Qt3DRender::QLayer *> &layers = stage->layers();
        for (Qt3DRender::QLayer *layer : layers)
            m_mainSceneLayerFilter->addLayer(layer);
    };

    // Setup RenderTargets for Effects
    int currentRT = 0;
    const bool hasMultipleViews = fxViews.size() > 1;
    for (View *v : fxViews) {
        // When we have effects in different views, we need to blit
        // RTA and RTB after we have performed all the FX for a given view
        // This is because effects are only applied to the region the view targets
        // and we flip flop between buffers
        setUpEffectStage(v->m_fxStages,
                         m_renderTargets[currentRT],
                         m_renderTargets[1 - currentRT],
                         hasMultipleViews);
        if (v->m_fxs.size()  % 2 != 0)
            currentRT = 1 - currentRT;
    }

    // Internal FXs
    setUpEffectStage(m_internalFXStages,
                     m_renderTargets[currentRT],
                     m_renderTargets[1 - currentRT],
                     false,
            true);
    // We shoud always have at least 1 internalFX (for Gamma/Exposure)
    Q_ASSERT(m_internalFXStages->effects().size() > 0);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setParent(m_surfaceSelector);
#endif

    const bool blocked = blockNotifications(true);
    emit frameGraphTreeReconfigured();
    blockNotifications(blocked);

//    dump();
}

void ForwardRenderer::setupRenderTargets(Qt3DRender::QRenderTargetSelector *sceneTargetSelector,
                                         size_t totalFXCount)
{
    // Render Targets are owned by the FrameGraph itself There should be few
    // reasons that requires them to be recreatd when the FrameGraph tree has
    // to be rebuild. One of them is when we need to add a Stencil Attachment.
    // Resize of the attachments when the surface size changes is handled by
    // handleSurfaceChange and updateTextureSizes
    const bool fboHasStencilAttachment =
            (FrameGraphUtils::renderTargetHasAttachmentOfType(m_renderTargets[0], Qt3DRender::QRenderTargetOutput::Stencil) ||
             FrameGraphUtils::renderTargetHasAttachmentOfType(m_renderTargets[0], Qt3DRender::QRenderTargetOutput::DepthStencil));
    const QSize surfaceSize = currentSurfaceSize();
    const FrameGraphUtils::RenderingFeatures glFeatures = FrameGraphUtils::checkRenderingFeatures();

    FrameGraphUtils::RenderTargetFlags baseRtFlags;
    if (FrameGraphUtils::hasHalfFloatRenderable())
        baseRtFlags |= FrameGraphUtils::HalfFloatAttachments;
    if (m_usesStencilMask)
        baseRtFlags |= FrameGraphUtils::IncludeStencil;

    const bool recreateRenderTargets = (m_usesStencilMask != fboHasStencilAttachment);
    if (recreateRenderTargets) {
        delete m_renderTargets[0];
        delete m_renderTargets[1];
        delete m_multisampleTarget;
        delete m_blitFramebufferNodeFromFBO0ToFBO1;
        delete m_msaaResolver;
        m_renderTargets[0] = nullptr;
        m_renderTargets[1] = nullptr;
        m_multisampleTarget = nullptr;
        m_blitFramebufferNodeFromFBO0ToFBO1 = nullptr;
        m_msaaResolver = nullptr;
    }

    // Unparent nodes
    if (m_msaaResolver)
        m_msaaResolver->setParent(Q_NODE_NULLPTR);
    if (m_blitFramebufferNodeFromMSToFBO0)
        m_blitFramebufferNodeFromMSToFBO0->setParent(Q_NODE_NULLPTR);
    if (m_blitFramebufferNodeFromFBO0ToFBO1)
        m_blitFramebufferNodeFromFBO0ToFBO1->setParent(Q_NODE_NULLPTR);

    // Recreate RenderTargets if required
    if (!m_renderTargets[0]) {
        // create a render target for main scene
        m_renderTargets[0] = FrameGraphUtils::createRenderTarget(baseRtFlags|FrameGraphUtils::IncludeDepth,
                                                this, surfaceSize);
    }
    if (!m_renderTargets[1] && totalFXCount > 1) {
        m_renderTargets[1] = FrameGraphUtils::createRenderTarget(baseRtFlags, this, surfaceSize);
        // create a secondary render target to do ping-pong when we have
        // more than 1 fx
    }

    const int samples = QSurfaceFormat::defaultFormat().samples();
    const bool useMSAA = glFeatures.hasMultisampledFBO && samples > 1;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const bool usesRHI = qgetenv("QT3D_RENDERER") == QByteArray("rhi");
#else
    const bool usesRHI = false;
#endif

    // If we support MSAA FBO -> Then render into it and Blit into regular non FBO
    // for post FX
    if (useMSAA) {
        if (!m_multisampleTarget)
            m_multisampleTarget = FrameGraphUtils::createRenderTarget(baseRtFlags|FrameGraphUtils::Multisampled|FrameGraphUtils::IncludeDepth,
                                                                      this,
                                                                      surfaceSize,
                                                                      samples);
        // RHI has no Blit operations so we manually resolve the multisampled
        // FBO into renderTarget[0]
        if (usesRHI) {
            if (!m_msaaResolver)
                m_msaaResolver = new FBOResolver();
            m_msaaResolver->setParent(sceneTargetSelector);
            m_msaaResolver->setSource(FrameGraphUtils::findRenderTargetTexture(m_multisampleTarget, Qt3DRender::QRenderTargetOutput::Color0));
            m_msaaResolver->setDestination(m_renderTargets[0]);
        } else {
            // Blit into regular Tex2D FBO
            if (!m_blitFramebufferNodeFromMSToFBO0) {
                m_blitFramebufferNodeFromMSToFBO0 = new Qt3DRender::QBlitFramebuffer();
                auto noDraw = new Qt3DRender::QNoDraw(m_blitFramebufferNodeFromMSToFBO0);
                Q_UNUSED(noDraw);
            }
            m_blitFramebufferNodeFromMSToFBO0->setParent(sceneTargetSelector);
            m_blitFramebufferNodeFromMSToFBO0->setSource(m_multisampleTarget);
            m_blitFramebufferNodeFromMSToFBO0->setDestination(m_renderTargets[0]);
            m_blitFramebufferNodeFromMSToFBO0->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
            m_blitFramebufferNodeFromMSToFBO0->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

            const QRect blitRect(QPoint(), surfaceSize);
            m_blitFramebufferNodeFromMSToFBO0->setSourceRect(blitRect);
            m_blitFramebufferNodeFromMSToFBO0->setDestinationRect(blitRect);

        }
        // Setup Scene RenderTarget Selector
        sceneTargetSelector->setTarget(m_multisampleTarget);
    } else {
        // Render into non MSAA FBO directly
        sceneTargetSelector->setTarget(m_renderTargets[0]);
    }

    // Blit RT0 into RT1
    if (m_renderTargets[1]) {
        if (usesRHI) {
            if (!m_rt0rt1Resolver)
                m_rt0rt1Resolver = new FBOResolver();
            m_rt0rt1Resolver->setParent(sceneTargetSelector);
            m_rt0rt1Resolver->setSource(FrameGraphUtils::findRenderTargetTexture(m_renderTargets[0], Qt3DRender::QRenderTargetOutput::Color0));
            m_rt0rt1Resolver->setDestination(m_renderTargets[1]);
        } else {
            // Blit FBO 0 to FBO 1 to be able to access stencil of the render in post fx
            if (!m_blitFramebufferNodeFromFBO0ToFBO1) {
                m_blitFramebufferNodeFromFBO0ToFBO1 = new Qt3DRender::QBlitFramebuffer();
                auto noDraw = new Qt3DRender::QNoDraw(m_blitFramebufferNodeFromFBO0ToFBO1);
                Q_UNUSED(noDraw);
            }

            m_blitFramebufferNodeFromFBO0ToFBO1->setParent(sceneTargetSelector);
            m_blitFramebufferNodeFromFBO0ToFBO1->setSource(m_renderTargets[0]);
            m_blitFramebufferNodeFromFBO0ToFBO1->setDestination(m_renderTargets[1]);
            m_blitFramebufferNodeFromFBO0ToFBO1->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
            m_blitFramebufferNodeFromFBO0ToFBO1->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

            const QRect blitRect(QPoint(), surfaceSize);
            m_blitFramebufferNodeFromFBO0ToFBO1->setSourceRect(blitRect);
            m_blitFramebufferNodeFromFBO0ToFBO1->setDestinationRect(blitRect);
        }
    }
}

const std::vector<View *> &ForwardRenderer::views() const
{
    return m_views;
}

/*!
 * \internal
 *
 * Helper functionto return the current render surface size
 */
QSize ForwardRenderer::currentSurfaceSize() const
{
    QSize size = m_surfaceSelector->externalRenderTargetSize();

    if (!size.isValid()) {
        auto currentSurface = m_surfaceSelector->surface();
        if (auto window = qobject_cast<QWindow *>(currentSurface)) {
            size = window->size() * window->screen()->devicePixelRatio();
        }
    }
    if (!size.isValid())
        size = QSize(512, 512);

    return size;
}

/*!
 * Sets the surface where rendering will occur to \a renderSurface. This can be
 * an offscreen surface or a window surface.
 */
void ForwardRenderer::setRenderSurface(QObject *renderSurface)
{
    m_surfaceSelector->setSurface(renderSurface);
}

QT_END_NAMESPACE
