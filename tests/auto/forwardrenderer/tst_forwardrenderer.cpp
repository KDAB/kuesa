/*
    tst_forwardrenderer.cpp

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

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <Kuesa/forwardrenderer.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/private/opaquerenderstage_p.h>
#include <Kuesa/private/zfillrenderstage_p.h>
#include <Kuesa/private/transparentrenderstage_p.h>
#include <Kuesa/private/scenestages_p.h>
#include <Kuesa/private/reflectionstages_p.h>
#include <Kuesa/private/framegraphutils_p.h>
#include <Kuesa/private/effectsstages_p.h>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QBlitFramebuffer>
#include <Qt3DRender/QDebugOverlay>
#include <Qt3DRender/QNoDraw>
#include <QWindow>
#include <QSurfaceFormat>
#include <QOffscreenSurface>

namespace {

class tst_FX : public Kuesa::AbstractPostProcessingEffect
{
    Q_OBJECT

public:
    explicit tst_FX(Qt3DCore::QNode *parent = nullptr)
        : Kuesa::AbstractPostProcessingEffect(parent)
        , m_rootNode(new Qt3DRender::QFrameGraphNode())
        , m_inputTexture(nullptr)
    {
    }

    // AbstractPostProcessingEffect interface
    FrameGraphNodePtr frameGraphSubTree() const override
    {
        return m_rootNode;
    }

    QVector<Qt3DRender::QLayer *> layers() const override
    {
        return QVector<Qt3DRender::QLayer *>();
    }

    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override
    {
        m_inputTexture = texture;
    }

    Qt3DRender::QAbstractTexture *inputTexture() const
    {
        return m_inputTexture;
    }

    void setWindowSize(const QSize &size) override
    {
        emit sceneSizeChanged(size);
    }

Q_SIGNALS:
    void sceneSizeChanged(const QSize &size);

private:
    FrameGraphNodePtr m_rootNode;
    Qt3DRender::QAbstractTexture *m_inputTexture;
};

} // namespace

class tst_ForwardRenderer : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testAllocAndDestruction()
    {
        // GIVEN
        {
            Kuesa::ForwardRenderer r;
        }
        // THEN -> Should not crash
    }

    void testChangingViewportRect()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        QSignalSpy spy(&renderer, SIGNAL(viewportRectChanged(const QRectF &)));
        QVERIFY(spy.isValid());

        // WHEN
        QRectF newRect(0, 0, 100, 200);
        renderer.setViewportRect(newRect);

        // THEN
        QCOMPARE(renderer.viewportRect(), newRect);
        QCOMPARE(spy.count(), 1);

        // WHEN
        renderer.setViewportRect(newRect);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testChangingExternalRenderTargetSize()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        Qt3DRender::QRenderSurfaceSelector *surfaceSelector = renderer.m_surfaceSelector;

        QSignalSpy spy(&renderer, &Kuesa::ForwardRenderer::externalRenderTargetSizeChanged);
        QVERIFY(spy.isValid());

        // WHEN
        QSize newSize(123, 456);
        surfaceSelector->setExternalRenderTargetSize(newSize);

        // THEN
        QCOMPARE(surfaceSelector->externalRenderTargetSize(), newSize);
        QCOMPARE(spy.count(), 1);

        // WHEN
        surfaceSelector->setExternalRenderTargetSize(newSize);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testChangingCamera()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        qRegisterMetaType<Qt3DCore::QEntity *>("QEntity");
        QSignalSpy spy(&renderer, SIGNAL(cameraChanged(Qt3DCore::QEntity *)));
        QVERIFY(spy.isValid());

        // WHEN
        Qt3DRender::QCamera newCamera;
        renderer.setCamera(&newCamera);

        // THEN
        QCOMPARE(renderer.camera(), &newCamera);
        QCOMPARE(spy.count(), 1);

        // WHEN
        renderer.setCamera(&newCamera);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testChangingClearColor()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        QSignalSpy spy(&renderer, SIGNAL(clearColorChanged(const QColor &)));
        QVERIFY(spy.isValid());

        // WHEN
        QColor newColor = Qt::blue;
        renderer.setClearColor(newColor);

        // THEN
        QCOMPARE(renderer.clearColor(), newColor);
        QCOMPARE(spy.count(), 1);

        // WHEN
        renderer.setClearColor(newColor);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testChangingClearBuffers()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        qRegisterMetaType<Qt3DRender::QClearBuffers::BufferType>("BufferType");
        QSignalSpy spy(&renderer, SIGNAL(clearBuffersChanged(Qt3DRender::QClearBuffers::BufferType)));

        // WHEN
        Qt3DRender::QClearBuffers::BufferType newBuffers(Qt3DRender::QClearBuffers::StencilBuffer);
        renderer.setClearBuffers(newBuffers);

        // THEN
        QCOMPARE(renderer.clearBuffers(), newBuffers);
        QCOMPARE(spy.count(), 1);

        // WHEN
        renderer.setClearBuffers(newBuffers);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testRenderingStyleFilterIsForward()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;

        // THEN
        Qt3DRender::QTechniqueFilter *filterNode = renderer.findChild<Qt3DRender::QTechniqueFilter *>();
        QVERIFY(filterNode);

        auto filters = filterNode->matchAll();
        QVERIFY(filters.count() > 0);

        auto findForwardRenderingFilterKey = [](Qt3DRender::QFilterKey *filterkey) {
            return filterkey->name() == QStringLiteral("renderingStyle") && filterkey->value().toString() == QStringLiteral("forward");
        };
        auto it = std::find_if(filters.begin(), filters.end(), findForwardRenderingFilterKey);
        QVERIFY(it != filters.end());
    }

    void testAddView()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;

        {
            Kuesa::View v;

            // WHEN
            f.addView(&v);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(f.views().size(), 1U);
            QCOMPARE(f.views().front(), &v);
        }
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(f.views().size(), 0U);
    }

    void testRemoveView()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;
        Kuesa::View v;

        // WHEN
        f.addView(&v);
        QCoreApplication::processEvents();

        f.removeView(&v);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(f.views().size(), 0U);
    }

    void testSetupRenderTargetsNoFXNoMSAA()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;
        Qt3DRender::QRenderTargetSelector rtSelector;

        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
        format.setSamples(1);
        QSurfaceFormat::setDefaultFormat(format);

        delete f.m_fg->m_multisampleTarget;
        delete f.m_fg->m_renderTargets[0];
        delete f.m_fg->m_renderTargets[1];
        f.m_fg->m_multisampleTarget = nullptr;
        f.m_fg->m_renderTargets[0] = nullptr;
        f.m_fg->m_renderTargets[1] = nullptr;

        // WHEN -> No Stencil, No MSAA, No FX
        f.m_fg->setupRenderTargets(&rtSelector, 0);

        // THEN
        QVERIFY(f.m_fg->m_renderTargets[0] != nullptr);
        QVERIFY(f.m_fg->m_renderTargets[1] == nullptr);
        QVERIFY(f.m_fg->m_multisampleTarget == nullptr);
        QVERIFY(f.m_fg->m_rt0rt1Resolver == nullptr);
        QVERIFY(f.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1 == nullptr);

        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[0],
                         Qt3DRender::QRenderTargetOutput::Depth),
                 true);

        // THEN
        QCOMPARE(rtSelector.target(), f.m_fg->m_renderTargets[0]);
        QCOMPARE(rtSelector.children().size(), 0);
    }

    void testSetupRenderTargetsNoFXMSAA()
    {
        if (!Kuesa::FrameGraphUtils::hasMSAASupport())
            QSKIP("Device has no MSAA support, skipping");

        Kuesa::ForwardRenderer f;
        Qt3DRender::QRenderTargetSelector rtSelector;

        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
        format.setSamples(4);
        QSurfaceFormat::setDefaultFormat(format);

        delete f.m_fg->m_multisampleTarget;
        delete f.m_fg->m_renderTargets[0];
        delete f.m_fg->m_renderTargets[1];
        f.m_fg->m_multisampleTarget = nullptr;
        f.m_fg->m_renderTargets[0] = nullptr;
        f.m_fg->m_renderTargets[1] = nullptr;

        // WHEN -> No Stencil, MSAA, No FX
        f.m_fg->setupRenderTargets(&rtSelector, 0);

        QVERIFY(f.m_fg->m_renderTargets[0] != nullptr);
        QVERIFY(f.m_fg->m_renderTargets[1] == nullptr);
        QVERIFY(f.m_fg->m_multisampleTarget != nullptr);
        QVERIFY(f.m_fg->m_rt0rt1Resolver == nullptr);
        QVERIFY(f.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1 == nullptr);

        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[0],
                         Qt3DRender::QRenderTargetOutput::Depth),
                 true);
        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_multisampleTarget,
                         Qt3DRender::QRenderTargetOutput::Depth),
                 true);

        QCOMPARE(rtSelector.target(), f.m_fg->m_multisampleTarget);
        QCOMPARE(rtSelector.children().size(), 1); // Blit of MSAAFBOResolver
        QVERIFY(qobject_cast<Qt3DRender::QFrameGraphNode *>(rtSelector.children()[0]));
    }

    void testSetupRenderTargetsFXNoMSAA()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;
        Qt3DRender::QRenderTargetSelector rtSelector;

        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
        format.setSamples(1);
        QSurfaceFormat::setDefaultFormat(format);

        delete f.m_fg->m_multisampleTarget;
        delete f.m_fg->m_renderTargets[0];
        delete f.m_fg->m_renderTargets[1];
        f.m_fg->m_multisampleTarget = nullptr;
        f.m_fg->m_renderTargets[0] = nullptr;
        f.m_fg->m_renderTargets[1] = nullptr;

        // WHEN -> No Stencil, No MSAA, FX(1)
        f.m_fg->setupRenderTargets(&rtSelector, 1);

        // THEN
        QVERIFY(f.m_fg->m_renderTargets[0] != nullptr);
        QVERIFY(f.m_fg->m_renderTargets[1] == nullptr);
        QVERIFY(f.m_fg->m_multisampleTarget == nullptr);
        QVERIFY(f.m_fg->m_rt0rt1Resolver == nullptr);
        QVERIFY(f.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1 == nullptr);

        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[0],
                         Qt3DRender::QRenderTargetOutput::Depth),
                 true);

        // THEN
        QCOMPARE(rtSelector.target(), f.m_fg->m_renderTargets[0]);
        QCOMPARE(rtSelector.children().size(), 0);
    }

    void testSetupRenderTargetsFXsNoMSAA()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;
        Qt3DRender::QRenderTargetSelector rtSelector;

        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
        format.setSamples(1);
        QSurfaceFormat::setDefaultFormat(format);

        delete f.m_fg->m_multisampleTarget;
        delete f.m_fg->m_renderTargets[0];
        delete f.m_fg->m_renderTargets[1];
        f.m_fg->m_multisampleTarget = nullptr;
        f.m_fg->m_renderTargets[0] = nullptr;
        f.m_fg->m_renderTargets[1] = nullptr;

        // WHEN -> No Stencil, No MSAA, FX(2)
        f.m_fg->setupRenderTargets(&rtSelector, 2);

        // THEN
        QVERIFY(f.m_fg->m_renderTargets[0] != nullptr);
        QVERIFY(f.m_fg->m_renderTargets[1] != nullptr);
        QVERIFY(f.m_fg->m_multisampleTarget == nullptr);
        if (f.m_fg->m_usesRHI)
            QVERIFY(f.m_fg->m_rt0rt1Resolver != nullptr);
        else
            QVERIFY(f.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1 != nullptr);

        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[0],
                         Qt3DRender::QRenderTargetOutput::Depth),
                 true);
        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[1],
                         Qt3DRender::QRenderTargetOutput::Depth),
                 false);

        // THEN
        QCOMPARE(rtSelector.target(), f.m_fg->m_renderTargets[0]);
        QCOMPARE(rtSelector.children().size(), 1);
        Qt3DRender::QBlitFramebuffer *blit = qobject_cast<Qt3DRender::QBlitFramebuffer *>(rtSelector.children()[0]);
        QVERIFY(blit != nullptr);
        QCOMPARE(blit->children().size(), 1);
        QVERIFY(qobject_cast<Qt3DRender::QNoDraw *>(blit->children().first()) != nullptr);
    }

    void testSetupRenderTargetsFXsNoMSAAStencil()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;
        Qt3DRender::QRenderTargetSelector rtSelector;

        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
        format.setSamples(1);
        QSurfaceFormat::setDefaultFormat(format);

        f.m_usesStencilMask = true;

        delete f.m_fg->m_multisampleTarget;
        delete f.m_fg->m_renderTargets[0];
        delete f.m_fg->m_renderTargets[1];
        f.m_fg->m_multisampleTarget = nullptr;
        f.m_fg->m_renderTargets[0] = nullptr;
        f.m_fg->m_renderTargets[1] = nullptr;

        // WHEN -> Stencil, No MSAA, FX(2)
        f.m_fg->setupRenderTargets(&rtSelector, 2);

        // THEN
        QVERIFY(f.m_fg->m_renderTargets[0] != nullptr);
        QVERIFY(f.m_fg->m_renderTargets[1] != nullptr);
        QVERIFY(f.m_fg->m_multisampleTarget == nullptr);
        if (f.m_fg->m_usesRHI)
            QVERIFY(f.m_fg->m_rt0rt1Resolver != nullptr);
        else
            QVERIFY(f.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1 != nullptr);

        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[0],
                         Qt3DRender::QRenderTargetOutput::DepthStencil),
                 true);
        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[1],
                         Qt3DRender::QRenderTargetOutput::DepthStencil),
                 false);

        // THEN
        QCOMPARE(rtSelector.target(), f.m_fg->m_renderTargets[0]);
        QCOMPARE(rtSelector.children().size(), 1);
        Qt3DRender::QBlitFramebuffer *blit = qobject_cast<Qt3DRender::QBlitFramebuffer *>(rtSelector.children()[0]);
        QVERIFY(blit != nullptr);
        QCOMPARE(blit->children().size(), 1);
        QVERIFY(qobject_cast<Qt3DRender::QNoDraw *>(blit->children().first()) != nullptr);
    }

    void testSetupRenderTargetsFXsMSAAStencil()
    {
        if (!Kuesa::FrameGraphUtils::hasMSAASupport())
            QSKIP("Device has no MSAA support, skipping");

        Kuesa::ForwardRenderer f;
        Qt3DRender::QRenderTargetSelector rtSelector;

        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
        format.setSamples(4);
        QSurfaceFormat::setDefaultFormat(format);

        f.m_usesStencilMask = true;

        delete f.m_fg->m_multisampleTarget;
        delete f.m_fg->m_renderTargets[0];
        delete f.m_fg->m_renderTargets[1];
        f.m_fg->m_multisampleTarget = nullptr;
        f.m_fg->m_renderTargets[0] = nullptr;
        f.m_fg->m_renderTargets[1] = nullptr;

        // WHEN -> Stencil, MSAA, FX(2)
        f.m_fg->setupRenderTargets(&rtSelector, 2);

        QVERIFY(f.m_fg->m_renderTargets[0] != nullptr);
        QVERIFY(f.m_fg->m_renderTargets[1] != nullptr);
        QVERIFY(f.m_fg->m_multisampleTarget != nullptr);
        if (f.m_fg->m_usesRHI)
            QVERIFY(f.m_fg->m_rt0rt1Resolver != nullptr);
        else
            QVERIFY(f.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1 != nullptr);

        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[0],
                         Qt3DRender::QRenderTargetOutput::DepthStencil),
                 true);
        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_multisampleTarget,
                         Qt3DRender::QRenderTargetOutput::DepthStencil),
                 true);
        QCOMPARE(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(
                         f.m_fg->m_renderTargets[1],
                         Qt3DRender::QRenderTargetOutput::DepthStencil),
                 false);

        QCOMPARE(rtSelector.target(), f.m_fg->m_multisampleTarget);
        QCOMPARE(rtSelector.children().size(), 2); // Blit of MSAAFBOResolver
        QVERIFY(qobject_cast<Qt3DRender::QFrameGraphNode *>(rtSelector.children()[0]));
        Qt3DRender::QBlitFramebuffer *blit = qobject_cast<Qt3DRender::QBlitFramebuffer *>(rtSelector.children()[1]);
        QVERIFY(blit != nullptr);
        QCOMPARE(blit->children().size(), 1);
        QVERIFY(qobject_cast<Qt3DRender::QNoDraw *>(blit->children().first()) != nullptr);
    }

    void testReconfigureFrameGraphNoViewNoFX()
    {
        // GIVEN (No View, No Fx)
        Kuesa::ForwardRenderer f;

        Qt3DRender::QRenderSurfaceSelector *surfaceSelector = f.m_surfaceSelector;
        Qt3DRender::QRenderTargetSelector *renderToTexture = f.m_fg->m_renderToTextureRootNode;
        Qt3DRender::QClearBuffers *clearSurface = f.m_clearBuffers;
        Qt3DRender::QClearBuffers *clearRenderTarget = f.m_fg->m_clearRT0;
        Qt3DRender::QLayerFilter *sceneLayerFilter = f.m_fg->m_mainSceneLayerFilter;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        Qt3DRender::QDebugOverlay *overlay = f.m_debugOverlay;
#endif

        // THEN
        QCOMPARE(surfaceSelector->parent(), &f);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children().size(), 4);
#else
        QCOMPARE(surfaceSelector->children().size(), 3);
#endif

        QCOMPARE(surfaceSelector->children()[0], clearSurface);
        QCOMPARE(surfaceSelector->children()[1], renderToTexture);
        QCOMPARE(surfaceSelector->children()[2], f.m_internalFXStages);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children()[3], overlay);
#endif

        QVERIFY(renderToTexture->children().size() >= 2);
        QCOMPARE(renderToTexture->children()[0], clearRenderTarget);
        QCOMPARE(renderToTexture->children()[1], sceneLayerFilter);

        // Single SceneStages
        QCOMPARE(sceneLayerFilter->children().size(), 1);
    }

    void testReconfigureFrameGraphNoViewFX()
    {
        // GIVEN (No View, FX)
        Kuesa::ForwardRenderer f;
        tst_FX fx;

        Qt3DRender::QRenderSurfaceSelector *surfaceSelector = f.m_surfaceSelector;
        Qt3DRender::QRenderTargetSelector *renderToTexture = f.m_fg->m_renderToTextureRootNode;
        Qt3DRender::QClearBuffers *clearSurface = f.m_clearBuffers;
        Qt3DRender::QClearBuffers *clearRenderTarget = f.m_fg->m_clearRT0;
        Qt3DRender::QLayerFilter *sceneLayerFilter = f.m_fg->m_mainSceneLayerFilter;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        Qt3DRender::QDebugOverlay *overlay = f.m_debugOverlay;
#endif

        // WHEN
        f.addPostProcessingEffect(&fx);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(surfaceSelector->parent(), &f);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children().size(), 5);
#else
        QCOMPARE(surfaceSelector->children().size(), 3);
#endif

        QCOMPARE(surfaceSelector->children()[0], clearSurface);
        QCOMPARE(surfaceSelector->children()[1], renderToTexture);
        QCOMPARE(surfaceSelector->children()[2], f.m_fxStages);
        QCOMPARE(surfaceSelector->children()[3], f.m_internalFXStages);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children()[4], overlay);
#endif

        QVERIFY(renderToTexture->children().size() >= 2);
        QCOMPARE(renderToTexture->children()[0], clearRenderTarget);
        QCOMPARE(renderToTexture->children()[1], sceneLayerFilter);

        // Single SceneStages
        QCOMPARE(sceneLayerFilter->children().size(), 1);
    }

    void testFrameGraphViewsNoFX()
    {
        // GIVEN (Views, No FX)
        Kuesa::ForwardRenderer f;
        Kuesa::View v1;
        Kuesa::View v2;

        // WHEN
        f.addView(&v1);
        f.addView(&v2);

        Qt3DRender::QRenderSurfaceSelector *surfaceSelector = f.m_surfaceSelector;
        Qt3DRender::QClearBuffers *clearSurface = f.m_clearBuffers;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        Qt3DRender::QDebugOverlay *overlay = f.m_debugOverlay;
#endif

        // THEN
        QCOMPARE(surfaceSelector->parent(), &f);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children().size(), 4);
#else
        QCOMPARE(surfaceSelector->children().size(), 5);
#endif

        QCOMPARE(surfaceSelector->children()[0], clearSurface);
        QCOMPARE(surfaceSelector->children()[1], &v1);
        QCOMPARE(surfaceSelector->children()[2], &v2);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children()[3], overlay);
#endif

        if (Kuesa::FrameGraphUtils::hasMSAASupport()) {
            QCOMPARE(v1.children().size(), 4);
            QCOMPARE(v1.children()[0], v1.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v1.children()[1], v1.m_fg->m_renderTargets[0]);
            QCOMPARE(v1.children()[2], v1.m_fg->m_multisampleTarget);
            QCOMPARE(v1.children()[3], v1.m_internalFXStages);
            QCOMPARE(v2.children().size(), 4);
            QCOMPARE(v2.children()[0], v2.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v2.children()[1], v2.m_fg->m_renderTargets[0]);
            QCOMPARE(v2.children()[2], v2.m_fg->m_multisampleTarget);
            QCOMPARE(v2.children()[3], v2.m_internalFXStages);

            QVERIFY(v1.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[0], v1.m_fg->m_clearRT0);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[1], v1.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children()[0], v1.m_sceneStages);

            QVERIFY(v2.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[0], v2.m_fg->m_clearRT0);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[1], v2.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children()[0], v2.m_sceneStages);
        } else {
            QCOMPARE(v1.children().size(), 3);
            QCOMPARE(v1.children()[0], v1.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v1.children()[1], v1.m_fg->m_renderTargets[0]);
            QCOMPARE(v1.children()[2], v1.m_internalFXStages);
            QCOMPARE(v2.children().size(), 3);
            QCOMPARE(v2.children()[0], v2.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v2.children()[1], v2.m_fg->m_renderTargets[0]);
            QCOMPARE(v2.children()[2], v2.m_internalFXStages);

            QVERIFY(v1.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[0], v1.m_fg->m_clearRT0);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[1], v1.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children()[0], v1.m_sceneStages);

            QVERIFY(v2.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[0], v2.m_fg->m_clearRT0);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[1], v2.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children()[0], v2.m_sceneStages);
        }
    }

    void testFrameGraphViewsAndFX()
    {
        // GIVEN (Views with FX)
        Kuesa::ForwardRenderer f;
        Kuesa::View v1;
        Kuesa::View v2;
        tst_FX fxV1;
        tst_FX fxV2;

        // WHEN
        f.addView(&v1);
        f.addView(&v2);

        v1.addPostProcessingEffect(&fxV1);
        v2.addPostProcessingEffect(&fxV2);

        Qt3DRender::QRenderSurfaceSelector *surfaceSelector = f.m_surfaceSelector;
        Qt3DRender::QClearBuffers *clearSurface = f.m_clearBuffers;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        Qt3DRender::QDebugOverlay *overlay = f.m_debugOverlay;
#endif

        // THEN
        QCOMPARE(surfaceSelector->parent(), &f);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children().size(), 4);
#else
        QCOMPARE(surfaceSelector->children().size(), 3);
#endif

        QCOMPARE(surfaceSelector->children()[0], clearSurface);
        QCOMPARE(surfaceSelector->children()[1], &v1);
        QCOMPARE(surfaceSelector->children()[2], &v2);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCOMPARE(surfaceSelector->children()[3], overlay);
#endif

        if (Kuesa::FrameGraphUtils::hasMSAASupport()) {
            QCOMPARE(v1.children().size(), 6);
            QCOMPARE(v1.children()[0], v1.m_fg->m_renderTargets[0]);
            QCOMPARE(v1.children()[1], v1.m_fg->m_multisampleTarget);
            QCOMPARE(v1.children()[2], v1.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v1.children()[3], v1.m_fg->m_renderTargets[1]);
            QCOMPARE(v1.children()[4], v1.m_fxStages);
            QCOMPARE(v1.children()[5], v1.m_internalFXStages);
            QCOMPARE(v2.children().size(), 6);
            QCOMPARE(v2.children()[0], v2.m_fg->m_renderTargets[0]);
            QCOMPARE(v2.children()[1], v2.m_fg->m_multisampleTarget);
            QCOMPARE(v2.children()[2], v2.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v2.children()[3], v2.m_fg->m_renderTargets[1]);
            QCOMPARE(v2.children()[4], v2.m_fxStages);
            QCOMPARE(v2.children()[5], v2.m_internalFXStages);

            QVERIFY(v1.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[0], v1.m_fg->m_clearRT0);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[1], v1.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children()[0], v1.m_sceneStages);

            QVERIFY(v2.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[0], v2.m_fg->m_clearRT0);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[1], v2.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children()[0], v2.m_sceneStages);
        } else {
            QCOMPARE(v1.children().size(), 5);
            QCOMPARE(v1.children()[0], v1.m_fg->m_renderTargets[0]);
            QCOMPARE(v1.children()[1], v1.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v1.children()[2], v1.m_fg->m_renderTargets[1]);
            QCOMPARE(v1.children()[3], v1.m_fxStages);
            QCOMPARE(v1.children()[4], v1.m_internalFXStages);
            QCOMPARE(v2.children().size(), 5);
            QCOMPARE(v2.children()[0], v2.m_fg->m_renderTargets[0]);
            QCOMPARE(v2.children()[1], v2.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v2.children()[2], v2.m_fg->m_renderTargets[1]);
            QCOMPARE(v2.children()[3], v2.m_fxStages);
            QCOMPARE(v2.children()[4], v2.m_internalFXStages);

            QVERIFY(v1.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[0], v1.m_fg->m_clearRT0);
            QCOMPARE(v1.m_fg->m_renderToTextureRootNode->children()[1], v1.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v1.m_fg->m_mainSceneLayerFilter->children()[0], v1.m_sceneStages);

            QVERIFY(v2.m_fg->m_renderToTextureRootNode->children().size() >= 2);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[0], v2.m_fg->m_clearRT0);
            QCOMPARE(v2.m_fg->m_renderToTextureRootNode->children()[1], v2.m_fg->m_mainSceneLayerFilter);

            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            QCOMPARE(v2.m_fg->m_mainSceneLayerFilter->children()[0], v2.m_sceneStages);
        }
    }

    void testHandleViewCountChanges()
    {
        // GIVEN (Views with FX)
        Kuesa::ForwardRenderer f;

        Kuesa::View v1;
        Kuesa::View v2;
        tst_FX fx;

        v1.addPostProcessingEffect(&fx);
        v2.addPostProcessingEffect(&fx);

        // WHEN -> add views
        f.addView(&v1);
        f.addView(&v2);

        // THEN
        QVERIFY(v1.parent() == f.m_surfaceSelector);
        QVERIFY(v2.parent() == f.m_surfaceSelector);
        QVERIFY(v1.m_fxStages->parent() == v1.m_fg->m_view);
        QVERIFY(v2.m_fxStages->parent() == v2.m_fg->m_view);
        QVERIFY(v1.m_fg->m_view->parent() == f.m_surfaceSelector);
        QVERIFY(v2.m_fg->m_view->parent() == f.m_surfaceSelector);

        // WHEN -> remove views
        f.removeView(&v1);

        // THEN
        QVERIFY(v1.parent() == nullptr);
        QVERIFY(v2.parent() == f.m_surfaceSelector);
        QVERIFY(v1.m_fxStages->parent() == v1.m_fg->m_view);
        QVERIFY(v1.m_fg->m_view->parent() == nullptr);
        QVERIFY(v2.m_fxStages->parent() == v2.m_fg->m_view);
        QVERIFY(v2.m_fg->m_view->parent() == f.m_surfaceSelector);

        // WHEN
        f.removeView(&v2);

        // THEN
        QVERIFY(v1.parent() == nullptr);
        QVERIFY(v2.parent() == nullptr);
        QVERIFY(v1.m_fxStages->parent() == v1.m_fg->m_view);
        QVERIFY(v1.m_fg->m_view->parent() == nullptr);
        QVERIFY(v2.m_fxStages->parent() == v2.m_fg->m_view);
        QVERIFY(v2.m_fg->m_view->parent() == nullptr);
    }

    void testRenderTargetsNoEffect()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;

        // THEN
        const auto rtPair = f.m_internalFXStages->renderTargets();
        QVERIFY(std::get<0>(rtPair) == f.m_fg->m_renderTargets[0]);
        QVERIFY(std::get<1>(rtPair) == f.m_fg->m_renderTargets[1]);
    }

    void testRenderTargetsSingleEffect()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;
        tst_FX fx;

        // WHEN
        f.addPostProcessingEffect(&fx);
        QCoreApplication::processEvents();

        // THEN
        const auto rtPairFx = f.m_fxStages->renderTargets();
        QVERIFY(std::get<0>(rtPairFx) == f.m_fg->m_renderTargets[0]);
        QVERIFY(std::get<1>(rtPairFx) == f.m_fg->m_renderTargets[1]);

        const auto rtPairInFx = f.m_internalFXStages->renderTargets();
        QVERIFY(std::get<0>(rtPairInFx) == f.m_fg->m_renderTargets[1]);
        QVERIFY(std::get<1>(rtPairInFx) == f.m_fg->m_renderTargets[0]);
    }

    void testRenderTargetsMultiEffects()
    {
        // GIVEN
        Kuesa::ForwardRenderer f;
        Kuesa::View v1;
        Kuesa::View v2;
        tst_FX *fx1 = new tst_FX();
        tst_FX *fx2 = new tst_FX();
        tst_FX *fx3 = new tst_FX();

        // WHEN
        v1.addPostProcessingEffect(fx1);
        v2.addPostProcessingEffect(fx2);
        v2.addPostProcessingEffect(fx3);

        f.addView(&v1);
        f.addView(&v2);
        QCoreApplication::processEvents();

        // THEN
        const auto rtPairFxV1 = v1.m_fxStages->renderTargets();
        QVERIFY(std::get<0>(rtPairFxV1) == v1.m_fg->m_renderTargets[0]);
        QVERIFY(std::get<1>(rtPairFxV1) == v1.m_fg->m_renderTargets[1]);

        const auto rtPairFxV2 = v2.m_fxStages->renderTargets();
        QVERIFY(std::get<0>(rtPairFxV2) == v2.m_fg->m_renderTargets[0]);
        QVERIFY(std::get<1>(rtPairFxV2) == v2.m_fg->m_renderTargets[1]);

        const auto rtPairInFxV1 = v1.m_internalFXStages->renderTargets();
        QVERIFY(std::get<0>(rtPairInFxV1) == v1.m_fg->m_renderTargets[1]);
        QVERIFY(std::get<1>(rtPairInFxV1) == v1.m_fg->m_renderTargets[0]);

        const auto rtPairInFxV2 = v2.m_internalFXStages->renderTargets();
        QVERIFY(std::get<0>(rtPairInFxV2) == v2.m_fg->m_renderTargets[0]);
        QVERIFY(std::get<1>(rtPairInFxV2) == v2.m_fg->m_renderTargets[1]);

        QVERIFY(fx1->inputTexture());
        QCOMPARE(fx1->inputTexture(), Kuesa::FrameGraphUtils::findRenderTargetTexture(v1.m_fg->m_renderTargets[0], Qt3DRender::QRenderTargetOutput::Color0));

        // THEN - fx1 is rendered into a texture which is not used for input texture for fx2.
        auto renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx1->frameGraphSubTree().data());
        QVERIFY(renderTargetSelector);

        auto outputTexture = Kuesa::FrameGraphUtils::findRenderTargetTexture(renderTargetSelector->target(), Qt3DRender::QRenderTargetOutput::Color0);
        QVERIFY(outputTexture);
        QVERIFY(outputTexture != fx2->inputTexture());
        QCOMPARE(fx2->inputTexture(), Kuesa::FrameGraphUtils::findRenderTargetTexture(v2.m_fg->m_renderTargets[0], Qt3DRender::QRenderTargetOutput::Color0));

        // THEN - fx2 is rendered into a texture which is used for input texture for fx3.
        renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx2->frameGraphSubTree().data());
        QVERIFY(renderTargetSelector);

        outputTexture = Kuesa::FrameGraphUtils::findRenderTargetTexture(renderTargetSelector->target(), Qt3DRender::QRenderTargetOutput::Color0);
        QVERIFY(outputTexture);
        QCOMPARE(outputTexture, fx3->inputTexture());
        QCOMPARE(fx3->inputTexture(), Kuesa::FrameGraphUtils::findRenderTargetTexture(v2.m_fg->m_renderTargets[1], Qt3DRender::QRenderTargetOutput::Color0));

        // THEN - fx3 is rendered into a texture which will be used as the input for the final tone mapping / gamma correction
        renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx3->frameGraphSubTree().data());
        QVERIFY(renderTargetSelector != nullptr);
    }

    void testResizingEffectsAndTextures()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        tst_FX *fx1 = new tst_FX();
        tst_FX *fx2 = new tst_FX();
        renderer.addPostProcessingEffect(fx1);
        renderer.addPostProcessingEffect(fx2);
        QCoreApplication::processEvents();

        auto renderTargetSelector = renderer.m_fg->m_renderToTextureRootNode;
        auto fx1Texture = renderTargetSelector->target()->outputs().first()->texture();

        QSignalSpy spy(fx1, &tst_FX::sceneSizeChanged);
        QVERIFY(spy.isValid());

        // WHEN
        auto window = new QWindow;
        window->resize(64, 64);
        renderer.setRenderSurface(window);

        // THEN - fx and textures get notified of screen size when changing render surface
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), window->width() * window->devicePixelRatio());
        QCOMPARE(fx1Texture->height(), window->height() * window->devicePixelRatio());

        // WHEN
        spy.clear();
        window->resize(128, 128);

        // THEN
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), window->width() * window->devicePixelRatio());
        QCOMPARE(fx1Texture->height(), window->height() * window->devicePixelRatio());

        // WHEN - setting offscreen surface
        spy.clear();
        renderer.m_surfaceSelector->setExternalRenderTargetSize(QSize(32, 32));
        auto surface = new QOffscreenSurface;
        renderer.setRenderSurface(surface);

        // THEN - it should use ExternalRenderTargetSize
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), 32);
        QCOMPARE(fx1Texture->height(), 32);

        // WHEN
        spy.clear();
        renderer.m_surfaceSelector->setExternalRenderTargetSize(QSize(64, 64));

        // THEN
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), 64);
        QCOMPARE(fx1Texture->height(), 64);
    }

    //    void testReflectionStages()
    //    {
    //        // GIVEN
    //        Kuesa::ForwardRenderer renderer;

    //        // THEN
    //        QCOMPARE(renderer.m_reflectionStages.size(), 0U);

    //        // WHEN
    //        renderer.addReflectionPlane({0.0f, 1.0f, 0.0f, 0.0f}, nullptr);

    //        // THEN
    //        QCOMPARE(renderer.reflectionPlanes().size(), 1);
    //        QCOMPARE(renderer.reflectionPlanes().first(), QVector4D(0.0f, 1.0f, 0.0f, 0.0f));
    //        QCOMPARE(renderer.m_reflectionStages.size(), 1U);
    //    }

private:
    template<class T>
    T *findParentSGNode(Qt3DCore::QNode *node)
    {
        Q_ASSERT(node);
        auto parent = node->parentNode();
        while (parent && qstrcmp(parent->metaObject()->className(), T::staticMetaObject.className()) != 0) {
            parent = parent->parentNode();
        }
        return qobject_cast<T *>(parent);
    }
};

QTEST_MAIN(tst_ForwardRenderer)
#include "tst_forwardrenderer.moc"
