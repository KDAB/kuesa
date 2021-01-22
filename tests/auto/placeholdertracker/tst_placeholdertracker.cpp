/*
    tst_transformtracker.cpp

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

#include <QtTest/QTest>
#include <QSignalSpy>

#include <Kuesa/placeholdertracker.h>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/private/qtransform_p.h>
#include <Kuesa/private/kuesa_utils_p.h>

namespace {

void updateWorldMatrixOnTransform(Qt3DCore::QTransform *t)
{
    Qt3DCore::QTransformPrivate *d = static_cast<Qt3DCore::QTransformPrivate *>(Qt3DCore::QNodePrivate::get(t));
    d->setWorldMatrix(t->matrix());
}

}

class tst_PlaceholderTracker : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::PlaceholderTracker tracker;

        // THEN
        QVERIFY(tracker.name().isEmpty());
        QVERIFY(tracker.camera() == nullptr);
        QVERIFY(tracker.target() == nullptr);
        QCOMPARE(tracker.screenSize(), QSize());
        QCOMPARE(tracker.viewportRect(), QRectF());
        QCOMPARE(tracker.x(), 0);
        QCOMPARE(tracker.y(), 0);
        QCOMPARE(tracker.width(), 0);
        QCOMPARE(tracker.height(), 0);
        QCOMPARE(tracker.screenPosition(), QPointF());
    }

    void checkSetViewportRect()
    {
        // GIVEN
        Kuesa::PlaceholderTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::PlaceholderTracker::viewportRectChanged);

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
        Kuesa::PlaceholderTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::PlaceholderTracker::screenSizeChanged);

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

    void checkSetName()
    {
        // GIVEN
        Kuesa::PlaceholderTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::PlaceholderTracker::nameChanged);
        QSignalSpy screenPositionChangedSpy(&tracker, &Kuesa::PlaceholderTracker::screenPositionChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        tracker.setName(QStringLiteral("MyPlaceholder"));

        // THEN
        QCOMPARE(spy.count(), 1);
        QCOMPARE(tracker.name(), QStringLiteral("MyPlaceholder"));

        // WHEN
        tracker.setName(QStringLiteral("MyPlaceholder"));

        // THEN
        QCOMPARE(spy.count(), 1);
        QCOMPARE(screenPositionChangedSpy.count(), 0);

        // WHEN
        Kuesa::SceneEntity scene;
        Kuesa::Placeholder p;
        Qt3DCore::QTransform *placeHolderTransform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(&p);
        placeHolderTransform->setTranslation({90.0f, 25.0f, 0.0f});
        scene.placeholders()->add(QStringLiteral("MyPlaceholder2"), &p);

        tracker.setSceneEntity(&scene);
        tracker.setName(QStringLiteral("MyPlaceholder2"));

        // THEN
        QCOMPARE(spy.count(), 2);
        QCOMPARE(screenPositionChangedSpy.count(), 1);

        // WHEN
        tracker.setName(QStringLiteral("MyPlaceholder"));

        // THEN
        QCOMPARE(spy.count(), 3);
        QCOMPARE(screenPositionChangedSpy.count(), 2);
    }

    void checkSetCamera()
    {
        // GIVEN
        Kuesa::PlaceholderTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::PlaceholderTracker::cameraChanged);

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

    void testTarget()
    {
        // GIVEN
        Kuesa::PlaceholderTracker tracker;
        QSignalSpy spy(&tracker, &Kuesa::PlaceholderTracker::targetChanged);
        QVERIFY(spy.isValid());

        // WHEN
        QObject *target = new QObject;
        tracker.setTarget(target);

        // THEN
        QCOMPARE(tracker.target(), target);
        QCOMPARE(spy.count(), 1);

        // WHEN
        tracker.setTarget(target);

        // THEN
        QCOMPARE(spy.count(), 1);

        // WHEN -> camera destroyed
        delete target;

        // THEN
        QCOMPARE(tracker.target(), nullptr);
        QCOMPARE(spy.count(), 2);
    }

    void checkUpdatesTarget()
    {
        // GIVEN
        Kuesa::SceneEntity sceneEntity;
        Kuesa::PlaceholderTracker placeholderTracker;
        Kuesa::Placeholder placeHolder;
        Qt3DCore::QTransform *placeHolderTransform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(&placeHolder);
        QObject *target = new QObject();
        Qt3DRender::QCamera *camera = new Qt3DRender::QCamera;

        auto simulateBackendWorldMatrixUpdate = [&] (Qt3DCore::QTransform *t) {
            Qt3DCore::QTransformPrivate *d = static_cast<Qt3DCore::QTransformPrivate *>(Qt3DCore::QNodePrivate::get(t));
            d->setWorldMatrix(t->matrix());
        };

        // THEN
        QVERIFY(placeHolderTransform);

        // WHEN
        camera->setPosition(QVector3D(0.0f, 0.0f, -10.0f));
        camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
        camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));

        camera->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
        camera->setLeft(-1.0f);
        camera->setTop(1.0f);
        camera->setBottom(-1.0f);
        camera->setRight(1.0f);

        placeHolderTransform->setScale(2.0f);

        placeholderTracker.setScreenSize({800, 600});
        placeholderTracker.setCamera(camera);
        placeholderTracker.setTarget(target);
        placeholderTracker.setName(QStringLiteral("Placeholder"));

        simulateBackendWorldMatrixUpdate(placeHolderTransform);
        simulateBackendWorldMatrixUpdate(camera->transform());

        sceneEntity.placeholders()->add(placeholderTracker.name(), &placeHolder);
        placeholderTracker.setSceneEntity(&sceneEntity);

        // THEN
        QCOMPARE(target->property("x").toInt(), -400);
        QCOMPARE(target->property("y").toInt(), -300);
        QCOMPARE(target->property("width").toInt(), 1600);
        QCOMPARE(target->property("height").toInt(), 1200);
        QCOMPARE(placeholderTracker.x(), target->property("x").toInt());
        QCOMPARE(placeholderTracker.y(), target->property("y").toInt());
        QCOMPARE(placeholderTracker.width(), target->property("width").toInt());
        QCOMPARE(placeholderTracker.height(), target->property("height").toInt());

        // WHEN
        placeHolderTransform->setScale(1.0f);
        simulateBackendWorldMatrixUpdate(placeHolderTransform);

        // THEN
        QCOMPARE(target->property("x").toInt(), 0);
        QCOMPARE(target->property("y").toInt(), 0);
        QCOMPARE(target->property("width").toInt(), 800);
        QCOMPARE(target->property("height").toInt(), 600);
        QCOMPARE(placeholderTracker.x(), target->property("x").toInt());
        QCOMPARE(placeholderTracker.y(), target->property("y").toInt());
        QCOMPARE(placeholderTracker.width(), target->property("width").toInt());
        QCOMPARE(placeholderTracker.height(), target->property("height").toInt());

        // WHEN
        placeholderTracker.setScreenSize({400, 300});

        // THEN
        QCOMPARE(target->property("x").toInt(), 0);
        QCOMPARE(target->property("y").toInt(), 0);
        QCOMPARE(target->property("width").toInt(), 400);
        QCOMPARE(target->property("height").toInt(), 300);
        QCOMPARE(placeholderTracker.x(), target->property("x").toInt());
        QCOMPARE(placeholderTracker.y(), target->property("y").toInt());
        QCOMPARE(placeholderTracker.width(), target->property("width").toInt());
        QCOMPARE(placeholderTracker.height(), target->property("height").toInt());

        // WHEN
        placeholderTracker.setScreenSize({800, 600});
        placeholderTracker.setViewportRect({0.5f, 0.5f, 0.5f, 0.5f});

        // THEN
        QCOMPARE(target->property("x").toInt(), 400);
        QCOMPARE(target->property("y").toInt(), 300);
        QCOMPARE(target->property("width").toInt(), 400);
        QCOMPARE(target->property("height").toInt(), 300);
        QCOMPARE(placeholderTracker.x(), target->property("x").toInt());
        QCOMPARE(placeholderTracker.y(), target->property("y").toInt());
        QCOMPARE(placeholderTracker.width(), target->property("width").toInt());
        QCOMPARE(placeholderTracker.height(), target->property("height").toInt());

        // WHEN
        placeholderTracker.setViewportRect({0.0f, 0.0f, 1.0f, 1.0f});
        camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);

        // THEN
        QCOMPARE(target->property("x").toInt(), 220);
        QCOMPARE(target->property("y").toInt(), 165);
        QCOMPARE(target->property("width").toInt(),360);
        QCOMPARE(target->property("height").toInt(), 270);
        QCOMPARE(placeholderTracker.x(), target->property("x").toInt());
        QCOMPARE(placeholderTracker.y(), target->property("y").toInt());
        QCOMPARE(placeholderTracker.width(), target->property("width").toInt());
        QCOMPARE(placeholderTracker.height(), target->property("height").toInt());
    }

    void checkHandlesInvalidCameraSanely()
    {
        // GIVEN
        Kuesa::PlaceholderTracker placeholderTracker;
        QObject target;
        placeholderTracker.setTarget(&target);

        {
            // WHEN
            Qt3DCore::QEntity e;
            placeholderTracker.setCamera(&e);

            // THEN -> shouldnt Crash
        }

        {
            // WHEN
            Qt3DCore::QEntity e;
            Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
            e.addComponent(transform);
            placeholderTracker.setCamera(&e);

            // THEN -> shouldnt Crash
        }

        {
            // WHEN
            Qt3DCore::QEntity e;
            Qt3DRender::QCameraLens *lens = new Qt3DRender::QCameraLens;
            e.addComponent(lens);
            placeholderTracker.setCamera(&e);

            // THEN -> shouldnt Crash
        }
    }

    void checkHandlesSceneEntityDestruction()
    {
        // GIVEN
        Kuesa::PlaceholderTracker tracker;
        Kuesa::Placeholder *placeholder = new Kuesa::Placeholder; // owned by collection

        // WHEN
        {
            Kuesa::SceneEntity scene;
            scene.placeholders()->add(QStringLiteral("MyPlaceholder"), placeholder);

            tracker.setSceneEntity(&scene);
            tracker.setName(QStringLiteral("MyPlaceholder"));

            // THEN -> Should have updated matrix from new Transform match
            QCOMPARE(tracker.sceneEntity(), &scene);
            QCOMPARE(tracker.x(), 0.0f);
        }

        // THEN
        QVERIFY(tracker.sceneEntity() == nullptr);
        QCOMPARE(tracker.x(), 0.0f);
    }

    void checkUpdateNodesOnSceneEntityLoaded()
    {
        // GIVEN
        Kuesa::PlaceholderTracker tracker;
        Kuesa::SceneEntity scene;
        Kuesa::Placeholder p;

        scene.placeholders()->add(QStringLiteral("MyPlaceholder"), &p);

        // WHEN
        tracker.setName(QStringLiteral("MyPlaceholder"));

        // THEN
        QCOMPARE(tracker.x(), 0.0f);

        // WHEN
        tracker.setSceneEntity(&scene);

        // THEN
        QCOMPARE(tracker.x(), 0.0f);
    }
};

QTEST_MAIN(tst_PlaceholderTracker)
#include "tst_placeholdertracker.moc"
