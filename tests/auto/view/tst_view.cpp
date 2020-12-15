/*
    tst_view.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Kuesa/view.h>
#include <Kuesa/reflectionplane.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/private/opaquerenderstage_p.h>
#include <Kuesa/private/zfillrenderstage_p.h>
#include <Kuesa/private/transparentrenderstage_p.h>
#include <Kuesa/private/scenestages_p.h>
#include <Kuesa/private/effectsstages_p.h>
#include <Kuesa/private/reflectionstages_p.h>
#include <Kuesa/private/particlerenderstage_p.h>
#include <Kuesa/private/framegraphutils_p.h>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QBlitFramebuffer>
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

    void setWindowSize(const QSize &size) override
    {
        emit windowSizeChanged(size);
    }

Q_SIGNALS:
    void windowSizeChanged(const QSize &size);

private:
    FrameGraphNodePtr m_rootNode;
    Qt3DRender::QAbstractTexture *m_inputTexture;
};

} // namespace

class tst_View : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::View view;

        // THEN
        QCOMPARE(view.viewportRect(), QRectF(0.0f, 0.0f, 1.0f, 1.0f));
        QVERIFY(view.camera() == nullptr);
        QCOMPARE(view.frustumCulling(), true);
        QCOMPARE(view.skinning(), false);
        QCOMPARE(view.backToFrontSorting(), false);
        QCOMPARE(view.zFilling(), false);
        QCOMPARE(view.particlesEnabled(), false);
        QCOMPARE(view.layers().size(), 0U);
        QCOMPARE(view.postProcessingEffects().size(), 0U);
        QCOMPARE(view.reflectionPlanes().size(), 0U);
        QCOMPARE(view.reflectionTextureSize(), QSize(512, 512));
    }

    void testChangingViewportRect()
    {
        // GIVEN
        Kuesa::View view;
        QSignalSpy spy(&view, &Kuesa::View::viewportRectChanged);
        QVERIFY(spy.isValid());

        // WHEN
        QRectF newRect(0.0f, 0.0f, 0.5f, 0.5f);
        view.setViewportRect(newRect);

        // THEN
        QCOMPARE(view.viewportRect(), newRect);
        QCOMPARE(spy.count(), 1);

        // WHEN
        view.setViewportRect(newRect);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testChangingCamera()
    {
        // GIVEN
        Kuesa::View view;
        qRegisterMetaType<Qt3DCore::QEntity *>("QEntity");
        QSignalSpy spy(&view, &Kuesa::View::cameraChanged);
        QVERIFY(spy.isValid());

        {
            // WHEN
            Qt3DRender::QCamera newCamera;
            view.setCamera(&newCamera);

            // THEN
            QCOMPARE(view.camera(), &newCamera);
            QCOMPARE(spy.count(), 1);

            // WHEN
            view.setCamera(&newCamera);

            // THEN
            QCOMPARE(spy.count(), 1);
        }

        // WHEN -> newCamera destroyed

        // THEN
        QVERIFY(view.camera() == nullptr);
    }

    void testChangingFrustumCulling()
    {
        // GIVEN
        Kuesa::View view;
        QSignalSpy spy(&view, &Kuesa::View::frustumCullingChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(view.frustumCulling());

        // WHEN
        view.setFrustumCulling(false);

        // THEN
        QVERIFY(!view.frustumCulling());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setFrustumCulling(false);

        // THEN
        QVERIFY(!view.frustumCulling());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setFrustumCulling(true);

        // THEN
        QVERIFY(view.frustumCulling());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingSkinning()
    {
        // GIVEN
        Kuesa::View view;
        QSignalSpy spy(&view, &Kuesa::View::skinningChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!view.skinning());

        // WHEN
        view.setSkinning(true);

        // THEN
        QVERIFY(view.skinning());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setSkinning(true);

        // THEN
        QVERIFY(view.skinning());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setSkinning(false);

        // THEN
        QVERIFY(!view.skinning());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingBackToFrontSorting()
    {
        // GIVEN
        Kuesa::View view;
        QSignalSpy spy(&view, &Kuesa::View::backToFrontSortingChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!view.backToFrontSorting());

        // WHEN
        view.setBackToFrontSorting(true);

        // THEN
        QVERIFY(view.backToFrontSorting());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setBackToFrontSorting(true);

        // THEN
        QVERIFY(view.backToFrontSorting());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setBackToFrontSorting(false);

        // THEN
        QVERIFY(!view.backToFrontSorting());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingZFilling()
    {
        // GIVEN
        Kuesa::View view;
        QSignalSpy spy(&view, &Kuesa::View::zFillingChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!view.zFilling());

        // WHEN
        view.setZFilling(true);

        // THEN
        QVERIFY(view.zFilling());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setZFilling(true);

        // THEN
        QVERIFY(view.zFilling());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setZFilling(false);

        // THEN
        QVERIFY(!view.zFilling());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingParticlesEnabled()
    {
        // GIVEN
        Kuesa::View view;
        QSignalSpy spy(&view, &Kuesa::View::particlesEnabledChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!view.particlesEnabled());

        // WHEN
        view.setParticlesEnabled(true);

        // THEN
        QVERIFY(view.particlesEnabled());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setParticlesEnabled(true);

        // THEN
        QVERIFY(view.particlesEnabled());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setParticlesEnabled(false);

        // THEN
        QVERIFY(!view.particlesEnabled());
        QCOMPARE(spy.count(), 1);
    }

    void testRegisterPostProcessingEffect()
    {
        // GIVEN
        Kuesa::View view;

        {
            tst_FX fx;

            // WHEN
            view.addPostProcessingEffect(&fx);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(view.postProcessingEffects().size(), 1U);
            QCOMPARE(view.postProcessingEffects().front(), &fx);
        }
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(view.postProcessingEffects().size(), 0U);
    }

    void testUnregisterPostProcessingEffect()
    {
        // GIVEN
        Kuesa::View view;
        tst_FX fx;

        // WHEN
        view.addPostProcessingEffect(&fx);
        QCoreApplication::processEvents();

        view.removePostProcessingEffect(&fx);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(view.postProcessingEffects().size(), 0U);
    }

    void testAddingMultipleEffects()
    {
        // GIVEN
        Kuesa::View view;
        tst_FX fx1, fx2, fx3;

        // WHEN
        view.addPostProcessingEffect(&fx3);
        view.addPostProcessingEffect(&fx1);
        view.addPostProcessingEffect(&fx2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(view.postProcessingEffects().size(), size_t(3));
        QCOMPARE(view.postProcessingEffects().at(0), &fx3);
        QCOMPARE(view.postProcessingEffects().at(1), &fx1);
        QCOMPARE(view.postProcessingEffects().at(2), &fx2);

        // WHEN - removing fx3
        view.removePostProcessingEffect(&fx3);

        // THEN
        QCOMPARE(view.postProcessingEffects().size(), size_t(2));

        // WHEN - re-adding fx3
        view.addPostProcessingEffect(&fx3);
        QCoreApplication::processEvents();

        // THEN - fx3 should go at end and order should now be 1, 2, 3
        QCOMPARE(view.postProcessingEffects().size(), size_t(3));
        QCOMPARE(view.postProcessingEffects().at(0), &fx1);
        QCOMPARE(view.postProcessingEffects().at(1), &fx2);
        QCOMPARE(view.postProcessingEffects().at(2), &fx3);
    }

    void testSingleReflectionPlanes()
    {
        // GIVEN
        Kuesa::View view;

        // THEN
        QCOMPARE(view.reflectionPlanes().size(), 0U);

        // WHEN
        {
            Kuesa::ReflectionPlane p;
            view.addReflectionPlane(&p);

            // THEN
            QCOMPARE(view.reflectionPlanes().size(), 1U);
            QCOMPARE(view.reflectionPlanes().front(), &p);
        }

        // WHEN -> Plane destroyed

        // THEN
        QCOMPARE(view.reflectionPlanes().size(), 0U);
    }

    void testMultipleReflectionPlanes()
    {
        // GIVEN
        Kuesa::View view;

        {
            // WHEN
            Kuesa::ReflectionPlane p1, p2, p3;

            view.addReflectionPlane(&p1);
            view.addReflectionPlane(&p2);
            view.addReflectionPlane(&p3);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(view.reflectionPlanes().size(), size_t(3));
            QCOMPARE(view.reflectionPlanes().at(0), &p1);
            QCOMPARE(view.reflectionPlanes().at(1), &p2);
            QCOMPARE(view.reflectionPlanes().at(2), &p3);

            // WHEN - removing p3
            view.removeReflectionPlane(&p3);

            // THEN
            QCOMPARE(view.reflectionPlanes().size(), size_t(2));

            // WHEN - re-adding p3
            view.addReflectionPlane(&p3);
            QCoreApplication::processEvents();

            // THEN - fx3 should go at end and order should now be 1, 2, 3
            QCOMPARE(view.reflectionPlanes().size(), size_t(3));
            QCOMPARE(view.reflectionPlanes().at(0), &p1);
            QCOMPARE(view.reflectionPlanes().at(1), &p2);
            QCOMPARE(view.reflectionPlanes().at(2), &p3);
        }

        // THEN -> Removed and doesn't crash
        QCOMPARE(view.reflectionPlanes().size(), size_t(0));
    }

    void testReflectionTextureResized()
    {
        // GIVEN
        Kuesa::View view;

        QSignalSpy reflectionTextureChangedSpy(&view, &Kuesa::View::reflectionTextureChanged);
        QSignalSpy reflectionTextureSizeChangedSpy(&view, &Kuesa::View::reflectionTextureSizeChanged);

        QVERIFY(reflectionTextureChangedSpy.isValid());
        QVERIFY(reflectionTextureSizeChangedSpy.isValid());

        // WHEN
        view.setReflectionTextureSize({1024, 1024});

        // THEN
        QCOMPARE(reflectionTextureChangedSpy.count(), 1);
        QCOMPARE(reflectionTextureSizeChangedSpy.count(), 1);
        QCOMPARE(view.reflectionTextureSize(), QSize(1024, 1024));
    }

    void checkGeneratedFrameGraphTreeNoFxNoReflectionNoLayer()
    {
        // GIVEN
        Kuesa::View v;
        // WHEN -> No Effects, No Reflection, No Layer

        // THEN
        QCOMPARE(v.children().size(), 3);
        QCOMPARE(v.children()[0], v.m_fg->m_renderToTextureRootNode);
        QCOMPARE(v.children()[1], v.m_fg->m_renderTargets[0]);
        QCOMPARE(v.children()[2], v.m_internalFXStages);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 2);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
        QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 1);
        Kuesa::SceneStages *child = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children().first());
        QVERIFY(child);

        QVERIFY(v.m_fxStages->parent() == nullptr);
        QCOMPARE(v.m_fxStages->effects().size(), size_t(0));
        QCOMPARE(v.m_fxs.size(), size_t(0));
    }

    void checkGeneratedFrameGraphTreeNoFxNoReflectionLayer()
    {
        // GIVEN
        Kuesa::View v;

        // WHEN
        {
            Qt3DRender::QLayer l;
            v.addLayer(&l);

            // THEN
            QCOMPARE(v.children().size(), 4);
            QCOMPARE(v.children()[0], v.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v.children()[1], v.m_fg->m_renderTargets[0]);
            QCOMPARE(v.children()[2], v.m_internalFXStages);
            QCOMPARE(v.children().last(), &l);
            QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 2);
            QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
            QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
            QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 1);
            Kuesa::SceneStages *child = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children().first());
            QVERIFY(child);
        }

        // THEN
        QCOMPARE(v.children().size(), 3);
        QCOMPARE(v.children()[0], v.m_fg->m_renderToTextureRootNode);
        QCOMPARE(v.children()[1], v.m_fg->m_renderTargets[0]);
        QCOMPARE(v.children()[2], v.m_internalFXStages);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 2);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
        QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 1);
        Kuesa::SceneStages *child = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children().first());
        QVERIFY(child);

        QVERIFY(v.m_fxStages->parent() == nullptr);
        QCOMPARE(v.m_fxStages->effects().size(), size_t(0));
        QCOMPARE(v.m_fxs.size(), size_t(0));
    }

    void checkGeneratedFrameGraphTreeNoFxReflectionNoLayer()
    {
        // GIVEN
        Kuesa::View v;

        {
            // WHEN -> Reflections, No Layer, No Effects
            Kuesa::ReflectionPlane plane;
            v.addReflectionPlane(&plane);

            // THEN
            QCOMPARE(v.children().size(), 4);
            QCOMPARE(v.children()[0], v.m_fg->m_renderTargets[0]);
            // We parent plane if it has no parent
            QCOMPARE(v.children()[1], &plane);
            QCOMPARE(v.children()[2], v.m_fg->m_renderToTextureRootNode);
            QCOMPARE(v.children()[3], v.m_internalFXStages);
            QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 2);
            QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
            QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
            QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 2);
            Kuesa::ReflectionStages *child1 = qobject_cast<Kuesa::ReflectionStages *>(v.m_fg->m_mainSceneLayerFilter->children()[0]);
            Kuesa::SceneStages *child2 = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children()[1]);
            QVERIFY(child1);
            QVERIFY(child2);
        }

        // THEN
        QCOMPARE(v.children().size(), 3);
        QCOMPARE(v.children()[0], v.m_fg->m_renderTargets[0]);
        QCOMPARE(v.children()[1], v.m_fg->m_renderToTextureRootNode);
        QCOMPARE(v.children()[2], v.m_internalFXStages);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 2);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
        QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 1);
        Kuesa::SceneStages *child = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children().first());
        QVERIFY(child);
    }

    void checkGeneratedFrameGraphTreeFxRNoeflectionNoLayer()
    {
        // GIVEN
        Kuesa::View v;
        tst_FX fx;

        // WHEN -> No Reflections Layer, No Layer, Effects
        v.addPostProcessingEffect(&fx);

        // THEN
        QCoreApplication::processEvents();

        QCOMPARE(v.children().size(), 5);
        QCOMPARE(v.children()[0], v.m_fg->m_renderTargets[0]);
        QCOMPARE(v.children()[1], v.m_fg->m_renderToTextureRootNode);
        QCOMPARE(v.children()[2], v.m_fg->m_renderTargets[1]);
        QCOMPARE(v.children()[3], v.m_fxStages);
        QCOMPARE(v.children()[4], v.m_internalFXStages);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 3);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().last(), v.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1);
        QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 1);
        Kuesa::SceneStages *child = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children()[0]);
        QVERIFY(child);

        QCOMPARE(v.m_fxStages->effects().size(), size_t(1));
        QCOMPARE(v.m_fxs.size(), size_t(1));
        QCOMPARE(v.m_fxStages->children().size(), 1);

        Qt3DRender::QRenderTargetSelector *rtSelectorEffects = qobject_cast<Qt3DRender::QRenderTargetSelector *>(v.m_fxStages->children().first());
        QVERIFY(rtSelectorEffects);
    }

    void checkGeneratedFrameGraphTreeParticlesNoFxNoReflectionNoLayer()
    {
        // GIVEN
        Kuesa::View v;

        // WHEN
        v.setParticlesEnabled(true);

        // THEN
        QCoreApplication::processEvents();

        QCOMPARE(v.children().size(), 3);
        QCOMPARE(v.children()[0], v.m_fg->m_renderTargets[0]);
        QCOMPARE(v.children()[1], v.m_fg->m_renderToTextureRootNode);
        QCOMPARE(v.children()[2], v.m_internalFXStages);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 2);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
        QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 1);

        Kuesa::SceneStages *child0 = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children()[0]);
        QVERIFY(child0);
        Kuesa::ParticleRenderStage *child1 = child0->findChild<Kuesa::ParticleRenderStage *>();
        QVERIFY(child1);
    }

    void checkGeneratedFrameGraphTreeParticlesFxReflectionNoLayer()
    {
        // GIVEN
        Kuesa::View v;
        tst_FX fx;
        Kuesa::ReflectionPlane plane;

        // WHEN
        v.setParticlesEnabled(true);
        v.addReflectionPlane(&plane);
        v.addPostProcessingEffect(&fx);

        // THEN
        QCoreApplication::processEvents();

        QCOMPARE(v.children().size(), 6);
        QCOMPARE(v.children()[0], v.m_fg->m_renderTargets[0]);
        QCOMPARE(v.children()[1], &plane);
        QCOMPARE(v.children()[2], v.m_fg->m_renderToTextureRootNode);
        QCOMPARE(v.children()[3], v.m_fg->m_renderTargets[1]);
        QCOMPARE(v.children()[4], v.m_fxStages);
        QCOMPARE(v.children()[5], v.m_internalFXStages);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().size(), 3);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().first(), v.m_fg->m_clearRT0);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children()[1], v.m_fg->m_mainSceneLayerFilter);
        QCOMPARE(v.m_fg->m_renderToTextureRootNode->children().last(), v.m_fg->m_blitFramebufferNodeFromFBO0ToFBO1);
        QCOMPARE(v.m_fg->m_mainSceneLayerFilter->children().size(), 2);
        Kuesa::ReflectionStages *child1 = qobject_cast<Kuesa::ReflectionStages *>(v.m_fg->m_mainSceneLayerFilter->children()[0]);
        Kuesa::SceneStages *child2 = qobject_cast<Kuesa::SceneStages *>(v.m_fg->m_mainSceneLayerFilter->children()[1]);
        QVERIFY(child1);
        QVERIFY(child2);

        Kuesa::ParticleRenderStage *child3 = child2->findChild<Kuesa::ParticleRenderStage *>();
        QVERIFY(child3);

        QCOMPARE(v.m_fxStages->effects().size(), size_t(1));
        QCOMPARE(v.m_fxs.size(), size_t(1));
        QCOMPARE(v.m_fxStages->children().size(), 1);

        Qt3DRender::QRenderTargetSelector *rtSelectorEffects = qobject_cast<Qt3DRender::QRenderTargetSelector *>(v.m_fxStages->children().first());
        QVERIFY(rtSelectorEffects);
    }

    void checkRootView()
    {

        {
            // GIVEN
            Kuesa::View root;

            // WHEN
            Kuesa::View *rV = root.rootView();

            // THEN
            QCOMPARE(rV, &root);
        }
        {
            // GIVEN
            Kuesa::View root;
            Kuesa::View *child = new Kuesa::View;

            // WHEN
            child->setParent(&root);
            Kuesa::View *rV = child->rootView();

            // THEN
            QCOMPARE(rV, &root);
        }
        {
            // GIVEN
            Kuesa::View root;
            Kuesa::View *childL1 = new Kuesa::View;
            Kuesa::View *childL2 = new Kuesa::View;
            Kuesa::View *childL3 = new Kuesa::View;

            // WHEN
            childL3->setParent(childL2);
            childL2->setParent(childL1);
            childL1->setParent(&root);
            Kuesa::View *rV = childL3->rootView();

            // THEN
            QCOMPARE(rV, &root);
        }
        {
            // GIVEN
            Kuesa::View root;
            Kuesa::View *childL1 = new Kuesa::View;
            Qt3DCore::QNode *childL2 = new Kuesa::View;
            Kuesa::View *childL3 = new Kuesa::View;
            Qt3DCore::QNode *childL4 = new Kuesa::View;
            Kuesa::View *childL5 = new Kuesa::View;

            // WHEN
            childL5->setParent(childL4);
            childL4->setParent(childL3);
            childL3->setParent(childL2);
            childL2->setParent(childL1);
            childL1->setParent(&root);
            Kuesa::View *rV = childL5->rootView();

            // THEN
            QCOMPARE(rV, &root);
        }
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

QTEST_MAIN(tst_View)
#include "tst_view.moc"
