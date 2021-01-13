/*
    tst_transformtracker.cpp

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
#include <QSignalSpy>

#include <Kuesa/transformtracker.h>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/private/qtransform_p.h>

namespace {

void updateWorldMatrixOnTransform(Qt3DCore::QTransform *t)
{
    Qt3DCore::QTransformPrivate *d = static_cast<Qt3DCore::QTransformPrivate *>(Qt3DCore::QNodePrivate::get(t));
    d->setWorldMatrix(t->matrix());
}

}

class tst_TransformTracker : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;

        // THEN
        QVERIFY(tracker.name().isEmpty());
        QVERIFY(tracker.camera() == nullptr);
        QCOMPARE(tracker.screenSize(), QSize());
        QCOMPARE(tracker.viewportRect(), QRectF());
        QCOMPARE(tracker.matrix(), QMatrix4x4());
        QCOMPARE(tracker.worldMatrix(), QMatrix4x4());
        QCOMPARE(tracker.rotation(), QQuaternion());
        QCOMPARE(tracker.rotationX(), 0.0f);
        QCOMPARE(tracker.rotationY(), 0.0f);
        QCOMPARE(tracker.rotationZ(), 0.0f);
        QCOMPARE(tracker.scale(), 0.0f);
        QCOMPARE(tracker.scale3D(), QVector3D(0.0f, 0.0f, 0.0f));
        QCOMPARE(tracker.translation(), QVector3D());
        QCOMPARE(tracker.screenPosition(), QPointF());
    }

    void checkSetViewportRect()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::viewportRectChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setViewportRect(QRectF(0.5f, 0.5f, 0.5f, 0.5f));

        // THEN
        QCOMPARE(tracker.viewportRect(), QRectF(0.5f, 0.5f, 0.5f, 0.5f));
        QCOMPARE(spy.count(), 1);

        // WHEN
        tracker.setViewportRect(QRectF(0.5f, 0.5f, 0.5f, 0.5f));

        // THEN
        QCOMPARE(spy.count(), 1);

        // WHEN
        tracker.setViewportRect(0.0f, 0.0f, 1.0f, 1.0f);

        // THEN
        QCOMPARE(tracker.viewportRect(), QRectF(0.0f, 0.0f, 1.0f, 1.0f));
        QCOMPARE(spy.count(), 2);
    }

    void checkSetScreenSize()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::screenSizeChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setScreenSize({512, 512});

        // THEN
        QCOMPARE(tracker.screenSize(), QSize(512, 512));
        QCOMPARE(spy.count(), 1);

        // WHEN
        tracker.setScreenSize({512, 512});

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void checkSetMatrix()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::matrixChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        QMatrix4x4 m;
        m.translate(QVector3D(5.0f, 5.0f, 5.0f));
        tracker.setMatrix(m);

        // THEN
        QCOMPARE(spy.count(), 0);
        QCOMPARE(tracker.matrix(), QMatrix4x4());

        // WHEN
        Kuesa::SceneEntity scene;
        {
            Qt3DCore::QTransform t;
            t.setMatrix(m);
            scene.transforms()->add(QStringLiteral("MyTransform"), &t);

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyTransform"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(spy.count(), 1);
            QCOMPARE(tracker.matrix(), m);

            // WHEN
            m.translate(QVector3D(1.0f, 0.0f, 0.0f));
            tracker.setMatrix(m);
        }

        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(tracker.matrix(), QMatrix4x4());
    }

    void checkSetRotationX()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::rotationXChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setRotationX(25.0f);

        // THEN
        QCOMPARE(spy.count(), 0);
        QCOMPARE(tracker.rotationX(), 0.0f);

        // WHEN
        Kuesa::SceneEntity scene;
        {
            Qt3DCore::QTransform t;
            scene.transforms()->add(QStringLiteral("MyTransform"), &t);
            t.setRotationX(25.0f);

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyTransform"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(spy.count(), 1);
            QCOMPARE(tracker.rotationX(), 25.0f);

            // WHEN
            tracker.setRotationX(50.0f);

            // THEN
            QCOMPARE(spy.count(), 2);
            QCOMPARE(tracker.rotationX(), 50.0f);
        }

        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(tracker.rotationX(), 0.0f);
    }

    void checkSetRotationY()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::rotationYChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setRotationY(25.0f);

        // THEN
        QCOMPARE(spy.count(), 0);
        QCOMPARE(tracker.rotationY(), 0.0f);

        // WHEN
        Kuesa::SceneEntity scene;
        {
            Qt3DCore::QTransform t;
            scene.transforms()->add(QStringLiteral("MyTransform"), &t);
            t.setRotationY(25.0f);

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyTransform"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(spy.count(), 1);
            QCOMPARE(tracker.rotationY(), 25.0f);

            // WHEN
            tracker.setRotationY(50.0f);

            // THEN
            QCOMPARE(spy.count(), 2);
            QCOMPARE(tracker.rotationY(), 50.0f);\
        }
        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(tracker.rotationY(), 0.0f);
    }

    void checkSetRotationZ()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::rotationZChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setRotationZ(25.0f);

        // THEN
        QCOMPARE(spy.count(), 0);
        QCOMPARE(tracker.rotationZ(), 0.0f);

        // WHEN
        Kuesa::SceneEntity scene;
        {
            Qt3DCore::QTransform t;
            scene.transforms()->add(QStringLiteral("MyTransform"), &t);
            t.setRotationZ(25.0f);

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyTransform"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(spy.count(), 1);
            QCOMPARE(tracker.rotationZ(), 25.0f);

            // WHEN
            tracker.setRotationZ(50.0f);

            // THEN
            QCOMPARE(spy.count(), 2);
            QCOMPARE(tracker.rotationZ(), 50.0f);
        }
        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(tracker.rotationZ(), 0.0f);
    }

    void checkSetScale()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::scaleChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setScale(5.0f);

        // THEN
        QCOMPARE(spy.count(), 0);
        QCOMPARE(tracker.scale(), 0.0f);

        // WHEN
        Kuesa::SceneEntity scene;
        {
            Qt3DCore::QTransform t;
            scene.transforms()->add(QStringLiteral("MyTransform"), &t);
            t.setScale(25.0f);

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyTransform"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(spy.count(), 1);
            QCOMPARE(tracker.scale(), 25.0f);

            // WHEN
            tracker.setScale(50.0f);

            // THEN
            QCOMPARE(spy.count(), 2);
            QCOMPARE(tracker.scale(), 50.0f);
        }
        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(tracker.scale(), 0.0f);
    }

    void checkSetScale3D()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::scale3DChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setScale3D(QVector3D(5.0f, 5.0f, 5.0f));

        // THEN
        QCOMPARE(spy.count(), 0);
        QCOMPARE(tracker.scale3D(), QVector3D());

        // WHEN
        Kuesa::SceneEntity scene;
        {
            Qt3DCore::QTransform t;
            scene.transforms()->add(QStringLiteral("MyTransform"), &t);
            t.setScale3D(QVector3D(5.0f, 5.0f, 5.0f));

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyTransform"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(spy.count(), 1);
            QCOMPARE(tracker.scale3D(), QVector3D(5.0f, 5.0f, 5.0f));

            // WHEN
            tracker.setScale3D(QVector3D(15.0f, 15.0f, 15.0f));

            // THEN
            QCOMPARE(spy.count(), 2);
            QCOMPARE(tracker.scale3D(), QVector3D(15.0f, 15.0f, 15.0f));
        }
        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(tracker.scale3D(), QVector3D());
    }

    void checkSetName()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::nameChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setName(QStringLiteral("MyTransform"));

        // THEN
        QCOMPARE(spy.count(), 1);
        QCOMPARE(tracker.name(), QStringLiteral("MyTransform"));

        // WHEN
        tracker.setName(QStringLiteral("MyTransform"));

        // THEN
        QCOMPARE(spy.count(), 1);

        // WHEN
        Kuesa::SceneEntity scene;
        Qt3DCore::QTransform t;
        t.setRotationX(90.0f);
        scene.transforms()->add(QStringLiteral("MyTransform2"), &t);

        tracker.setSceneEntity(&scene);
        tracker.setName(QStringLiteral("MyTransform2"));

        // THEN
        QCOMPARE(spy.count(), 2);
        QCOMPARE(tracker.rotationX(), 90.0f);

        // WHEN
        tracker.setName(QStringLiteral("MyTransform"));

        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(tracker.rotationX(), 0.0f);
    }

    void checkSetCamera()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::cameraChanged);

        // THEN
        QVERIFY(spy.isValid());

        {
            // WHEN
            Qt3DCore::QEntity camera;
            tracker.setCamera(&camera);

            // THEN
            QCOMPARE(tracker.camera(), &camera);
            QCOMPARE(spy.count(), 1);

            // WHEN
            tracker.setCamera(&camera);

            // THEN
            QCOMPARE(tracker.camera(), &camera);
            QCOMPARE(spy.count(), 1);
        }
        // THEN
        QVERIFY(tracker.camera() == nullptr);
        QCOMPARE(spy.count(), 2);
    }

    void checkUpdatesScreenProjection()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::TransformTracker::screenPositionChanged);

        Qt3DCore::QEntity camera;
        Qt3DCore::QTransform* cameraTransform = new Qt3DCore::QTransform;
        Qt3DRender::QCameraLens *cameraLens = new Qt3DRender::QCameraLens;
        camera.addComponent(cameraTransform);
        camera.addComponent(cameraLens);

        // WHEN
        Kuesa::SceneEntity scene;
        Qt3DCore::QTransform t1;
        Qt3DCore::QTransform t2;
        scene.transforms()->add(QStringLiteral("MyTransform"), &t1);
        scene.transforms()->add(QStringLiteral("MyTransform2"), &t2);
        tracker.setSceneEntity(&scene);

        // THEN
        QCOMPARE(spy.count(), 0);

        // WHEN
        tracker.setName(QStringLiteral("MyTransform"));

        // THEN
        QCOMPARE(spy.count(), 1);

        // WHEN -> Camera Changed
        tracker.setCamera(&camera);

        // THEN
        QCOMPARE(spy.count(), 2);

        // WHEN -> Camera Transformed
        cameraTransform->setTranslation(QVector3D(0.0f, 1.0f, 0.0f));
        // Simulate backend updating transform
        cameraTransform->worldMatrixChanged(cameraTransform->matrix());

        // THEN
        QCOMPARE(spy.count(), 3);

        // WHEN -> Camera Lens changed
        cameraLens->setFieldOfView(180.0f);

        // THEN
        QCOMPARE(spy.count(), 4);

        // WHEN -> Transform Changed
        tracker.setName(QStringLiteral("MyTransform2"));

        // THEN
        QCOMPARE(spy.count(), 5);

        // WHEN -> Transform Transformed
        t2.setScale(25.0f);
        // Simulate backend updating transform
        t2.worldMatrixChanged(t2.matrix());

        // THEN
        QCOMPARE(spy.count(), 6);

        // WHEN -> SceneSize updated
        tracker.setScreenSize({512, 512});

        // THEN
        QCOMPARE(spy.count(), 7);

        // WHEN -> Viewport updated
        tracker.setViewportRect({0.0f, 0.5f, 1.0f, 0.5f});
    }

    void checkHandlesSceneEntityDestruction()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        Qt3DCore::QTransform *t = new Qt3DCore::QTransform();
        t->setScale3D(QVector3D(5.0f, 5.0f, 5.0f));

        // WHEN
        {
            Kuesa::SceneEntity scene;
            scene.transforms()->add(QStringLiteral("MyTransform"), t);

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyTransform"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(tracker.sceneEntity(), &scene);
            QCOMPARE(tracker.scale3D(), QVector3D(5.0f, 5.0f, 5.0f));
        }

        // THEN
        QVERIFY(tracker.sceneEntity() == nullptr);
        QCOMPARE(tracker.scale3D(), QVector3D());
    }

    void checkClampsScreenPosition_data()
    {
        QTest::addColumn<QVector3D>("translation");
        QTest::addColumn<QRectF>("viewport");
        QTest::addColumn<QPointF>("expectedScreenPosition");

        QTest::newRow("Center") << QVector3D(0.0, 0.0f, 1.0f) << QRectF() << QPointF(256.0f, 256.0f);
        QTest::newRow("TopLeft") << QVector3D(-0.5, 0.5f, 1.0f) << QRectF() << QPointF(128.0f,128.0f);
        QTest::newRow("TopRight") << QVector3D(0.5, 0.5f, 1.0f) << QRectF() << QPointF(384.0f,128.0f);
        QTest::newRow("BottomLeft") << QVector3D(-0.5, -0.5f, 1.0f) << QRectF() << QPointF(128.0f, 384.0f);
        QTest::newRow("BottomRight") << QVector3D(0.5, -0.5f, 1.0f) << QRectF() << QPointF(384.0f, 384.0f);
        QTest::newRow("OutsideTopLeft") << QVector3D(-2.0f, 2.0f, 1.0f) << QRectF() << QPointF(0.0f, 0.0f);
        QTest::newRow("OutsideTopRight") << QVector3D(2.0f, 2.0f, 1.0f) << QRectF() << QPointF(512.0f, 0.0f);
        QTest::newRow("OutsideBottomLeft") << QVector3D(-2.0f, -2.0f, 1.0f) << QRectF() << QPointF(0.0f, 512.0f);
        QTest::newRow("OutsideBottomRight") << QVector3D(2.0f, -2.0f, 1.0f) << QRectF() << QPointF(512.0f, 512.0f);

        QRectF innerViewport(0.25, 0.25, 0.5f, 0.5f);
        QTest::newRow("CenterInnerViewport") << QVector3D(0.0, 0.0f, 1.0f) << innerViewport << QPointF(256.0f, 256.0f);
        QTest::newRow("TopLeftInnerViewport") << QVector3D(-0.5, 0.5f, 1.0f) << innerViewport << QPointF(192.0f,192.0f);
        QTest::newRow("TopRightInnerViewport") << QVector3D(0.5, 0.5f, 1.0f) << innerViewport << QPointF(320.0f,192.0f);
        QTest::newRow("BottomLeftInnerViewport") << QVector3D(-0.5, -0.5f, 1.0f) << innerViewport << QPointF(192.0f, 320.0f);
        QTest::newRow("BottomRightInnerViewport") << QVector3D(0.5, -0.5f, 1.0f) << innerViewport << QPointF(320.0f, 320.0f);
        QTest::newRow("OutsideTopLeftInnerViewport") << QVector3D(-2.0f, 2.0f, 1.0f) << innerViewport<< QPointF(128.0f, 128.0f);
        QTest::newRow("OutsideTopRightInnerViewport") << QVector3D(2.0f, 2.0f, 1.0f) << innerViewport << QPointF(384.0f, 128.0f);
        QTest::newRow("OutsideBottomLeftInnerViewport") << QVector3D(-2.0f, -2.0f, 1.0f) << innerViewport << QPointF(128.0f, 384.0f);
        QTest::newRow("OutsideBottomRightInnerViewport") << QVector3D(2.0f, -2.0f, 1.0f) << innerViewport << QPointF(384.0f, 384.0f);
    }

    void checkClampsScreenPosition()
    {
        // GIVEN
        QFETCH(QVector3D, translation);
        QFETCH(QRectF, viewport);
        QFETCH(QPointF, expectedScreenPosition);
        Kuesa::TransformTracker tracker;

        Kuesa::SceneEntity scene;
        // Note: we ignore transforms that are not reference by an Entity
        Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(&scene);
        Qt3DCore::QTransform *t = new Qt3DCore::QTransform;

        entity->addComponent(t);
        scene.transforms()->add(QStringLiteral("Transform"), t);
        t->setTranslation(QVector3D(0.0f, 0.0f, 1.0f));

        Qt3DRender::QCamera camera;
        camera.setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
        camera.setLeft(-1.0f);
        camera.setRight(1.0f);
        camera.setTop(1.0f);
        camera.setBottom(-1.0f);

        tracker.setName(QStringLiteral("Transform"));
        tracker.setCamera(&camera);
        tracker.setViewportRect(viewport);
        tracker.setSceneEntity(&scene);
        tracker.setScreenSize({512, 512});

        // WHEN
        t->setTranslation(translation);
        updateWorldMatrixOnTransform(t);

        // THEN
        QCOMPARE(tracker.screenPosition(), expectedScreenPosition);
    }

    void checkUpdateNodesOnSceneEntityLoaded()
    {
        // GIVEN
        Kuesa::TransformTracker tracker;
        Kuesa::SceneEntity scene;
        Qt3DCore::QTransform t;

        t.setScale3D(QVector3D(5.0f, 5.0f, 5.0f));
        scene.transforms()->add(QStringLiteral("MyTransform"), &t);

        // WHEN
        tracker.setName(QStringLiteral("MyTransform"));

        // THEN
        QCOMPARE(tracker.scale3D(), QVector3D());

        // WHEN
        tracker.setSceneEntity(&scene);

        // THEN
        QCOMPARE(tracker.scale3D(), QVector3D(5.0f, 5.0f, 5.0f));
    }
};

QTEST_MAIN(tst_TransformTracker)
#include "tst_transformtracker.moc"
