/*
    tst_view3dscene.cpp

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

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <KuesaUtils/view3dscene.h>
#include <Kuesa/Iro2PlanarReflectionSemProperties>

class tst_View3DScene : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        KuesaUtils::View3DScene view;

        // THEN
        QVERIFY(view.importer());
        QVERIFY(view.frameGraph());
        QCOMPARE(view.source(), QUrl());
        QCOMPARE(view.cameraName(), QString());
        QCOMPARE(view.showDebugOverlay(), false);
        QCOMPARE(view.screenSize(), QSize());
        QCOMPARE(view.isReady(), false);
        QCOMPARE(view.isLoaded(), false);
        QCOMPARE(view.asynchronous(), false);
        QVERIFY(view.activeScene() == nullptr);
        QVERIFY(view.animationPlayers().empty());
        QVERIFY(view.transformTrackers().empty());
        QVERIFY(view.reflectionPlaneName().isEmpty());
    }

    void checkSetSource()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration scene;

        QSignalSpy sourceChangedSpy(&view, &KuesaUtils::View3DScene::sourceChanged);

        // THEN
        QVERIFY(sourceChangedSpy.isValid());

        // WHEN
        const QUrl newSource = QUrl("some/bad/path");
        scene.setSource(newSource);
        view.setActiveScene(&scene);

        // THEN
        QCOMPARE(view.source(), newSource);
        QCOMPARE(sourceChangedSpy.count(), 1);
        QCOMPARE(view.source(), view.importer()->source());
    }

    void checkSetCameraName()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration scene;
        QSignalSpy cameraNameChangedSpy(&view, &KuesaUtils::View3DScene::cameraNameChanged);

        // THEN
        QVERIFY(cameraNameChangedSpy.isValid());

        // WHEN
        const QString newCameraName = QStringLiteral("Camera_Orientation");
        scene.setCameraName(newCameraName);
        view.setActiveScene(&scene);

        // THEN
        QCOMPARE(view.cameraName(), newCameraName);
        QCOMPARE(cameraNameChangedSpy.count(), 1);
    }

    void checkSetAutoloadReflections()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        QSignalSpy reflectionPlaneNameChangedSpy(&view, &KuesaUtils::View3DScene::reflectionPlaneNameChanged);

        // THEN
        QVERIFY(reflectionPlaneNameChangedSpy.isValid());

        // WHEN
        view.setReflectionPlaneName(QStringLiteral("reflection"));

        // THEN
        QCOMPARE(view.reflectionPlaneName(), QStringLiteral("reflection"));
        QCOMPARE(reflectionPlaneNameChangedSpy.count(), 1);
    }

    void checkSetScreenSize()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration scene;
        Kuesa::TransformTracker tracker;
        QSignalSpy screenSizeChangedSpy(&view, &KuesaUtils::View3DScene::screenSizeChanged);
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        // THEN
        QVERIFY(loadedSpy.isValid());
        scene.setSource(QUrl("file:///" ASSETS "Box.gltf"));
        scene.addTransformTracker(&tracker);

        view.setActiveScene(&scene);

        // WHEN
        loadedSpy.wait();

        // THEN
        QCOMPARE(tracker.screenSize(), QSize());
        QVERIFY(screenSizeChangedSpy.isValid());
        QCOMPARE(view.transformTrackers().size(), 1UL);

        // WHEN
        const QSize newScreenSize(512, 512);
        view.setScreenSize(newScreenSize);

        // THEN
        QCOMPARE(view.screenSize(), newScreenSize);
        QCOMPARE(screenSizeChangedSpy.count(), 1);
        QCOMPARE(tracker.screenSize(), newScreenSize);
    }

    void checkSetDebugOverlay()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        QSignalSpy showDebugOverlayChangedSpy(&view, &KuesaUtils::View3DScene::showDebugOverlayChanged);

        // THEN
        QVERIFY(showDebugOverlayChangedSpy.isValid());

        // WHEN
        view.setShowDebugOverlay(true);

        // THEN
        QVERIFY(view.showDebugOverlay());
        QCOMPARE(showDebugOverlayChangedSpy.count(), 1);
        QVERIFY(view.frameGraph()->showDebugOverlay());
    }

    void checkSetAsynchronous()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        QSignalSpy asynchronousChangedSpy(&view, &KuesaUtils::View3DScene::asynchronousChanged);

        // THEN
        QVERIFY(asynchronousChangedSpy.isValid());
        QCOMPARE(view.asynchronous(), false);
        QCOMPARE(view.importer()->asynchronous(), false);

        // WHEN
        view.setAsynchronous(true);

        // THEN
        QVERIFY(view.asynchronous());
        QCOMPARE(asynchronousChangedSpy.count(), 1);
        QVERIFY(view.importer()->asynchronous());
    }

    void checkTransformTrackers()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration scene;
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        QVERIFY(loadedSpy.isValid());
        scene.setSource(QUrl("file:///" ASSETS "Box.gltf"));
        view.setActiveScene(&scene);
        loadedSpy.wait();

        // WHEN
        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            scene.addTransformTracker(&t1);
            scene.addTransformTracker(&t2);

            // THEN
            QCOMPARE(view.transformTrackers().size(), 2UL);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(view.transformTrackers().size(), 0UL);

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            scene.addTransformTracker(&t1);
            scene.addTransformTracker(&t1);

            // THEN
            QCOMPARE(view.transformTrackers().size(), 1UL);

            //WHEN
            scene.removeTransformTracker(&t2);

            // THEN
            QCOMPARE(view.transformTrackers().size(), 1UL);
        }

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            scene.addTransformTracker(&t1);
            scene.addTransformTracker(&t2);

            // THEN
            QCOMPARE(view.transformTrackers().size(), 2UL);

            // WHEN
            scene.clearTransformTrackers();

            // THEN
            QCOMPARE(view.transformTrackers().size(), 0UL);
        }

        // THEN -> Shouldn't crash

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            scene.addTransformTracker(&t1);
            scene.addTransformTracker(&t2);

            // THEN
            QCOMPARE(view.transformTrackers().size(), 2UL);

            // WHEN
            scene.removeTransformTracker(&t1);

            // THEN
            QCOMPARE(view.transformTrackers().size(), 1UL);
            QCOMPARE(view.transformTrackers().front(), &t2);
        }

        // THEN -> Shouldn't crash
    }

    void checkAnimationPlayers()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration scene;
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        QVERIFY(loadedSpy.isValid());
        scene.setSource(QUrl("file:///" ASSETS "Box.gltf"));
        view.setActiveScene(&scene);
        loadedSpy.wait();

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            scene.addAnimationPlayer(&t1);
            scene.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(view.animationPlayers().size(), 2UL);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(view.animationPlayers().size(), 0UL);

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            scene.addAnimationPlayer(&t1);
            scene.addAnimationPlayer(&t1);

            // THEN
            QCOMPARE(view.animationPlayers().size(), 1UL);

            // WHEN
            scene.removeAnimationPlayer(&t2);
            QCOMPARE(view.animationPlayers().size(), 1UL);
        }

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            scene.addAnimationPlayer(&t1);
            scene.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(view.animationPlayers().size(), 2UL);

            // WHEN
            scene.clearAnimationPlayers();

            // THEN
            QCOMPARE(view.animationPlayers().size(), 0UL);
        }

        // THEN -> Shouldn't crash

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            scene.addAnimationPlayer(&t1);
            scene.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(view.animationPlayers().size(), 2UL);

            // WHEN
            scene.removeAnimationPlayer(&t1);

            // THEN
            QCOMPARE(view.animationPlayers().size(), 1UL);
            QCOMPARE(view.animationPlayers().front(), &t2);
        }

        // THEN -> Shouldn't crash
    }

    void checkAnimationPlayback()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration scene;
        Kuesa::AnimationPlayer t1;
        Kuesa::AnimationPlayer t2;
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        QVERIFY(loadedSpy.isValid());
        scene.setSource(QUrl("file:///" ASSETS "Box.gltf"));
        view.setActiveScene(&scene);
        loadedSpy.wait();

        scene.addAnimationPlayer(&t1);
        scene.addAnimationPlayer(&t2);

        // THEN
        QCOMPARE(view.animationPlayers().size(), 2UL);

        // WHEN
        view.gotoEnd();

        // THEN
        QCOMPARE(t1.normalizedTime(), 1.0f);
        QCOMPARE(t2.normalizedTime(), 1.0f);

        // WHEN
        view.gotoStart();

        //THEN
        QCOMPARE(t1.normalizedTime(), 0.0f);
        QCOMPARE(t2.normalizedTime(), 0.0f);

        // WHEN
        view.gotoNormalizedTime(0.5f);

        // THEN
        QCOMPARE(t1.normalizedTime(), 0.5f);
        QCOMPARE(t2.normalizedTime(), 0.5f);

        // Can't test without valid clips
        //        // WHEN
        //        view.start();

        //        // THEN
        //        QCOMPARE(t1.isRunning(), true);
        //        QCOMPARE(t2.isRunning(), true);

        //        // WHEN
        //        view.stop();

        //        // THEN
        //        QCOMPARE(t1.isRunning(), false);
        //        QCOMPARE(t2.isRunning(), false);

        //        // WHEN
        //        view.restart();

        //        // THEN
        //        QCOMPARE(t1.normalizedTime(), 0.0f);
        //        QCOMPARE(t2.normalizedTime(), 0.0f);
        //        QCOMPARE(t1.isRunning(), true);
        //        QCOMPARE(t2.isRunning(), true);
    }

    void checkSceneConfiguration()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        // THEN
        QVERIFY(loadedSpy.isValid());

        {
            // WHEN
            KuesaUtils::SceneConfiguration c1;

            Kuesa::AnimationPlayer a1;
            Kuesa::AnimationPlayer a2;
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            c1.addAnimationPlayer(&a1);
            c1.addAnimationPlayer(&a2);
            c1.addTransformTracker(&t1);
            c1.addTransformTracker(&t2);
            c1.setCameraName(QStringLiteral("Camera_Orientation"));
            c1.setSource(QUrl("file:///" ASSETS "Box.gltf"));

            view.setActiveScene(&c1);
            loadedSpy.wait();

            // THEN
            QCOMPARE(view.activeScene(), &c1);
            QCOMPARE(view.animationPlayers().size(), 2UL);
            QCOMPARE(view.transformTrackers().size(), 2UL);
            QCOMPARE(view.source(), QUrl("file:///" ASSETS "Box.gltf"));
            QCOMPARE(view.cameraName(), QStringLiteral("Camera_Orientation"));
        }

        // THEN
        QVERIFY(view.activeScene() == nullptr);
        QCOMPARE(view.animationPlayers().size(), 0UL);
        QCOMPARE(view.transformTrackers().size(), 0UL);
        QCOMPARE(view.source(), QUrl());
        QCOMPARE(view.cameraName(), QString());
    }

    void checkAutoloadReflections()
    {
        // Loading with autoloadReflection set to false initially
        {
            // GIVEN
            KuesaUtils::View3DScene view;
            KuesaUtils::SceneConfiguration scene;
            QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadedChanged);

            // THEN
            QVERIFY(loadedSpy.isValid());

            // WHEN
            scene.setCameraName(QStringLiteral("Camera_Orientation"));
            scene.setSource(QUrl("file:///" ASSETS "KDAB_reflection_planes/reflections_scene.gltf"));
            view.setActiveScene(&scene);
            loadedSpy.wait();

            // THEN
            QVERIFY(view.isLoaded());
            QCOMPARE(view.reflectionPlanes()->size(), 1);
            QCOMPARE(view.frameGraph()->reflectionPlanes().size(), 0UL);

            Kuesa::Iro2PlanarReflectionSemProperties *material = qobject_cast<Kuesa::Iro2PlanarReflectionSemProperties *>(view.material(QStringLiteral("MaterialReflection")));
            QVERIFY(material);

            // WHEN
            view.setReflectionPlaneName(QStringLiteral("ReflectionPlane"));

            // THEN
            QCOMPARE(view.frameGraph()->reflectionPlanes().size(), 1UL);
        }

        // Loading with autoloadReflection set to true
        {
            // GIVEN
            KuesaUtils::View3DScene view;
            KuesaUtils::SceneConfiguration scene;
            QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadedChanged);

            // THEN
            QVERIFY(loadedSpy.isValid());

            // WHEN
            view.setReflectionPlaneName(QStringLiteral("ReflectionPlane"));
            scene.setCameraName(QStringLiteral("Camera_Orientation"));
            scene.setSource(QUrl("file:///" ASSETS "KDAB_reflection_planes/reflections_scene.gltf"));
            view.setActiveScene(&scene);
            loadedSpy.wait();

            // THEN
            QVERIFY(view.isLoaded());
            QCOMPARE(view.reflectionPlanes()->size(), 1);
            QCOMPARE(view.frameGraph()->reflectionPlanes().size(), 1UL);
        }
    }
};

QTEST_MAIN(tst_View3DScene)
#include "tst_view3dscene.moc"
