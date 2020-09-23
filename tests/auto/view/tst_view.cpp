/*
    tst_forwardrenderer.cpp

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

#include <QtTest/QtTest>

#include <Kuesa/view.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/private/opaquerenderstage_p.h>
#include <Kuesa/private/zfillrenderstage_p.h>
#include <Kuesa/private/transparentrenderstage_p.h>
#include <Kuesa/private/scenestages_p.h>
#include <Kuesa/private/reflectionstages_p.h>
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
        QCOMPARE(view.skinning(), true);
        QCOMPARE(view.backToFrontSorting(), false);
        QCOMPARE(view.zFilling(), false);
        QCOMPARE(view.particlesEnabled(), false);
        QCOMPARE(view.layers().size(), 0U);
        QCOMPARE(view.postProcessingEffects().size(), 0U);
        QCOMPARE(view.reflectionPlanes().size(), 0U);
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
        QVERIFY(view.skinning());

        // WHEN
        view.setSkinning(false);

        // THEN
        QVERIFY(!view.skinning());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setSkinning(false);

        // THEN
        QVERIFY(!view.skinning());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setSkinning(true);

        // THEN
        QVERIFY(view.skinning());
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
            QCOMPARE(view.postProcessingEffects().size(), 1);
            QCOMPARE(view.postProcessingEffects().front(), &fx);
        }
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(view.postProcessingEffects().size(), 0);
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
        QCOMPARE(view.postProcessingEffects().size(), 0);
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
        QCOMPARE(view.postProcessingEffects().size(), 3);
        QCOMPARE(view.postProcessingEffects().at(0), &fx3);
        QCOMPARE(view.postProcessingEffects().at(1), &fx1);
        QCOMPARE(view.postProcessingEffects().at(2), &fx2);

        // WHEN - removing fx3
        view.removePostProcessingEffect(&fx3);

        // THEN
        QCOMPARE(view.postProcessingEffects().size(), 2);

        // WHEN - re-adding fx3
        view.addPostProcessingEffect(&fx3);
        QCoreApplication::processEvents();

        // THEN - fx3 should go at end and order should now be 1, 2, 3
        QCOMPARE(view.postProcessingEffects().size(), 3);
        QCOMPARE(view.postProcessingEffects().at(0), &fx1);
        QCOMPARE(view.postProcessingEffects().at(1), &fx2);
        QCOMPARE(view.postProcessingEffects().at(2), &fx3);

//        // THEN - fx1 should have a valid input texture (main scene)
//        QVERIFY(fx1.inputTexture());

//        // THEN - fx1 is rendered into a texture which is used for input texture for fx2.
//        auto renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx1.frameGraphSubTree().data());
//        QVERIFY(renderTargetSelector);

//        auto outputTexture = Kuesa::ForwardRenderer::findRenderTargetTexture(renderTargetSelector->target(), Qt3DRender::QRenderTargetOutput::Color0);
//        QVERIFY(outputTexture);
//        QCOMPARE(outputTexture, fx2.inputTexture());

//        // THEN - fx2 is rendered into a texture which is used for input texture for fx3.
//        renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx2.frameGraphSubTree().data());
//        QVERIFY(renderTargetSelector);

//        outputTexture = Kuesa::ForwardRenderer::findRenderTargetTexture(renderTargetSelector->target(), Qt3DRender::QRenderTargetOutput::Color0);
//        QVERIFY(outputTexture);
//        QCOMPARE(outputTexture, fx3.inputTexture());

//        // THEN - fx3 is rendered into a texture which will be used as the input for the final tone mapping / gamma correction
//        renderTargetSelector = findParentSGNode<Qt3DRender::QRenderTargetSelector>(fx3.frameGraphSubTree().data());
//        QVERIFY(renderTargetSelector != nullptr);
    }


    void testReflectionStages()
    {
        // GIVEN
        Kuesa::View view;

        // THEN
        QCOMPARE(view.m_reflectionPlanes.size(), 0U);

        // WHEN
        view.addReflectionPlane({0.0f, 1.0f, 0.0f, 0.0f}, nullptr);

        // THEN
        QCOMPARE(view.reflectionPlanes().size(), 1);
        QCOMPARE(view.reflectionPlanes().front(), QVector4D(0.0f, 1.0f, 0.0f, 0.0f));
    }

    void checkGeneratedFrameGraphTree()
    {
        {
            // GIVEN
            // WHEN -> No Effects, No Reflection, No Layer

            // THEN
        }
        {
            // GIVEN

            // WHEN -> Reflections No Layer, No Layer, No Effects
        }
        {
            // GIVEN

            // WHEN -> Reflections Layer, No Layer, No Effects
        }
        {
            // GIVEN

            // WHEN -> Reflections Layer, No Layer, No Effects
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
