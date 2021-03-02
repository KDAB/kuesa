/*
    forwardrenderer.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Qt3DRender/qdebugoverlay.h>
#include <private/qrhi_p.h>
#include <QWindow>
#include <QScreen>
#include <Qt3DRender/qnodraw.h>
#include <Kuesa/private/viewresolver_p.h>
#include "kuesa_p.h"

#include <Qt3DCore/private/qnode_p.h>
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
    \property Kuesa::ForwardRenderer::clearBuffers

    Holds which buffers will be cleared at the start of each frame.
*/

/*!
    \qmlproperty enumeration Kuesa::ForwardRenderer::clearBuffers

    Holds which buffers will be cleared at the start of each frame.
*/

ForwardRenderer::ForwardRenderer(Qt3DCore::QNode *parent)
    : View(parent)
    , m_surfaceSelector(new Qt3DRender::QRenderSurfaceSelector(this))
    , m_clearBuffers(new Qt3DRender::QClearBuffers())
    , m_defaultViewHolder(new Qt3DRender::QFrameGraphNode())
    , m_viewsHolder(new Qt3DRender::QFrameGraphNode())
    , m_rhiViewResolver(new Qt3DRender::QFrameGraphNode())
    , m_debugOverlay(new Qt3DRender::QDebugOverlay)
{
    m_surfaceSelector->setObjectName(QStringLiteral("ForwardRenderer RenderSurfaceSelector"));

    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthStencilBuffer);
    m_clearBuffers->setObjectName(QStringLiteral("ForwardRenderer ClearBuffers"));
    new Qt3DRender::QNoDraw(m_clearBuffers);

    connect(this, &View::shadowMapsChanged, this, [this](const QVector<ShadowMapPtr> &shadowMaps) {
        for (auto view : m_views)
            view->setShadowMaps(shadowMaps);
        scheduleFGTreeRebuild();
    });
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::clearColorChanged, this, &ForwardRenderer::clearColorChanged);
    connect(m_clearBuffers, &Qt3DRender::QClearBuffers::buffersChanged, this, &ForwardRenderer::clearBuffersChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::renderSurfaceChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::surfaceChanged, this, &ForwardRenderer::handleSurfaceChange);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::externalRenderTargetSizeChanged);
    connect(m_surfaceSelector, &Qt3DRender::QRenderSurfaceSelector::externalRenderTargetSizeChanged, this, &ForwardRenderer::updateTextureSizes);
    m_debugOverlay->setEnabled(false);
    // Add a NoDraw to the debug overlay FG branch to ensure nothing but the
    // overlay will be drawn
    new Qt3DRender::QNoDraw(m_debugOverlay);
    connect(m_debugOverlay, &Qt3DRender::QDebugOverlay::enabledChanged, this, &ForwardRenderer::showDebugOverlayChanged);

    rebuildFGTree();
}

ForwardRenderer::~ForwardRenderer()
{
    if (!m_viewsHolder->parent())
        delete m_viewsHolder;
    if (!m_defaultViewHolder->parent())
        delete m_defaultViewHolder;
    if (!m_rhiViewResolver->parent())
        delete m_rhiViewResolver;
}

/*!
 * Returns which buffers are cleared at the start of each frame.
 */
Qt3DRender::QClearBuffers::BufferType ForwardRenderer::clearBuffers() const
{
    return m_clearBuffers->buffers();
}

bool ForwardRenderer::showDebugOverlay() const
{
    return m_debugOverlay->isEnabled();
}

/*!
 * Returns the surface where rendering will occur.
 */
QObject *ForwardRenderer::renderSurface() const
{
    return m_surfaceSelector->surface();
}

/*!
 * Sets which \a clearBuffers will be cleared at the start of each frame.
 */
void ForwardRenderer::setClearBuffers(Qt3DRender::QClearBuffers::BufferType clearBuffers)
{
    m_clearBuffers->setBuffers(clearBuffers);
}

void ForwardRenderer::setShowDebugOverlay(bool showDebugOverlay)
{
    m_debugOverlay->setEnabled(showDebugOverlay);
}

void ForwardRenderer::addView(View *view)
{
    if (!view->parent())
        view->setParent(this);

    auto d = Qt3DCore::QNodePrivate::get(this);
    d->registerDestructionHelper(view, &ForwardRenderer::removeView, m_views);

    view->setShadowMaps(shadowMaps());
    view->setSurfaceSize(m_surfaceSize);

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
    qDebug() << qPrintable(Qt3DRender::QFrameGraphNodePrivate::get(this)->dumpFrameGraph());
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
 */
void ForwardRenderer::updateTextureSizes()
{
    const QSize surfaceSize = currentSurfaceSize();
    for (View *v : m_views)
        v->setSurfaceSize(surfaceSize);
    View::setSurfaceSize(surfaceSize);
}

/*!
 * \internal
 *
 * Rebuild FrameGraph tree based on selected effects in the correct order.
 */
void ForwardRenderer::reconfigureFrameGraph()
{
    m_clearBuffers->setParent(Q_NODE_NULLPTR);
    m_defaultViewHolder->setParent(Q_NODE_NULLPTR);
    m_viewsHolder->setParent(Q_NODE_NULLPTR);
    m_debugOverlay->setParent(Q_NODE_NULLPTR);
    m_rhiViewResolver->setParent(Q_NODE_NULLPTR);

    // Unparent previous View Stages
    for (Qt3DCore::QNode *c : m_viewsHolder->childNodes())
        c->setParent(Q_NODE_NULLPTR);

    // Rebuild FG Tree
    m_clearBuffers->setParent(m_surfaceSelector);

    if (m_views.empty()) {
        View::reconfigureFrameGraphHelper(m_defaultViewHolder);
        m_defaultViewHolder->setParent(m_surfaceSelector);
    } else {
        for (View *v : m_views)
            v->setParent(m_viewsHolder);
        m_viewsHolder->setParent(m_surfaceSelector);
    }

    // When using RHI, we need a dedicated pass to render
    // FBO associated with each view back to the main surface
    if (m_fg->m_usesRHI) {
        for (Qt3DCore::QNode *c : m_rhiViewResolver->childNodes())
            c->setParent(Q_NODE_NULLPTR);

        const std::vector<View *> &views = (m_views.empty()) ? std::vector<View *>({ this }) : m_views;
        const size_t viewCount = views.size();

        while (m_viewRenderers.size() > viewCount) {
            delete m_viewRenderers.back();
            m_viewRenderers.pop_back();
        }
        while (m_viewRenderers.size() < viewCount)
            m_viewRenderers.push_back(new ViewResolver());

        for (size_t i = 0; i < viewCount; ++i) {
            View *v = views[i];
            ViewResolver *resolver = m_viewRenderers[i];
            resolver->setView(v);
            resolver->setParent(m_rhiViewResolver);
        }
        m_rhiViewResolver->setParent(m_surfaceSelector);
    }

    m_debugOverlay->setParent(m_surfaceSelector);

    const bool blocked = blockNotifications(true);
    emit frameGraphTreeReconfigured();
    blockNotifications(blocked);

    //    dump();
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
