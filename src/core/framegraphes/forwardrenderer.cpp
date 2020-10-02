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
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <Qt3DRender/qdebugoverlay.h>
#include <private/qrhi_p.h>
#endif
#include <QWindow>
#include <QScreen>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/particles.h>
#include "zfillrenderstage_p.h"
#include "opaquerenderstage_p.h"
#include "transparentrenderstage_p.h"
#include "particlerenderstage_p.h"
#include "tonemappingandgammacorrectioneffect.h"
#include "msaafboresolver_p.h"
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

namespace {

struct RenderingFeatures {
    bool hasHalfFloatTexture = false;
    bool hasHalfFloatRenderable = false;
    bool hasMultisampledTexture = false;
    bool hasMultisampledFBO = false;
};

// Check supported GL Features
RenderingFeatures checkGLFeatures()
{
    RenderingFeatures features;
    QOffscreenSurface offscreen;
    QOpenGLContext ctx;

    if (qgetenv("QT3D_RENDERER") != QByteArray("rhi")) {
        offscreen.setFormat(QSurfaceFormat::defaultFormat());
        offscreen.create();
        Q_ASSERT_X(offscreen.isValid(), Q_FUNC_INFO, "Unable to create offscreen surface to gather capabilities");

        ctx.setFormat(QSurfaceFormat::defaultFormat());
        if (ctx.create()) {
            ctx.makeCurrent(&offscreen);
            const QSurfaceFormat format = ctx.format();
            // Since ES 3.0 or GL 3.0
            features.hasHalfFloatTexture = (format.majorVersion() >= 3 || ctx.hasExtension(QByteArray("GL_OES_texture_half_float")));
            // Since GL 3.0 or ES 3.2 or extension
            features.hasHalfFloatRenderable = (ctx.isOpenGLES() ? (format.majorVersion() == 3 && format.minorVersion() >= 2)
                                                                : format.majorVersion() >= 3) ||
                    ctx.hasExtension(QByteArray("GL_EXT_color_buffer_half_float"));
            // Since ES 3.1, GL 3.0 or extension
            features.hasMultisampledTexture = (ctx.isOpenGLES() ? (format.majorVersion() == 3 && format.minorVersion() >= 1)
                                                                : (format.majorVersion() >= 3)) ||
                    ctx.hasExtension(QByteArray("ARB_texture_multisample"));
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 5) || (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0) && QT_VERSION <= QT_VERSION_CHECK(5, 13, 1))
            // Blitting bug in Qt3D prevents correct depth blitting with multisampled FBO
            // Fixed for 5.12.6/5.13/2 with https://codereview.qt-project.org/c/qt/qt3d/+/276774
#else
            // Since ES 3.1, GL 3.0 or extension
            // TO DO: Find how to support that on ES
            features.hasMultisampledFBO = (ctx.isOpenGLES() ? (format.majorVersion() >= 3 && format.minorVersion() >= 1)
                                                            : format.majorVersion() >= 3);
#endif
            const bool forceMultisampledFBO = qgetenv("KUESA_FORCE_MULTISAMPLING").length() > 0;
            features.hasMultisampledFBO |= forceMultisampledFBO;

            ctx.doneCurrent();
        }
    } else {

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        // TO DO: Update QRenderCapabilities and use that instead
        features.hasHalfFloatTexture = true;
        features.hasHalfFloatRenderable = true;
        features.hasMultisampledTexture = true;
        features.hasMultisampledFBO = true;
#endif
    }

    qCDebug(kuesa) << "Rendering Features:\n"
                   << "hasHalfFloatTexture" << features.hasHalfFloatTexture << "\n"
                   << "hasHalfFloatRenderable" << features.hasHalfFloatRenderable << "\n"
                   << "hasMultisampledTexture" << features.hasMultisampledTexture << "\n"
                   << "hasMultisampledFBO" << features.hasMultisampledFBO;

    return features;
}
RenderingFeatures const *_glFeatures = nullptr;

enum RenderTargetFlag {
    IncludeDepth = (1 << 0),
    Multisampled = (1 << 1),
    HalfFloatAttachments = (1 << 2),
    IncludeStencil = (1 << 3)
};
Q_DECLARE_FLAGS(RenderTargetFlags, RenderTargetFlag)

/*!
 * \internal
 *
 * Helper function to create a QRenderTarget with the correct texture formats
 * and sizes.
 */
Qt3DRender::QRenderTarget *createRenderTarget(RenderTargetFlags flags,
                                              Qt3DCore::QNode *owner,
                                              const QSize surfaceSize,
                                              int samples = 0)
{
    auto renderTarget = new Qt3DRender::QRenderTarget(owner);
    Qt3DRender::QAbstractTexture *colorTexture = nullptr;
    if (flags & RenderTargetFlag::Multisampled) {
        colorTexture = new Qt3DRender::QTexture2DMultisample;
        colorTexture->setSamples(samples);
    } else {
        colorTexture = new Qt3DRender::QTexture2D;
    }
    // We need to use 16 based format as our content is HDR linear
    // which we will eventually exposure correct, tone map to LDR and
    // gamma correct
    // This requires support for extension OES_texture_half_float on ES2 platforms
    colorTexture->setFormat((flags & RenderTargetFlag::HalfFloatAttachments)
                                    ? Qt3DRender::QAbstractTexture::RGBA16F
                                    : Qt3DRender::QAbstractTexture::RGBA8_UNorm);
    colorTexture->setGenerateMipMaps(false);
    colorTexture->setSize(surfaceSize.width(), surfaceSize.height());
    auto colorOutput = new Qt3DRender::QRenderTargetOutput;

    colorOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    colorOutput->setTexture(colorTexture);
    renderTarget->addOutput(colorOutput);

    // Three options:
    // Depth is requested
    // Stencil is requested
    // Depth and stencil is requested

    const auto depthStencilFlags = flags & (RenderTargetFlag::IncludeDepth | RenderTargetFlag::IncludeStencil);
    if (depthStencilFlags) {
        Qt3DRender::QAbstractTexture::TextureFormat textureFormat = Qt3DRender::QAbstractTexture::NoFormat;
        Qt3DRender::QRenderTargetOutput::AttachmentPoint attachmentPoint = Qt3DRender::QRenderTargetOutput::Color0;
        if (depthStencilFlags == RenderTargetFlag::IncludeDepth) {
            textureFormat = Qt3DRender::QAbstractTexture::D24;
            attachmentPoint = Qt3DRender::QRenderTargetOutput::Depth;
        }
        if (depthStencilFlags == RenderTargetFlag::IncludeStencil) {
            textureFormat = Qt3DRender::QAbstractTexture::D24S8;
            attachmentPoint = Qt3DRender::QRenderTargetOutput::Stencil;
        }
        if (depthStencilFlags == (RenderTargetFlag::IncludeDepth | RenderTargetFlag::IncludeStencil)) {
            textureFormat = Qt3DRender::QAbstractTexture::D24S8;
            attachmentPoint = Qt3DRender::QRenderTargetOutput::DepthStencil;
        }

        Q_ASSERT(textureFormat != Qt3DRender::QAbstractTexture::NoFormat);
        Q_ASSERT(attachmentPoint != Qt3DRender::QRenderTargetOutput::Color0);

        Qt3DRender::QAbstractTexture *texture = nullptr;
        if (flags & RenderTargetFlag::Multisampled) {
            texture = new Qt3DRender::QTexture2DMultisample;
            texture->setSamples(samples);
        } else {
            texture = new Qt3DRender::QTexture2D;
        }
        texture->setSize(surfaceSize.width(), surfaceSize.height());
        texture->setGenerateMipMaps(false);
        texture->setFormat(textureFormat);
        auto depthStencilOutput = new Qt3DRender::QRenderTargetOutput;
        depthStencilOutput->setAttachmentPoint(attachmentPoint);
        depthStencilOutput->setTexture(texture);
        renderTarget->addOutput(depthStencilOutput);
    }

    return renderTarget;
}

bool renderTargetHasAttachmentOfType(const Qt3DRender::QRenderTarget *target,
                                     const Qt3DRender::QRenderTargetOutput::AttachmentPoint attachmentType)
{
    if (!target)
        return false;
    const auto &outputs = target->outputs();
    for (const Qt3DRender::QRenderTargetOutput *output : outputs) {
        if (output->attachmentPoint() == attachmentType)
            return true;
    }
    return false;
}

} // namespace

ForwardRenderer::ForwardRenderer(Qt3DCore::QNode *parent)
    : Qt3DRender::QRenderSurfaceSelector(parent)
    , m_noFrustumCullingOpaqueTechniqueFilter(new Qt3DRender::QTechniqueFilter())
    , m_frustumCullingOpaqueTechniqueFilter(new Qt3DRender::QTechniqueFilter())
    , m_noFrustumCullingTransparentTechniqueFilter(new Qt3DRender::QTechniqueFilter())
    , m_frustumCullingTransparentTechniqueFilter(new Qt3DRender::QTechniqueFilter())
    , m_viewport(0., 0., 1., 1.)
    , m_effectsViewport(new Qt3DRender::QViewport())
    , m_stagesRoot(new Qt3DRender::QFrameGraphNode())
    , m_camera(nullptr)
    , m_clearBuffers(new Qt3DRender::QClearBuffers())
    , m_noDrawClearBuffer(new Qt3DRender::QNoDraw())
    , m_frustumCullingOpaque(new Qt3DRender::QFrustumCulling())
    , m_frustumCullingTransparent(new Qt3DRender::QFrustumCulling())
    , m_backToFrontSorting(true)
    , m_zfilling(false)
    , m_renderToTextureRootNode(nullptr)
    , m_effectsRootNode(nullptr)
    , m_multisampleTarget(nullptr)
    , m_blitFramebufferNodeFromMSToFBO0(nullptr)
    , m_blitFramebufferNodeFromFBO0ToFBO1(nullptr)
    , m_msaaResolver(nullptr)
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    , m_debugOverlay(new Qt3DRender::QDebugOverlay)
#endif
    , m_fgTreeRebuiltScheduled(false)
    , m_particlesEnabled(false)
    , m_particleRenderStage(new ParticleRenderStage())
    , m_usesStencilMask(false)
    , m_gammaCorrectionFX(new ToneMappingAndGammaCorrectionEffect())
{
    m_effectsViewport->setObjectName(QLatin1String("KuesaPostProcessing"));
    m_stagesRoot->setObjectName(QLatin1String("KuesaStagesRoot"));

    m_frustumCullingOpaque->setObjectName(QLatin1String("KuesaFrustrumCullingOpaque"));
    m_noFrustumCullingOpaqueTechniqueFilter->setObjectName(QLatin1String("KuesaNoFrustrumCullingOpaque"));
    m_frustumCullingTransparentTechniqueFilter->setObjectName(QLatin1String("KuesaFrustrumCullingTransparent"));
    m_noFrustumCullingTransparentTechniqueFilter->setObjectName(QLatin1String("KuesaNoFrustrumCullingTransparent"));

    m_renderTargets[0] = nullptr;
    m_renderTargets[1] = nullptr;

    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        m_frustumCullingOpaqueTechniqueFilter->addMatch(filterKey);
        m_noFrustumCullingOpaqueTechniqueFilter->addMatch(filterKey);
        m_frustumCullingTransparentTechniqueFilter->addMatch(filterKey);
        m_noFrustumCullingTransparentTechniqueFilter->addMatch(filterKey);
    }
    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("allowCulling"));
        filterKey->setValue(true);
        m_frustumCullingOpaqueTechniqueFilter->addMatch(filterKey);
        m_frustumCullingTransparentTechniqueFilter->addMatch(filterKey);
    }
    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("allowCulling"));
        filterKey->setValue(false);
        m_noFrustumCullingOpaqueTechniqueFilter->addMatch(filterKey);
        m_noFrustumCullingTransparentTechniqueFilter->addMatch(filterKey);
    }

    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthStencilBuffer);

    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged, this, &ForwardRenderer::clearColorChanged);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::buffersChanged, this, &ForwardRenderer::clearBuffersChanged);
    connect(m_effectsViewport, &Qt3DRender::QViewport::normalizedRectChanged, this, &ForwardRenderer::viewportRectChanged);
    connect(this, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::renderSurfaceChanged);
    connect(this, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::handleSurfaceChange);
    connect(this, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::updateTextureSizes);
    connect(m_frustumCullingOpaque, &Qt3DRender::QFrustumCulling::enabledChanged, this, &ForwardRenderer::frustumCullingChanged);
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
        // Add internal post FX to the pipeline
        m_internalPostProcessingEffects.push_back(m_gammaCorrectionFX);
        m_effectFGSubtrees.insert(m_gammaCorrectionFX, m_gammaCorrectionFX->frameGraphSubTree());
    }

    rebuildFGTree();
}

ForwardRenderer::~ForwardRenderer()
{
    // unparent the effect subtrees or they'll be deleted twice
    for (auto &framegraph : qAsConst(m_effectFGSubtrees))
        framegraph->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    m_effectFGSubtrees.clear();
    m_sceneStages.clear();
    delete m_particleRenderStage;
}

/*!
 * Returns the camera used to render the scene.
 */
Qt3DCore::QEntity *ForwardRenderer::camera() const
{
    return m_camera;
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
 * Returns whether frustum culling is enabled or not.
 */
bool ForwardRenderer::frustumCulling() const
{
    return m_frustumCullingOpaque->isEnabled();
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

bool ForwardRenderer::particlesEnabled() const
{
    return m_particlesEnabled;
}

bool ForwardRenderer::usesStencilMask() const
{
    return m_usesStencilMask;
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

        scheduleFGTreeRebuild();
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
    // Remove effect entry
    if (m_userPostProcessingEffects.removeAll(effect) <= 0)
        return;

    // unparent FG subtree associated with Effect.
    m_effectFGSubtrees.take(effect)->setParent(static_cast<Qt3DCore::QNode *>(nullptr));

    // Reconfigure FrameGraph Tree
    scheduleFGTreeRebuild();
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
 * Returns the surface where rendering will occur.
 */
QObject *ForwardRenderer::renderSurface() const
{
    return surface();
}

/*!
 * Returns the viewport rectangle in normalized coordinates.
 */
QRectF ForwardRenderer::viewportRect() const
{
    return m_viewport;
}

/*!
 * Sets the \a camera used to view the scene.
 */
void ForwardRenderer::setCamera(Qt3DCore::QEntity *camera)
{
    if (m_camera != camera) {
        m_camera = camera;
        emit cameraChanged(m_camera);
        reconfigureStages();
    }
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
 * Activates \a frustumCulling of geometries which lie outside of the view frustum.
 */
void ForwardRenderer::setFrustumCulling(bool frustumCulling)
{
    m_frustumCullingOpaque->setEnabled(frustumCulling);
    m_frustumCullingTransparent->setEnabled(frustumCulling);
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
    Activates multi-pass \a zfilling support.

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

void ForwardRenderer::setParticlesEnabled(bool enabled)
{
    if (enabled == m_particlesEnabled)
        return;
    m_particlesEnabled = enabled;
    scheduleFGTreeRebuild();
    emit particlesEnabledChanged(enabled);
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

/*!
    Adds \a layer to the list of layers used for rendering. When no layer is
    set, everything gets rendered. As soon as you starting adding a layer, only
    the entities that match the layers will be rendered.

    Since 1.3, an optional camera and viewport can be provided.

    If passing a valid camera and/or viewport (in normalized coordinates), those will
    be selected when rendering. This can be used to display different objects in
    different portions of the screen.

    If no camera is specified, then the main default camera will be used. If no
    viewport is specified, then the main default viewport will be used.

\since Kuesa 1.3
*/
void ForwardRenderer::addLayer(Qt3DRender::QLayer *layer, Qt3DCore::QEntity *camera, QRectF viewport)
{
    if (layer) {
        // Parent it to the FG if they have no parent
        // We want to avoid them ever being parented to the SceneStages
        // As those might be destroyed when rebuilding the FG
        if (!layer->parent())
            layer->setParent(this);

        auto connection = QObject::connect(layer, &Qt3DCore::QNode::nodeDestroyed,
                                           this, [this, layer] { removeLayer(layer); });
        m_layers.push_back({ layer, camera, viewport, connection });
        reconfigureStages();
    } else
        qCWarning(Kuesa::kuesa) << Q_FUNC_INFO << "called with null layer";
}

/*!
    This is an overloaded method to add a layer and specify the associated camera and / or viewport.

\since Kuesa 1.3
*/
void ForwardRenderer::addLayer(Qt3DRender::QLayer *layer, Qt3DCore::QEntity *camera, qreal left, qreal top, qreal width, qreal height)
{
    addLayer(layer, camera, { left, top, width, height });
}

/*!
    Removes the \a layer from the layers used for rendering.
    \since Kuesa 1.2
*/
void ForwardRenderer::removeLayer(Qt3DRender::QLayer *layer, Qt3DCore::QEntity *camera, QRectF viewport)
{
    auto it = std::find_if(std::begin(m_layers), std::end(m_layers), [layer, camera, viewport](const LayerConfiguration &c) {
        return c.m_layer == layer && c.m_camera == camera && c.m_viewport == viewport;
    });
    if (it != std::end(m_layers)) {
        QObject::disconnect((*it).m_layerDestructionConnection);
        m_layers.erase(it);
        reconfigureStages();
    }
}

/*!
    This is an overloaded method to remove a layer with the associated camera and / or viewport.

\since Kuesa 1.3
        */
void ForwardRenderer::removeLayer(Qt3DRender::QLayer *layer, Qt3DCore::QEntity *camera, qreal left, qreal top, qreal width, qreal height)
{
    removeLayer(layer, camera, { left, top, width, height });
}

/*!
    Returns the layers used for rendering.
    \since Kuesa 1.2
*/
QVector<Qt3DRender::QLayer *> ForwardRenderer::layers() const
{
    QVector<Qt3DRender::QLayer *> res;
    std::transform(std::begin(m_layers), std::end(m_layers), std::back_inserter(res), [](const LayerConfiguration &c) { return c.m_layer; });
    return res;
}

void ForwardRenderer::clearLayers()
{
    m_layers.clear();
    reconfigureStages();
}

/*!
 * \internal
 *
 * Updates all the off-screen rendering textures whenever the render surface
 * size changes
 */
void ForwardRenderer::updateTextureSizes()
{
    QSize targetSize = currentSurfaceSize();
    if (!targetSize.isValid())
        targetSize = QSize(512, 512);

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
    for (auto effect : qAsConst(m_userPostProcessingEffects))
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
    auto currentSurface = surface();
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
    // Based on the effect types, reorder elements from the FrameGraph in the correct order
    // e.g We want Bloom to happen after DoF...
    //     We may need to render the scene into a texture first ...
    static const RenderingFeatures glFeatures = checkGLFeatures();
    _glFeatures = &glFeatures;

    m_effectsViewport->setParent(Q_NODE_NULLPTR);

    m_clearBuffers->setParent(this);
    m_noDrawClearBuffer->setParent(m_clearBuffers);

    // Camera selector hold the root FG node to render the gltf Scene
    // It might get reparented to something else than this
    // in case rendering into FBO is required
    m_stagesRoot->setParent(this);

    // Skinned Meshes are not checked against frustum culling as the skinning
    // could actually make them still be in the view frustum even if their
    // transform technically makes them out of sight

    // Non skinned opaque
    m_frustumCullingOpaque->setParent(m_stagesRoot);
    m_frustumCullingOpaqueTechniqueFilter->setParent(m_frustumCullingOpaque);

    // Skinned opaque
    m_noFrustumCullingOpaqueTechniqueFilter->setParent(m_stagesRoot);

    // Non skinned transparent
    m_frustumCullingTransparent->setParent(m_stagesRoot);
    m_frustumCullingTransparentTechniqueFilter->setParent(m_frustumCullingTransparent);

    // Skinned tranparent
    m_noFrustumCullingTransparentTechniqueFilter->setParent(m_stagesRoot);

    // Particles
    m_particleRenderStage->setParent(m_particlesEnabled ? m_stagesRoot : Q_NODE_NULLPTR);

    // Temporarily reparent effect subtrees and other nodes, then delete the node that held the last
    // subtree framegraph including any render target selectors.
    // It's easier just to re-create the tree below
    for (AbstractPostProcessingEffect *effect : qAsConst(m_userPostProcessingEffects))
        m_effectFGSubtrees.value(effect)->setParent(Q_NODE_NULLPTR);
    for (AbstractPostProcessingEffect *effect : qAsConst(m_internalPostProcessingEffects))
        m_effectFGSubtrees.value(effect)->setParent(Q_NODE_NULLPTR);
    for (const SceneStagesPtr &stage : qAsConst(m_sceneStages))
        stage->unParent();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setParent(Q_NODE_NULLPTR);
#endif

    delete m_effectsRootNode;
    m_effectsRootNode = nullptr;

    // delete the extra nodes to render main scene to texture.  Will recreate if needed
    delete m_renderToTextureRootNode;
    m_renderToTextureRootNode = nullptr;

    // Render Targets are owned by the FrameGraph itself There should be few
    // reasons that requires them to be recreatd when the FrameGraph tree has
    // to be rebuild. One of them is when we need to add a Stencil Attachment.
    // Resize of the attachments when the surface size changes is handled by
    // handleSurfaceChange and updateTextureSizes
    const bool fboHasStencilAttachment =
            (renderTargetHasAttachmentOfType(m_renderTargets[0], Qt3DRender::QRenderTargetOutput::Stencil) ||
             renderTargetHasAttachmentOfType(m_renderTargets[0], Qt3DRender::QRenderTargetOutput::DepthStencil));
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

    Qt3DRender::QAbstractTexture *depthTex = nullptr;

    // Configure effects
    const bool hasFX = !m_userPostProcessingEffects.empty() || !m_internalPostProcessingEffects.empty();
    if (hasFX) {
        RenderTargetFlags baseRenderTargetFlags;

        if (glFeatures.hasHalfFloatRenderable)
            baseRenderTargetFlags |= RenderTargetFlag::HalfFloatAttachments;

        if (m_usesStencilMask)
            baseRenderTargetFlags |= RenderTargetFlag::IncludeStencil;

        const QSize surfaceSize = currentSurfaceSize();
        if (!m_renderTargets[0]) {
            // create a render target for main scene
            m_renderTargets[0] = createRenderTarget(baseRenderTargetFlags | RenderTargetFlag::IncludeDepth,
                                                    this, surfaceSize);
        }
        const auto userFXCount = m_userPostProcessingEffects.size();
        const auto totalFXCount = userFXCount + m_internalPostProcessingEffects.size();
        if (totalFXCount > 1 && !m_renderTargets[1]) {
            m_renderTargets[1] = createRenderTarget(baseRenderTargetFlags, this, surfaceSize);
            // create a secondary render target to do ping-pong when we have
            // more than 1 fx
        }

        const auto &target0outputs = m_renderTargets[0]->outputs();
        // Output #0 is the color
        // Output #1 is the depth
        if (!target0outputs.empty())
            depthTex = target0outputs[1]->texture();

        // Remove stages subtree
        m_stagesRoot->setParent(Q_NODE_NULLPTR);

        // create a subtree under this to render the main scene into a texture
        // Effectively taking the place of where the CameraSelector node was in the
        // tree
        m_renderToTextureRootNode = new Qt3DRender::QFrameGraphNode(this);
        m_renderToTextureRootNode->setObjectName(QStringLiteral("KuesaMainScene"));

        // need to exclude the effects layers from being drawn in the main scene
        auto mainSceneFilter = new Qt3DRender::QLayerFilter(m_renderToTextureRootNode);
        mainSceneFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);
        auto sceneTargetSelector = new Qt3DRender::QRenderTargetSelector(mainSceneFilter);

        auto clearScreen = new Qt3DRender::QClearBuffers(sceneTargetSelector);
        clearScreen->setBuffers(Qt3DRender::QClearBuffers::ColorDepthStencilBuffer);
        clearScreen->setClearColor(m_clearBuffers->clearColor());
        connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged,
                clearScreen, &Qt3DRender::QClearBuffers::setClearColor);
        new Qt3DRender::QNoDraw(clearScreen);

        // Reparent stages to sceneTargetSelector
        m_stagesRoot->setParent(sceneTargetSelector);

        // If we support MSAA FBO -> Then render into it and Blit into regular non FBO
        // for post FX
        const int samples = QSurfaceFormat::defaultFormat().samples();
        if (glFeatures.hasMultisampledFBO && samples > 1) {
            // Render Into MSAA FBO
            if (!m_multisampleTarget) {
                m_multisampleTarget = createRenderTarget(baseRenderTargetFlags | RenderTargetFlag::Multisampled | RenderTargetFlag::IncludeDepth,
                                                         this,
                                                         surfaceSize,
                                                         samples);
            }

            sceneTargetSelector->setTarget(m_multisampleTarget);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            const bool usesRHI = qgetenv("QT3D_RENDERER") == QByteArray("rhi");
#else
            const bool usesRHI = false;
#endif
            // RHI has no Blit operations so we manually resolve the multisampled
            // FBO into renderTarget[0]
            if (usesRHI) {
                m_msaaResolver = new MSAAFBOResolver();
                m_msaaResolver->setParent(sceneTargetSelector);
                m_msaaResolver->setSource(findRenderTargetTexture(m_multisampleTarget, Qt3DRender::QRenderTargetOutput::Color0));
                m_msaaResolver->setDestination(m_renderTargets[0]);
                m_msaaResolver->setYFlip(totalFXCount % 2 != 0);
            } else {
                // Blit into regular Tex2D FBO
                m_blitFramebufferNodeFromMSToFBO0 = new Qt3DRender::QBlitFramebuffer(sceneTargetSelector);

                m_blitFramebufferNodeFromMSToFBO0->setSource(m_multisampleTarget);
                m_blitFramebufferNodeFromMSToFBO0->setDestination(m_renderTargets[0]);
                m_blitFramebufferNodeFromMSToFBO0->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
                m_blitFramebufferNodeFromMSToFBO0->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

                const QRect blitRect(QPoint(), surfaceSize);
                m_blitFramebufferNodeFromMSToFBO0->setSourceRect(blitRect);
                m_blitFramebufferNodeFromMSToFBO0->setDestinationRect(blitRect);

                auto noDraw = new Qt3DRender::QNoDraw(m_blitFramebufferNodeFromMSToFBO0);
                Q_UNUSED(noDraw);
            }
        } else {
            // Render into non MSAA FBO directly
            sceneTargetSelector->setTarget(m_renderTargets[0]);
        }

        // Blit FBO 0 to FBO 1 to be able to access stencil of the render in post fx
        if (m_renderTargets[1] && m_usesStencilMask && m_blitFramebufferNodeFromFBO0ToFBO1 == nullptr) {
            m_blitFramebufferNodeFromFBO0ToFBO1 = new Qt3DRender::QBlitFramebuffer(sceneTargetSelector);

            m_blitFramebufferNodeFromFBO0ToFBO1->setSource(m_renderTargets[0]);
            m_blitFramebufferNodeFromFBO0ToFBO1->setDestination(m_renderTargets[1]);
            m_blitFramebufferNodeFromFBO0ToFBO1->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
            m_blitFramebufferNodeFromFBO0ToFBO1->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

            const QRect blitRect(QPoint(), surfaceSize);
            m_blitFramebufferNodeFromFBO0ToFBO1->setSourceRect(blitRect);
            m_blitFramebufferNodeFromFBO0ToFBO1->setDestinationRect(blitRect);

            auto noDraw = new Qt3DRender::QNoDraw(m_blitFramebufferNodeFromFBO0ToFBO1);
            Q_UNUSED(noDraw);
        }

        // create a node to hold all the effect subtrees, under the main viewport. They don't need camera, alpha, frustum, etc.
        m_effectsViewport->setParent(this);
        m_effectsRootNode = new Qt3DRender::QFrameGraphNode(m_effectsViewport);
        m_effectsRootNode->setObjectName(QStringLiteral("KuesaPostProcessingEffects"));

        // Gather the different effect types
        int previousRenderTargetIndex = 0;
        Qt3DRender::QFrameGraphNode *lastEffectFG = nullptr;

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
            effect->setSceneSize(surfaceSize);

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
            lastEffectFG = m_effectFGSubtrees.value(effect).data();
            lastEffectFG->setParent(effectParentNode);

            previousRenderTargetIndex = currentRenderTargetIndex;
        }
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setParent(this);
#endif

    const bool blocked = blockNotifications(true);
    emit frameGraphTreeReconfigured();
    blockNotifications(blocked);
}

// Stages are the FrameGraph sub branches required for the rendering
// of the actual gltf scene
void ForwardRenderer::reconfigureStages()
{
    // The total number of stages me need is:
    // No layers
    //    2 (culled, non culled) to handle the non layered elements
    // Layers
    //    2 (culled, non culled) per layer
    const auto actualSceneStagesCount = m_sceneStages.size();
    const bool hasLayers = m_layers.size() > 0;
    const int requiredSceneStagesCount = std::max(2, int(m_layers.size()) * 2);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setParent(Q_NODE_NULLPTR);
#endif

    m_sceneStages.resize(requiredSceneStagesCount);
    for (auto i = actualSceneStagesCount; i < requiredSceneStagesCount; ++i) {
        SceneStagesPtr &sceneStage = m_sceneStages[i];
        sceneStage = SceneStagesPtr::create();
        sceneStage->init();
    }

    // Update SceneStages and unparent
    for (int i = 0; i < requiredSceneStagesCount; ++i) {
        Qt3DRender::QLayer *layer = nullptr;
        Qt3DCore::QEntity *camera = m_camera;
        QRectF viewport = m_viewport;
        if (hasLayers) {
            const auto &layerConfig = m_layers[size_t(i / 2)];
            layer = layerConfig.m_layer;
            if (layerConfig.m_camera)
                camera = layerConfig.m_camera;
            if (layerConfig.m_viewport.isValid())
                viewport = layerConfig.m_viewport;
        }

        SceneStagesPtr &sceneStage = m_sceneStages[i];
        sceneStage->setZFilling(m_zfilling);
        sceneStage->reconfigure(layer, camera, viewport);
        sceneStage->setBackToFrontSorting(m_backToFrontSorting);
        sceneStage->unParent();
    }

    // Insert ZFilling/Opaque branches
    // Root Nodes into which we have to perform the Render Stages to draw the glTF Scene
    const std::pair<Qt3DRender::QFrameGraphNode *, Qt3DRender::QFrameGraphNode *> stageRoots[] = {
        { m_frustumCullingOpaqueTechniqueFilter, m_frustumCullingTransparentTechniqueFilter },
        { m_noFrustumCullingOpaqueTechniqueFilter, m_noFrustumCullingTransparentTechniqueFilter }
    };

    for (int i = 0; i < requiredSceneStagesCount;) {
        for (const auto &rootPair : stageRoots) {
            SceneStagesPtr &sceneStage = m_sceneStages[i++];
            sceneStage->setParent(std::get<0>(rootPair), std::get<1>(rootPair));
        }
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_debugOverlay->setParent(this);
#endif
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

bool ForwardRenderer::hasHalfFloatRenderable()
{
    return _glFeatures && _glFeatures->hasHalfFloatRenderable;
}

/*!
 * \internal
 *
 * Helper functionto return the current render surface size
 */
QSize ForwardRenderer::currentSurfaceSize() const
{
    QSize size = externalRenderTargetSize();

    if (!size.isValid()) {
        auto currentSurface = surface();
        if (auto window = qobject_cast<QWindow *>(currentSurface))
            size = window->size() * window->screen()->devicePixelRatio();
    }

    return size;
}

void ForwardRenderer::scheduleFGTreeRebuild()
{
    if (!m_fgTreeRebuiltScheduled) {
        m_fgTreeRebuiltScheduled = true;
        QMetaObject::invokeMethod(this, &ForwardRenderer::rebuildFGTree, Qt::QueuedConnection);
    }
}

void ForwardRenderer::rebuildFGTree()
{
    m_fgTreeRebuiltScheduled = false;
    // Reconfigure FrameGraph Tree
    reconfigureFrameGraph();
    reconfigureStages();
}

/*!
 * Sets the surface where rendering will occur to \a renderSurface. This can be
 * an offscreen surface or a window surface.
 */
void ForwardRenderer::setRenderSurface(QObject *renderSurface)
{
    setSurface(renderSurface);
}

/*!
 * Sets the dimensions of the viewport where rendering will occur to \a
 * viewportRect. Dimensions are in normalized coordinates.
 */
void ForwardRenderer::setViewportRect(const QRectF &viewportRect)
{
    if (m_viewport != viewportRect) {
        m_viewport = viewportRect;
        m_effectsViewport->setNormalizedRect(viewportRect);
        reconfigureStages();
    }
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
    delete m_opaqueStagesConfiguration.m_root;
    delete m_transparentStagesConfiguration.m_root;
}

/*!
    \internal
 */
void ForwardRenderer::SceneStages::init()
{
    m_opaqueStagesConfiguration.m_root = new Qt3DRender::QLayerFilter();
    m_opaqueStagesConfiguration.m_cameraSelector = new Qt3DRender::QCameraSelector(m_opaqueStagesConfiguration.m_root);
    m_opaqueStagesConfiguration.m_viewport = new Qt3DRender::QViewport(m_opaqueStagesConfiguration.m_cameraSelector);
    m_transparentStagesConfiguration.m_root = new Qt3DRender::QLayerFilter();
    m_transparentStagesConfiguration.m_cameraSelector = new Qt3DRender::QCameraSelector(m_transparentStagesConfiguration.m_root);
    m_transparentStagesConfiguration.m_viewport = new Qt3DRender::QViewport(m_transparentStagesConfiguration.m_cameraSelector);
    m_opaqueStage = new OpaqueRenderStage(m_opaqueStagesConfiguration.m_viewport);
    m_transparentStage = new TransparentRenderStage(m_transparentStagesConfiguration.m_viewport);
    m_opaqueStagesConfiguration.m_root->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);
    m_transparentStagesConfiguration.m_root->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);
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
                                                          m_zFillStage, [this] { m_zFillStage = nullptr; });
            m_opaqueStage->setParent(Q_NODE_NULLPTR);
            m_zFillStage->setParent(m_opaqueStagesConfiguration.m_viewport);
            m_opaqueStage->setParent(m_opaqueStagesConfiguration.m_viewport);
        }
        m_opaqueStage->setZFilling(zFilling);
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
void ForwardRenderer::SceneStages::reconfigure(Qt3DRender::QLayer *l, Qt3DCore::QEntity *camera, QRectF viewport)
{
    Qt3DRender::QLayer *oldLayer = layer();

    if (oldLayer) {
        m_opaqueStagesConfiguration.m_root->removeLayer(oldLayer);
        m_opaqueStagesConfiguration.m_root->setObjectName(QLatin1String("Opaque Stages Root"));
        m_transparentStagesConfiguration.m_root->removeLayer(oldLayer);
        m_transparentStagesConfiguration.m_root->setObjectName(QLatin1String("Transparent Stages Root"));
    }

    if (l) {
        m_opaqueStagesConfiguration.m_root->addLayer(l);
        m_opaqueStagesConfiguration.m_root->setObjectName(QString(QLatin1String("Opaque Stages Root (%1)")).arg(l->objectName()));
        m_transparentStagesConfiguration.m_root->addLayer(l);
        m_transparentStagesConfiguration.m_root->setObjectName(QString(QLatin1String("Transparent Stages Root (%1)")).arg(l->objectName()));
    }

    m_opaqueStagesConfiguration.m_root->setEnabled(l != nullptr);
    m_transparentStagesConfiguration.m_root->setEnabled(l != nullptr);

    m_opaqueStagesConfiguration.m_cameraSelector->setCamera(camera);
    m_transparentStagesConfiguration.m_cameraSelector->setCamera(camera);

    m_opaqueStagesConfiguration.m_viewport->setNormalizedRect(viewport);
    m_transparentStagesConfiguration.m_viewport->setNormalizedRect(viewport);
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

Qt3DRender::QLayer *ForwardRenderer::SceneStages::layer() const
{
    if (m_opaqueStagesConfiguration.m_root->layers().size() > 0)
        return m_opaqueStagesConfiguration.m_root->layers().first();
    return nullptr;
}

/*!
    \internal
 */
void ForwardRenderer::SceneStages::unParent()
{
    m_opaqueStagesConfiguration.m_root->setParent(Q_NODE_NULLPTR);
    m_transparentStagesConfiguration.m_root->setParent(Q_NODE_NULLPTR);
}

/*!
    \internal
 */
void ForwardRenderer::SceneStages::setParent(Qt3DCore::QNode *opaqueRoot,
                                             Qt3DCore::QNode *transparentRoot)
{
    m_opaqueStagesConfiguration.m_root->setParent(opaqueRoot);
    m_transparentStagesConfiguration.m_root->setParent(transparentRoot);
}

/*!
    \internal
 */
Qt3DRender::QLayerFilter *ForwardRenderer::SceneStages::opaqueStagesRoot() const
{
    return m_opaqueStagesConfiguration.m_root;
}

/*!
    \internal
 */
Qt3DRender::QLayerFilter *ForwardRenderer::SceneStages::transparentStagesRoot() const
{
    return m_transparentStagesConfiguration.m_root;
}

/*!
    \internal
 */
OpaqueRenderStage *ForwardRenderer::SceneStages::opaqueStage() const
{
    return m_opaqueStage;
}

/*!
    \internal
 */
ZFillRenderStage *ForwardRenderer::SceneStages::zFillStage() const
{
    return m_zFillStage;
}

/*!
    \internal
 */
TransparentRenderStage *ForwardRenderer::SceneStages::transparentStage() const
{
    return m_transparentStage;
}

QT_END_NAMESPACE
