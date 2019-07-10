/*
    tst_forwardrenderer.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QtTest>

#include <Kuesa/forwardrenderer.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/private/opaquerenderstage_p.h>
#include <Kuesa/private/zfillrenderstage_p.h>
#include <Kuesa/private/transparentrenderstage_p.h>
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
#include <QWindow>
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

    void setSceneSize(const QSize &size) override
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
        QSignalSpy spy(&renderer, SIGNAL(externalRenderTargetSizeChanged(const QSize &)));
        QVERIFY(spy.isValid());

        // WHEN
        QSize newSize(123, 456);
        renderer.setExternalRenderTargetSize(newSize);

        // THEN
        QCOMPARE(renderer.externalRenderTargetSize(), newSize);
        QCOMPARE(spy.count(), 1);

        // WHEN
        renderer.setExternalRenderTargetSize(newSize);

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

    void testChangingFrustumCulling()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        QSignalSpy spy(&renderer, SIGNAL(frustumCullingChanged(bool)));

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(renderer.frustumCulling());

        // WHEN
        renderer.setFrustumCulling(false);

        // THEN
        QVERIFY(!renderer.frustumCulling());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        renderer.setFrustumCulling(false);

        // THEN
        QVERIFY(!renderer.frustumCulling());
        QCOMPARE(spy.count(), 0);

        // WHEN
        renderer.setFrustumCulling(true);

        // THEN
        QVERIFY(renderer.frustumCulling());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingAlpha()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        QSignalSpy spy(&renderer, SIGNAL(backToFrontSortingChanged(bool)));

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(renderer.backToFrontSorting());

        // WHEN
        renderer.setBackToFrontSorting(false);

        // THEN
        QVERIFY(!renderer.backToFrontSorting());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        renderer.setBackToFrontSorting(false);

        // THEN
        QVERIFY(!renderer.backToFrontSorting());
        QCOMPARE(spy.count(), 0);

        // WHEN
        renderer.setBackToFrontSorting(true);

        // THEN
        QVERIFY(renderer.backToFrontSorting());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingZFilling()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        QSignalSpy spy(&renderer, SIGNAL(zFillingChanged(bool)));

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!renderer.zFilling());

        // WHEN
        renderer.setZFilling(true);

        // THEN
        QVERIFY(renderer.zFilling());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        renderer.setZFilling(true);

        // THEN
        QVERIFY(renderer.zFilling());
        QCOMPARE(spy.count(), 0);

        // WHEN
        renderer.setZFilling(false);

        // THEN
        QVERIFY(!renderer.zFilling());
        QCOMPARE(spy.count(), 1);
    }

    void testRegisterPostProcessingEffect()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        QSignalSpy spy(&renderer, SIGNAL(frameGraphTreeReconfigured()));

        QVERIFY(spy.isValid());

        {
            tst_FX fx;

            // WHEN
            renderer.addPostProcessingEffect(&fx);

            // THEN
            QCOMPARE(renderer.postProcessingEffects().size(), 1);
            QCOMPARE(renderer.postProcessingEffects().first(), &fx);
            // QVERIFY(renderer.frameGraphSubtreeForPostProcessingEffect(&fx) != nullptr);
            QCOMPARE(spy.size(), 1);
            spy.clear();
        }

        // THEN
        QCOMPARE(renderer.postProcessingEffects().size(), 0);
        QCOMPARE(spy.size(), 1);
    }

    void testUnregisterPostProcessingEffect()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        tst_FX fx;
        QSignalSpy spy(&renderer, SIGNAL(frameGraphTreeReconfigured()));
        renderer.addPostProcessingEffect(&fx);

        // THEN
        // QVERIFY(renderer.frameGraphSubtreeForPostProcessingEffect(&fx) != nullptr);
        QCOMPARE(spy.size(), 1);
        spy.clear();

        // WHEN
        renderer.removePostProcessingEffect(&fx);

        // THEN
        QCOMPARE(renderer.postProcessingEffects().size(), 0);
        // QVERIFY(renderer.frameGraphSubtreeForPostProcessingEffect(&fx) == nullptr);
    }

    void testAddingMultipleEffects()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        tst_FX fx1, fx2, fx3;

        // WHEN
        renderer.addPostProcessingEffect(&fx3);
        renderer.addPostProcessingEffect(&fx1);
        renderer.addPostProcessingEffect(&fx2);

        // THEN
        QCOMPARE(renderer.postProcessingEffects().size(), 3);
        QCOMPARE(renderer.postProcessingEffects().at(0), &fx3);
        QCOMPARE(renderer.postProcessingEffects().at(1), &fx1);
        QCOMPARE(renderer.postProcessingEffects().at(2), &fx2);

        // WHEN - removing and re-adding fx3
        renderer.removePostProcessingEffect(&fx3);
        renderer.addPostProcessingEffect(&fx3);

        // THEN - fx3 should go at end and order should now be 1, 2, 3
        QCOMPARE(renderer.postProcessingEffects().size(), 3);
        QCOMPARE(renderer.postProcessingEffects().at(0), &fx1);
        QCOMPARE(renderer.postProcessingEffects().at(1), &fx2);
        QCOMPARE(renderer.postProcessingEffects().at(2), &fx3);

        // THEN - fx1 should have a valid input texture (main scene)
        QVERIFY(fx1.inputTexture());

        // THEN - fx1 is rendered into a texture which is used for input texture for fx2.
        auto renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx1.frameGraphSubTree().data());
        QVERIFY(renderTargetSelector);

        auto outputTexture = Kuesa::ForwardRenderer::findRenderTargetTexture(renderTargetSelector->target(), Qt3DRender::QRenderTargetOutput::Color0);
        QVERIFY(outputTexture);
        QCOMPARE(outputTexture, fx2.inputTexture());

        // THEN - fx2 is rendered into a texture which is used for input texture for fx3.
        renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx2.frameGraphSubTree().data());
        QVERIFY(renderTargetSelector);

        outputTexture = Kuesa::ForwardRenderer::findRenderTargetTexture(renderTargetSelector->target(), Qt3DRender::QRenderTargetOutput::Color0);
        QVERIFY(outputTexture);
        QCOMPARE(outputTexture, fx3.inputTexture());

        // THEN - fx3 is rendered into a texture which will be used as the input for the final tone mapping / gamma correction
        renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx3.frameGraphSubTree().data());
        QVERIFY(renderTargetSelector != nullptr);
    }

    void testResizingEffectsAndTextures()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;
        tst_FX fx1, fx2;
        renderer.addPostProcessingEffect(&fx1);
        renderer.addPostProcessingEffect(&fx2);

        auto renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx1.frameGraphSubTree().data());
        auto fx1Texture = renderTargetSelector->target()->outputs().first()->texture();

        QSignalSpy spy(&fx1, SIGNAL(sceneSizeChanged(const QSize &)));
        QVERIFY(spy.isValid());

        // WHEN
        auto window = new QWindow;
        window->resize(64, 64);
        renderer.setRenderSurface(window);

        // THEN - fx and textures get notified of screen size when changing render surface
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), window->width());
        QCOMPARE(fx1Texture->height(), window->height());

        // WHEN
        spy.clear();
        window->resize(128, 128);

        // THEN
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), window->width());
        QCOMPARE(fx1Texture->height(), window->height());

        // WHEN - setting offscreen surface
        spy.clear();
        renderer.setExternalRenderTargetSize(QSize(32, 32));
        auto surface = new QOffscreenSurface;
        renderer.setRenderSurface(surface);

        // THEN - it should use ExternalRenderTargetSize
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), 32);
        QCOMPARE(fx1Texture->height(), 32);

        // WHEN
        spy.clear();
        renderer.setExternalRenderTargetSize(QSize(64, 64));

        // THEN
        QVERIFY(spy.size() >= 1);
        QCOMPARE(fx1Texture->width(), 64);
        QCOMPARE(fx1Texture->height(), 64);
    }

    void testRenderStages()
    {
        // GIVEN
        Kuesa::ForwardRenderer renderer;

        // THEN
        QCOMPARE(renderer.m_sceneStages.size(), 2);

        Kuesa::ForwardRenderer::SceneStagesPtr sceneStage = renderer.m_sceneStages.first();

        Kuesa::OpaqueRenderStage *opaqueStage = sceneStage->m_opaqueStage;
        Kuesa::TransparentRenderStage *transparentStage = sceneStage->m_transparentStage;
        QVERIFY(sceneStage->m_zFillStage == nullptr);

        QVERIFY(qobject_cast<Qt3DRender::QTechniqueFilter *>(opaqueStage->parent()) != nullptr);
        QVERIFY(qobject_cast<Qt3DRender::QTechniqueFilter *>(transparentStage->parent()) != nullptr);


        QCOMPARE(opaqueStage->zFilling(), false);
        QCOMPARE(transparentStage->backToFrontSorting(), true);

        // WHEN
        renderer.setBackToFrontSorting(false);

        // THEN
        QCOMPARE(opaqueStage->zFilling(), false);
        QCOMPARE(transparentStage->backToFrontSorting(), false);

        // WHEN
        renderer.setZFilling(true);

        // THEN
        sceneStage = renderer.m_sceneStages.first();

        QVERIFY(sceneStage->m_zFillStage != nullptr);
        QCOMPARE(opaqueStage->zFilling(), true);
        QCOMPARE(transparentStage->backToFrontSorting(), false);

        QCOMPARE(sceneStage->m_opaqueStage, opaqueStage);
        QCOMPARE(sceneStage->m_transparentStage, transparentStage);

        // WHEN
        renderer.setZFilling(false);

        // THEN
        sceneStage = renderer.m_sceneStages.first();

        QVERIFY(sceneStage->m_zFillStage == nullptr);
        QCOMPARE(sceneStage->m_opaqueStage, opaqueStage);
        QCOMPARE(sceneStage->m_transparentStage, transparentStage);
    }

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
