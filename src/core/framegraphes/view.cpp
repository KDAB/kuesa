/*
    view.cpp

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

#include "view.h"
#include <QSurfaceFormat>
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
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qclearbuffers.h>

#include <Kuesa/reflectionplane.h>
#include <Kuesa/tonemappingandgammacorrectioneffect.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DRender/private/qframegraphnode_p.h>
#include <private/kuesa_utils_p.h>
#include <private/kuesa_p.h>
#include <private/scenestages_p.h>
#include <private/effectsstages_p.h>
#include <private/reflectionstages_p.h>
#include <private/particlerenderstage_p.h>
#include <private/framegraphutils_p.h>
#include <private/fboresolver_p.h>

#include <cmath>


QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
 * \internal
 * Forward Renderer FrameGraph for a View
 * This leaves the option of later on having different FrameGraph to represent
 * a View
 */
View::ViewForward::ViewForward(View *v)
    : m_renderToTextureRootNode(new Qt3DRender::QRenderTargetSelector())
    , m_clearRT0(new Qt3DRender::QClearBuffers())
    , m_mainSceneLayerFilter(new Qt3DRender::QLayerFilter())
    , m_view(v)
{
    m_renderToTextureRootNode->setObjectName(QLatin1String("KuesaViewMainSceneRenderToTexture"));
    m_mainSceneLayerFilter->setObjectName(QLatin1String("ViewMainSceneLayerFilter"));

    m_mainSceneLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);

    m_clearRT0->setBuffers(Qt3DRender::QClearBuffers::ColorDepthStencilBuffer);
    new Qt3DRender::QNoDraw(m_clearRT0);
}

void View::ViewForward::setClearColor(const QColor &color)
{
    m_clearRT0->setClearColor(color);
}

QColor View::ViewForward::clearColor()
{
    return m_clearRT0->clearColor();
}

void View::ViewForward::setupRenderTargets(Qt3DRender::QRenderTargetSelector *sceneTargetSelector,
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
    const QSize targetSize = m_view->currentTargetSize();
    const FrameGraphUtils::RenderingFeatures glFeatures = FrameGraphUtils::checkRenderingFeatures();

    FrameGraphUtils::RenderTargetFlags baseRtFlags;
    if (FrameGraphUtils::hasHalfFloatRenderable())
        baseRtFlags |= FrameGraphUtils::HalfFloatAttachments;
    const bool usesStencil = m_view->usesStencilMask();
    if (usesStencil)
        baseRtFlags |= FrameGraphUtils::IncludeStencil;

    const bool recreateRenderTargets = (usesStencil != fboHasStencilAttachment);
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
                                                m_view, targetSize);
    }
    if (!m_renderTargets[1] && totalFXCount > 1) {
        m_renderTargets[1] = FrameGraphUtils::createRenderTarget(baseRtFlags, m_view, targetSize);
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
                                                                      m_view,
                                                                      targetSize,
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

            const QRect blitRect(QPoint(), targetSize);
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

            const QRect blitRect(QPoint(), targetSize);
            m_blitFramebufferNodeFromFBO0ToFBO1->setSourceRect(blitRect);
            m_blitFramebufferNodeFromFBO0ToFBO1->setDestinationRect(blitRect);
        }
    }
}

/*!
 * \internal
 *
 * Updates all the off-screen rendering textures whenever the render surface
 * size changes
 */
void View::ViewForward::updateTextureSizes()
{
    const QSize targetSize = m_view->currentTargetSize();

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
    for (auto effect : m_view->m_fxs)
        effect->setWindowSize(targetSize);

    m_view->m_fxStages->setWindowSize(targetSize);
    m_view->m_internalFXStages->setWindowSize(targetSize);
}

/*!
 * \internal
 */
void View::ViewForward::reconfigure(Qt3DRender::QFrameGraphNode *fgRoot)
{
    m_renderToTextureRootNode->setParent(Q_NODE_NULLPTR);
    m_clearRT0->setParent(Q_NODE_NULLPTR);
    m_mainSceneLayerFilter->setParent(Q_NODE_NULLPTR);

    // Clear all layers on mainSceneFilter
    const QVector<Qt3DRender::QLayer *> fxLayers = m_mainSceneLayerFilter->layers();
    for (Qt3DRender::QLayer *l : fxLayers)
        m_mainSceneLayerFilter->removeLayer(l);

    // Unparent Stages
    m_view->m_sceneStages->setParent(Q_NODE_NULLPTR);
    m_view->m_reflectionStages->setParent(Q_NODE_NULLPTR);
    m_view->m_fxStages->setParent(Q_NODE_NULLPTR);
    m_view->m_internalFXStages->setParent(Q_NODE_NULLPTR);

    // Rebuild FG

    // 1) Setup RT for View
    m_renderToTextureRootNode->setParent(fgRoot);

    // 1.1 Clear FBO Texture
    m_clearRT0->setParent(m_renderToTextureRootNode);

    // 1.2) Render Stages
    m_mainSceneLayerFilter->setParent(m_renderToTextureRootNode);

    // We draw reflections prior to drawing the scene
    const bool needsReflections = m_view->m_reflectionPlanes.size() > 0;
    if (needsReflections)
        m_view->m_reflectionStages->setParent(m_mainSceneLayerFilter);

    // Render Scene

    // The scene is drawn with a viewport of (0, 0, 1, 1) regardless of the
    // viewport rect set on the View as this parts gets rendered into a texture.
    m_view->m_sceneStages->setParent(m_mainSceneLayerFilter);

    // 2.3) Set up RenderTargets and optional Blits to RT[0] if using MSAA
    // and blit between RT[0] and RT[1] to copy stencil mask
    const size_t userFXCount = m_view->m_fxs.size();
    const size_t totalFXCount = userFXCount + m_view->m_internalFXStages->effects().size();
    setupRenderTargets(m_renderToTextureRootNode, totalFXCount);

    // 3) Set up Post Processing Effects
    Qt3DRender::QAbstractTexture *depthTex = nullptr;
    const auto &target0outputs = m_renderTargets[0]->outputs();
    // Output #0 is the color
    // Output #1 is the depth
    if (!target0outputs.empty())
        depthTex = target0outputs[1]->texture();

    auto setUpEffectStage = [&] (EffectsStagesPtr &stage,
            Qt3DRender::QRenderTarget *rtA,
            Qt3DRender::QRenderTarget *rtB,
            bool blitRts = true,
            bool presentLastFXToScreen = false) {
        // Don't add the effect stage it if has no effects
        if (stage->effects().size() == 0)
            return;

        stage->setDepthTexture(depthTex);
        stage->setRenderTargets(rtA, rtB);
        stage->setPresentToScreen(presentLastFXToScreen);
        stage->setParent(fgRoot);
        stage->setBlitFinalRT(blitRts);

        // Exclude fx layers from the main scene
        const std::vector<Qt3DRender::QLayer *> &layers = stage->layers();
        for (Qt3DRender::QLayer *layer : layers)
            m_mainSceneLayerFilter->addLayer(layer);
    };

    // Setup RenderTargets for Effects
    int currentRT = 0;
    setUpEffectStage(m_view->m_fxStages,
                     m_renderTargets[currentRT],
                     m_renderTargets[1 - currentRT],
                     false);
    if (m_view->m_fxs.size()  % 2 != 0)
        currentRT = 1 - currentRT;

    // Internal FXs
    setUpEffectStage(m_view->m_internalFXStages,
                     m_renderTargets[currentRT],
                     m_renderTargets[1 - currentRT],
                    false,
                    true); // Present to screen
    // We shoud always have at least 1 internalFX (for Gamma/Exposure)
    Q_ASSERT(m_view->m_internalFXStages->effects().size() > 0);
}

/*!
    \class Kuesa::View
    \brief View allows to specify the rendering description for a rendered view
    of the scene. They are to be used along with the QForwardRenderer
    FrameGraph.
    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Qt3DRender::QFrameGraphNode

    View allows to specify the description of a rendered view of the scene.
    They are to be used along with the QForwardRenderer FrameGraph.

    This is especially useful to render on the same window multiple views of
    different subset of content coming from a glTF file and scene from
    different view points.

    Views are rendered in the order in which they were added to the ForwardRenderer.

    \badcode
    Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();

    Kuesa::View *view1 = new Kuesa::View();
    view1->setCamera(sceneCamera1);
    view1->setViewport(QRectF(0, 0, 0.5, 1);

    Kuesa::View *view2 = new Kuesa::View();
    view2->setCamera(sceneCamera2);
    view2->setViewport(QRectF(0.5, 0, 0.5, 1);
    view2->addLayer(groundLayer);

    frameGraph->addView(view1);
    frameGraph->addView(view2);
    \endcode

*/

/*!
    \qmltype View
    \brief View allows to specify the description of a rendered view of the scene.
    They are to be used along with the ForwardRenderer FrameGraph.
    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::View

    View allows to specify the rendering description for a rendered view
    of the scene. They are to be used along with the ForwardRenderer FrameGraph.

    This is especially useful to render on the same window multiple views of
    different subset of content coming from a glTF file and scene from
    different view points.

    Views are rendered in the order in which they were added to the ForwardRenderer.

    \badcode
    ForwardRenderer {
        views: [
            View {
                camera: sceneCamera1
                viewport: Qt.rectf(0, 0, 0.5, 1)
            },
            View {
                camera: sceneCamera2
                viewport: Qt.rectf(0.5, 0, 0.5, 1)
                layers: [ groundLayer ]
            }
        ]
    }
    \endcode
*/

View::View(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(parent)
    , m_sceneStages(SceneStagesPtr::create())
    , m_reflectionStages(ReflectionStagesPtr::create())
    , m_fxStages(EffectsStagesPtr::create())
    , m_internalFXStages(EffectsStagesPtr::create())
    , m_usesStencilMask(false)
    , m_gammaCorrectionFX(new ToneMappingAndGammaCorrectionEffect())
    , m_fg(new View::ViewForward(this))
{
    QObject::connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::gammaChanged, this, &View::gammaChanged);
    QObject::connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::exposureChanged, this, &View::exposureChanged);
    QObject::connect(m_gammaCorrectionFX, &ToneMappingAndGammaCorrectionEffect::toneMappingAlgorithmChanged, this, &View::toneMappingAlgorithmChanged);
    QObject::connect(m_reflectionStages.data(), &ReflectionStages::reflectionTextureChanged,
                     this, &View::reflectionTextureChanged);

    // Setup Internal PostFXs
    {
        m_internalFXStages->setObjectName(QLatin1String("InternalFXStage"));
        // Add internal post FX to the pipeline
        m_internalFXStages->addEffect(m_gammaCorrectionFX);
    }

    rebuildFGTree();
}

View::~View()
{
}

/*!
    \property Kuesa::View::viewportRect

    Holds the viewport rectangle from within which the rendering will occur.
    Rectangle is in normalized coordinates.
*/

/*!
    \qmlproperty rect Kuesa::View::viewportRect

    Holds the viewport rectangle from within which the rendering will occur.
    Rectangle is in normalized coordinates.
*/

QRectF View::viewportRect() const
{
    return m_viewport;
}

/*!
    \property Kuesa::View::camera

    Holds the camera used to view the scene.
*/

/*!
    \qmlproperty Entity Kuesa::View::camera

    Holds the camera used to view the scene.
*/

Qt3DCore::QEntity *View::camera() const
{
    return m_camera;
}

/*!
    \property Kuesa::View::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/
bool View::frustumCulling() const
{
    return m_features & FrustumCulling;
}

/*!
    \property Kuesa::View::skinning

    Holds whether skinned mesh support is required.
    Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::skinning

    Holds whether skinned mesh support is required.
    Disabled by default.
*/
bool View::skinning() const
{
    return m_features & Skinning;
}

/*!
    \property Kuesa::View::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Disabled by default.
*/
bool View::backToFrontSorting() const
{
    return m_features & BackToFrontSorting;
}

/*!
    \property Kuesa::View::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/
bool View::zFilling() const
{
    return m_features & ZFilling;
}

/*!
    \property Kuesa::View::particlesEnabled

    Holds whether particles support is enabled. Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::particlesEnabled

    Holds whether particles support is enabled. Disabled by default.
*/
bool View::particlesEnabled() const
{
    return m_features & Particles;
}

/*!
    \property Kuesa::View::reflectionTexture

    Returns a 2D texture containing the reflections obtained from the
    reflection planes set for the View.
*/

/*!
    \qmlproperty Qt3DRender.AbstractTexture Kuesa::View::reflectionTexture

    Returns a 2D texture containing the reflections obtained from the
    reflection planes set for the View.
*/
Qt3DRender::QAbstractTexture *View::reflectionTexture() const
{
    return m_reflectionStages->reflectionTexture();
}

/*!
    \property Kuesa::View::reflectionTextureSize

    The size of the reflectionTexture. Defaults to 512x512. Increasing the size
    will lead to better reflections at the expense of more memory consumption.
*/

/*!
    \qmlproperty size Kuesa::View::reflectionTextureSize

    The size of the reflectionTexture. Defaults to 512x512. Increasing the size
    will lead to better reflections at the expense of more memory consumption.
*/
QSize View::reflectionTextureSize() const
{
    Qt3DRender::QAbstractTexture *t = m_reflectionStages->reflectionTexture();
    return {t->width(), t->height()};
}

/*!
 * Returns the color used to clear the screen at the start of each frame. The
 * color is returned in sRGB color space.
 */
QColor View::clearColor() const
{
    const QColor linearColor = m_fg->m_clearRT0->clearColor();
    const float oneOverGamma = 1.0f / 2.2f;
    return QColor::fromRgbF(powf(linearColor.redF(), oneOverGamma),
                            powf(linearColor.greenF(), oneOverGamma),
                            powf(linearColor.blueF(), oneOverGamma),
                            linearColor.alphaF());
}


/*!
    \property Kuesa::View::gamma

    Holds the gamma value to use for gamma correction that brings linear colors
    to sRGB colors.
    \default 2.2
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::View::gamma

    Holds the gamma value to use for gamma correction that brings linear colors
    to sRGB colors.
    \default 2.2
    \since Kuesa 1.3
 */

/*!
    Gamma correction value used for the linear to sRGB conversion.
    \default 2.2
    \since Kuesa 1.1
*/
float View::gamma() const
{
    return m_gammaCorrectionFX->gamma();
}

/*!
    \property Kuesa::View::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::View::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
    \since Kuesa 1.3
 */

/*!
    Exposure correction factor used before the linear to sRGB conversion.

    \default 0.0
*/
float View::exposure() const
{
    return m_gammaCorrectionFX->exposure();
}

/*!
    \property Kuesa::View::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \since Kuesa 1.3
    \default ToneMappingAndGammaCorrectionEffect::None
 */

/*!
    \qmlproperty ToneMappingAndGammaCorrectionEffect.ToneMapping Kuesa::View::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \since Kuesa 1.3
    \default ToneMappingAndGammaCorrectionEffect.None
 */

/*!
    Returns the tone mapping algorithm used by the shader.
    \default ToneMappingAndGammaCorrectionEffect::None
    \since Kuesa 1.1
 */
ToneMappingAndGammaCorrectionEffect::ToneMapping View::toneMappingAlgorithm() const
{
    return m_gammaCorrectionFX->toneMappingAlgorithm();
}

/*!
    \property bool Kuesa::View::usesStencilMask

    Enables/disables stencil buffers. If true, stencil operations be used during the render phase to modify the stencil buffer.
    The resulting stencil buffer can later be used to apply post process effect to only part of the scene

    \since Kuesa 1.3
    \default False
 */

/*!
    \qmlproperty bool Kuesa::View::usesStencilMask

    Enables/disables stencil buffers. If true, stencil operations be used during the render phase to modify the stencil buffer.
    The resulting stencil buffer can later be used to apply post process effect to only part of the scene

    \since Kuesa 1.3
    \default False
 */
bool View::usesStencilMask() const
{
    return m_usesStencilMask;
}

/*!
 * Sets the \a clearColor used to clear the screen at the start of each frame.
 * The color is expected to be in sRGB color space.
 */
void View::setClearColor(const QColor &clearColor)
{
    // Convert QColor from sRGB to Linear
    const QColor linearColor = QColor::fromRgbF(std::pow(clearColor.redF(), 2.2),
                                                std::pow(clearColor.greenF(), 2.2),
                                                std::pow(clearColor.blueF(), 2.2),
                                                clearColor.alphaF());
    if (linearColor == m_fg->clearColor())
        return;
    m_fg->setClearColor(linearColor);
    emit clearColorChanged(clearColor);
}


/*!
    \qmlproperty list<AbstractPostProcessingEffect> Kuesa::View::postProcessingEffects

    Holds the list of post processing effects.

    In essence this will complete the FrameGraph tree with a
    dedicated subtree provided by the effect.

    Lifetime of the subtree will be entirely managed by the ForwardRenderer.

    Be aware that registering several effects of the same type might have
    unexpected behavior. It is advised against unless explicitly documented in
    the effect.

    The FrameGraph tree is reconfigured upon replacing the list of effects.
*/
const std::vector<AbstractPostProcessingEffect *> &View::postProcessingEffects() const
{
    return m_fxs;
}

/*!
 * Returns all layers used by the view.
 */
const std::vector<Qt3DRender::QLayer *> &View::layers() const
{
    return m_layers;
}

const std::vector<ReflectionPlane *> &View::reflectionPlanes() const
{
    return m_reflectionPlanes;
}

/*!
 * Registers a new post processing effect \a effect with the View.
 * In essence this will complete the FrameGraph tree with a
 * dedicated subtree provided by the effect.
 *
 * Lifetime of the subtree will be entirely managed by the View.
 *
 * Be aware that registering several effects of the same type might have
 * unexpected behavior. It is advised against unless explicitly documented in
 * the effect.
 *
 * The FrameGraph tree is reconfigured upon insertion of a new effect.
 *
 * Effects added to a view, contrary to effects added to the ForwardRenderer,
 * only affect the view.
 */
void View::addPostProcessingEffect(AbstractPostProcessingEffect *effect)
{
    if (Utils::contains(m_fxs, effect))
        return;

    // Add effect to vector of registered effects
    m_fxs.push_back(effect);

    // Handle destruction of effect
    auto d = Qt3DCore::QNodePrivate::get(this);
    d->registerDestructionHelper(effect, &View::removePostProcessingEffect, m_fxs);

    // Request FG Tree rebuild
    rebuildFGTree();
}

/*!
 * Unregisters \a effect from the current View. This will destroy the
 * FrameGraph subtree associated with the effect.
 *
 * The FrameGraph tree is reconfigured upon removal of an effect.
 */
void View::removePostProcessingEffect(AbstractPostProcessingEffect *effect)
{
    // Remove effect entry
    if (Utils::removeAll(m_fxs, effect) <= 0)
        return;

    // Stop watching for effect destruction
    auto d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(effect);

    // Request FG Tree rebuild
    rebuildFGTree();
}

void View::addLayer(Qt3DRender::QLayer *layer)
{
    if (layer) {
        // Parent it to the View if they have no parent
        // We want to avoid them ever being parented to the SceneStages
        // As those might be destroyed when rebuilding the FG
        if (!layer->parent())
            layer->setParent(this);

        auto d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(layer, &View::removeLayer, m_layers);
        m_layers.push_back(layer);
        reconfigureStages();
    }
}

void View::removeLayer(Qt3DRender::QLayer *layer)
{
    auto it = std::find(std::begin(m_layers), std::end(m_layers), layer);
    if (it != std::end(m_layers)) {
        auto d = Qt3DCore::QNodePrivate::get(this);
        d->unregisterDestructionHelper(layer);
        m_layers.erase(it);
        reconfigureStages();
    }
}

void View::addReflectionPlane(ReflectionPlane *plane)
{
    if (plane) {
        // Parent it to the View if they have no parent
        // We want to avoid them ever being parented to the SceneStages
        // As those might be destroyed when rebuilding the FG
        if (!plane->parent())
            plane->setParent(this);

        auto d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(plane, &View::removeReflectionPlane, m_reflectionPlanes);
        m_reflectionPlanes.push_back(plane);
        rebuildFGTree();

        QObject::connect(plane, &ReflectionPlane::equationChanged,
                         this, &View::rebuildFGTree);
        QObject::connect(plane, &ReflectionPlane::layersChanged,
                         this, &View::rebuildFGTree);
    }
}

void View::removeReflectionPlane(ReflectionPlane *plane)
{
    auto it = std::find(std::begin(m_reflectionPlanes), std::end(m_reflectionPlanes), plane);
    if (it != std::end(m_reflectionPlanes)) {
        auto d = Qt3DCore::QNodePrivate::get(this);
        d->unregisterDestructionHelper(plane);
        m_reflectionPlanes.erase(it);
        rebuildFGTree();
        plane->disconnect(this);
    }
}

void View::setViewportRect(const QRectF &viewportRect)
{
    if (m_viewport == viewportRect)
        return;
    m_viewport = viewportRect;
    emit viewportRectChanged(viewportRect);
    reconfigureStages();

    m_fg->updateTextureSizes();
}

void View::setCamera(Qt3DCore::QEntity *camera)
{
    if (m_camera == camera)
        return;

    auto d = Qt3DCore::QNodePrivate::get(this);
    if (m_camera)
        d->unregisterDestructionHelper(m_camera);

    m_camera = camera;

    if (m_camera) {
        if (!m_camera->parent())
            m_camera->setParent(this);
        d->registerDestructionHelper(m_camera, &View::setCamera, m_camera);
    }

    const bool blocked = blockNotifications(true);
    emit cameraChanged(m_camera);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setFrustumCulling(bool frustumCulling)
{
    if (frustumCulling == bool(m_features & FrustumCulling))
        return;
    m_features.setFlag(FrustumCulling, frustumCulling);
    const bool blocked = blockNotifications(true);
    emit frustumCullingChanged(frustumCulling);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setSkinning(bool skinning)
{
    if (skinning == bool(m_features & Skinning))
        return;
    m_features.setFlag(Skinning, skinning);
    const bool blocked = blockNotifications(true);
    emit skinningChanged(skinning);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setBackToFrontSorting(bool backToFrontSorting)
{
    if (backToFrontSorting == bool(m_features & BackToFrontSorting))
        return;
    m_features.setFlag(BackToFrontSorting, backToFrontSorting);
    const bool blocked = blockNotifications(true);
    emit backToFrontSortingChanged(backToFrontSorting);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setZFilling(bool zfilling)
{
    if (zfilling == bool(m_features & ZFilling))
        return;
    m_features.setFlag(ZFilling, zfilling);
    const bool blocked = blockNotifications(true);
    emit zFillingChanged(zfilling);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setParticlesEnabled(bool enabled)
{
    if (enabled == bool(m_features & Particles))
        return;
    m_features.setFlag(Particles, enabled);
    const bool blocked = blockNotifications(true);
    emit particlesEnabledChanged(enabled);
    blockNotifications(blocked);
    rebuildFGTree();
}

/*!
    Sets the \a gamma value to use for gamma correction that brings linear
    colors to sRGB colors.
    \default 2.2
 */
void View::setGamma(float gamma)
{
    m_gammaCorrectionFX->setGamma(gamma);
}

/*!
    Sets the \a exposure value to use for exposure correction
    \default 0
 */
void View::setExposure(float exposure)
{
    m_gammaCorrectionFX->setExposure(exposure);
}

/*!
    Sets the tone mapping algorithm to \a toneMappingAlgorithm.
    \since Kuesa 1.1
*/
void View::setToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm)
{
    m_gammaCorrectionFX->setToneMappingAlgorithm(toneMappingAlgorithm);
}

/*!
    Allows to use stencil buffer during the render phase. The resulting
    stencil buffer is then accesible from the post processing effects.
    This allows to apply post process effects only to part of the scene.

    \since Kuesa 1.3
*/
void View::setUsesStencilMask(bool usesStencilMask)
{
    if (usesStencilMask == m_usesStencilMask)
        return;
    m_usesStencilMask = usesStencilMask;
    scheduleFGTreeRebuild();
    emit usesStencilMaskChanged(m_usesStencilMask);
}

/*!
    Dumps the FrameGraph tree to the console. Can be really convenient for
    debug or troubleshooting purposes.
 */
void View::dump()
{
    qDebug() << Qt3DRender::QFrameGraphNodePrivate::get(this)->dumpFrameGraph();
}


void View::setReflectionTextureSize(const QSize &reflectionTextureSize)
{
    if (this->reflectionTextureSize() == reflectionTextureSize)
        return;
    m_reflectionStages->setReflectionTextureSize(reflectionTextureSize);
    emit reflectionTextureSizeChanged(reflectionTextureSize);
}

void View::scheduleFGTreeRebuild()
{
    if (!m_fgTreeRebuiltScheduled) {
        m_fgTreeRebuiltScheduled = true;
        QMetaObject::invokeMethod(this, &View::rebuildFGTree, Qt::QueuedConnection);
    }
}

void View::rebuildFGTree()
{
    m_fgTreeRebuiltScheduled = false;
    // Reconfigure FrameGraph Tree
    reconfigureStages();
    reconfigureFrameGraph();
}

void View::reconfigureStages()
{
    const bool useSkinning = bool(m_features & Skinning);
    const bool useZFilling = bool(m_features & ZFilling);
    const bool sortBackToFront = bool(m_features & BackToFrontSorting);
    const bool useFrustumCulling = bool(m_features & FrustumCulling);
    const bool useParticles = bool(m_features & Particles);

    // Scene Stages
    m_sceneStages->setBackToFrontSorting(sortBackToFront);
    m_sceneStages->setSkinning(useSkinning);
    m_sceneStages->setZFilling(useZFilling);
    m_sceneStages->setFrustumCulling(useFrustumCulling);
    m_sceneStages->setParticlesEnabled(useParticles);
    m_sceneStages->setCamera(m_camera);
    m_sceneStages->setCullingMode(Qt3DRender::QCullFace::Back);

    // Update layers on sceneStages
    // Remove old layers
    const QVector<Qt3DRender::QLayer *> oldLayers = m_sceneStages->layers();
    for (Qt3DRender::QLayer *l : oldLayers)
        m_sceneStages->removeLayer(l);

    // Add new ones
    for (Qt3DRender::QLayer *l : m_layers)
        m_sceneStages->addLayer(l);

    // FX
    m_fxStages->setCamera(m_camera);
    m_fxStages->setViewport(m_viewport);
    m_internalFXStages->setCamera(m_camera);
    m_internalFXStages->setViewport(m_viewport);

    // Update fxs on effectsStages
    m_fxStages->clearEffects();

    for (Kuesa::AbstractPostProcessingEffect *fx : m_fxs)
        m_fxStages->addEffect(fx);

    // Reflections Stages
    m_reflectionStages->setBackToFrontSorting(sortBackToFront);
    m_reflectionStages->setSkinning(useSkinning);
    m_reflectionStages->setZFilling(useZFilling);
    m_reflectionStages->setFrustumCulling(useFrustumCulling);
    m_reflectionStages->setCamera(m_camera);
    // We need to cull Front faces for reflections
    m_reflectionStages->setCullingMode(Qt3DRender::QCullFace::Front);

    const bool needsReflections = m_reflectionPlanes.size() > 0;

    if (needsReflections) {
        if (m_reflectionPlanes.size() > 1)
            qCWarning(kuesa) << "Kuesa only handles a single reflection plane per View";
        // Remove layers
        const QVector<Qt3DRender::QLayer *> oldReflectionLayers = m_reflectionStages->layers();
        for (Qt3DRender::QLayer *l : oldLayers)
            m_reflectionStages->removeLayer(l);
        const std::vector<Qt3DRender::QLayer *> &reflectionVisibleLayers =
                m_reflectionPlanes[0]->layers().size() > 0 ? m_reflectionPlanes[0]->layers() : m_layers;
        // Readd layers
        for (Qt3DRender::QLayer *l : reflectionVisibleLayers)
            m_reflectionStages->addLayer(l);
        // Set equation
        m_reflectionStages->setReflectivePlaneEquation(m_reflectionPlanes[0]->equation());
    }
}

void View::reconfigureFrameGraph()
{
    reconfigureFrameGraphHelper(this);
}

void View::reconfigureFrameGraphHelper(Qt3DRender::QFrameGraphNode *fgRoot)
{
    // Reconfigure our FG
    m_fg->reconfigure(fgRoot);

    const bool blocked = blockNotifications(true);
    emit frameGraphTreeReconfigured();
    blockNotifications(blocked);
}

void View::setSurfaceSize(const QSize &size)
{
    if (size == m_surfaceSize)
        return;
    m_surfaceSize = size;
    m_fg->updateTextureSizes();
}

QSize View::surfaceSize() const
{
    return m_surfaceSize;
}

QSize View::currentTargetSize() const
{
    return { int(m_viewport.width() * m_surfaceSize.width()),
             int(m_viewport.height() * m_surfaceSize.height()) };
}

View *View::rootView() const
{
    Qt3DCore::QNode *parent = parentNode();
    View *parentView = qobject_cast<View *>(parent);

    // Find nearest parent that is a parent view
    while (parent != nullptr && parentView == nullptr) {
        parent = parentNode();
        parentView = qobject_cast<View *>(parent);
    }

    if (parentView)
        return parentView->rootView();
    return const_cast<View *>(this);
}

} // Kuesa

QT_END_NAMESPACE
