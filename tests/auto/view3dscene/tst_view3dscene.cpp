/*
    tst_view3dscene.cpp

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
#include <QtTest/QSignalSpy>
#include <KuesaUtils/view3dscene.h>
#include <KuesaUtils/sceneconfiguration.h>
#include <KuesaUtils/viewconfiguration.h>
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
        QCOMPARE(view.showDebugOverlay(), false);
        QCOMPARE(view.screenSize(), QSize());
        QCOMPARE(view.isReady(), false);
        QCOMPARE(view.isLoaded(), false);
        QCOMPARE(view.asynchronous(), false);
        QVERIFY(view.activeScene() == nullptr);
        QVERIFY(view.animationPlayers().empty());
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
        KuesaUtils::ViewConfiguration viewConfiguration;
        Kuesa::TransformTracker transformTracker;
        Kuesa::PlaceholderTracker placeholderTracker;
        QSignalSpy screenSizeChangedSpy(&view, &KuesaUtils::View3DScene::screenSizeChanged);
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        // THEN
        QVERIFY(loadedSpy.isValid());
        viewConfiguration.addTransformTracker(&transformTracker);
        viewConfiguration.addPlaceholderTracker(&placeholderTracker);
        scene.addViewConfiguration(&viewConfiguration);
        scene.setSource(QUrl("file:///" ASSETS "Box.gltf"));

        view.setActiveScene(&scene);

        // WHEN
        loadedSpy.wait();

        // THEN
        QCOMPARE(transformTracker.screenSize(), QSize());
        QCOMPARE(placeholderTracker.screenSize(), QSize());
        QVERIFY(screenSizeChangedSpy.isValid());

        // WHEN
        const QSize newScreenSize(512, 512);
        view.setScreenSize(newScreenSize);

        // THEN
        QCOMPARE(view.screenSize(), newScreenSize);
        QCOMPARE(screenSizeChangedSpy.count(), 1);
        QCOMPARE(transformTracker.screenSize(), newScreenSize);
        QCOMPARE(placeholderTracker.screenSize(), newScreenSize);
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

    void checkTransformTrackersInViews()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration scene;
        KuesaUtils::ViewConfiguration v1;
        KuesaUtils::ViewConfiguration v2;
        scene.addViewConfiguration(&v1);
        scene.addViewConfiguration(&v2);
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        QVERIFY(loadedSpy.isValid());
        scene.setSource(QUrl("file:///" ASSETS "Box.gltf"));
        view.setActiveScene(&scene);
        loadedSpy.wait();

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            v1.addTransformTracker(&t1);
            v2.addTransformTracker(&t2);

            // THEN
            // Both trackers should have an undefined camera since we haven't set any in the ViewConfiguration
            QCOMPARE(t1.camera(), nullptr);
            QCOMPARE(t2.camera(), nullptr);
        }

        // THEN -> Shouldn't crash and should have remove trackers

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            v1.addTransformTracker(&t1);
            view.setScreenSize(QSize(100, 100));
            v2.addTransformTracker(&t2);

            // THEN
            // Trackers should have been notified about the new screen size
            QCOMPARE(t1.screenSize(), QSize(100, 100));
            QCOMPARE(t2.screenSize(), QSize(100, 100));
        }

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            v1.setViewportRect(QRectF(0, 0, 1, 1));
            v1.addTransformTracker(&t1);
            v2.addTransformTracker(&t2);
            v2.setViewportRect(QRectF(0, 0, 0.5, 0.5));

            // THEN
            // Trackers should have been notified about the new screen size
            QCOMPARE(t1.viewportRect(), QRectF(0, 0, 1, 1));
            QCOMPARE(t2.viewportRect(), QRectF(0, 0, 0.5, 0.5));
        }
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

            c1.addAnimationPlayer(&a1);
            c1.addAnimationPlayer(&a2);
            c1.setSource(QUrl("file:///" ASSETS "Box.gltf"));

            view.setActiveScene(&c1);
            loadedSpy.wait();

            // THEN
            QCOMPARE(view.activeScene(), &c1);
            QCOMPARE(view.animationPlayers().size(), 2UL);
            QCOMPARE(view.source(), QUrl("file:///" ASSETS "Box.gltf"));
        }

        // THEN
        QVERIFY(view.activeScene() == nullptr);
        QCOMPARE(view.animationPlayers().size(), 0UL);
        QCOMPARE(view.source(), QUrl());
    }

    void checkSceneConfigurationWithViews()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadingDone);

        // THEN
        QVERIFY(loadedSpy.isValid());

        {
            // WHEN
            KuesaUtils::SceneConfiguration c1;
            KuesaUtils::SceneConfiguration c2;

            KuesaUtils::ViewConfiguration v1;
            v1.setViewportRect({ 0.5f, 0.5f, 1.0f, 1.0f });
            KuesaUtils::ViewConfiguration v2;
            v2.setViewportRect({ 0.25f, 0.25f, 1.0f, 1.0f });
            KuesaUtils::ViewConfiguration v3;

            c1.addViewConfiguration(&v1);
            c2.addViewConfiguration(&v2);
            c2.addViewConfiguration(&v3);

            c1.setSource(QUrl("file:///" ASSETS "Box.gltf"));
            c2.setSource(QUrl("file:///" ASSETS "Box.gltf"));

            view.setActiveScene(&c1);
            loadedSpy.wait();

            // THEN
            QCOMPARE(view.activeScene(), &c1);
            QCOMPARE(view.frameGraph()->views().size(), size_t(1));
            QCOMPARE(view.frameGraph()->views()[0]->viewportRect(), v1.viewportRect());

            // WHEN
            view.setActiveScene(&c2);
            loadedSpy.wait();

            // THEN
            QCOMPARE(view.activeScene(), &c2);
            QCOMPARE(view.frameGraph()->views().size(), size_t(2));
            QCOMPARE(view.frameGraph()->views()[0]->viewportRect(), v2.viewportRect());
            QCOMPARE(view.frameGraph()->views()[1]->viewportRect(), v3.viewportRect());

            // WHEN
            view.setActiveScene(&c1);
            loadedSpy.wait();

            // THEN
            QCOMPARE(view.activeScene(), &c1);
            QCOMPARE(view.frameGraph()->views().size(), size_t(1));
            QCOMPARE(view.frameGraph()->views()[0]->viewportRect(), v1.viewportRect());

            // WHEN
            view.setActiveScene(&c2);
            loadedSpy.wait();
            c2.removeViewConfiguration(&v2);

            // THEN
            QCOMPARE(view.activeScene(), &c2);
            QCOMPARE(view.frameGraph()->views().size(), size_t(1));
            QCOMPARE(view.frameGraph()->views()[0]->viewportRect(), v3.viewportRect());

            // WHEN
            c2.addViewConfiguration(&v2);

            // THEN
            QCOMPARE(view.frameGraph()->views().size(), size_t(2));
            QCOMPARE(view.frameGraph()->views()[0]->viewportRect(), v3.viewportRect());
            QCOMPARE(view.frameGraph()->views()[1]->viewportRect(), v2.viewportRect());
        }

        // THEN
        QVERIFY(view.activeScene() == nullptr);
        QCOMPARE(view.frameGraph()->views().size(), size_t(0));
        QCOMPARE(view.animationPlayers().size(), 0UL);
        QCOMPARE(view.source(), QUrl());
    }

    void checkClearAndUnloadOnSceneConfigurationChange()
    {
        // GIVEN
        KuesaUtils::View3DScene view;

        {
            // WHEN
            KuesaUtils::SceneConfiguration c1;
            KuesaUtils::ViewConfiguration v1;
            c1.addViewConfiguration(&v1);
            KuesaUtils::SceneConfiguration c2;
            KuesaUtils::ViewConfiguration v2;
            c2.addViewConfiguration(&v2);

            v1.setCameraName(QStringLiteral("Camera_Orientation"));
            c1.setSource(QUrl("file:///" ASSETS "Box.gltf"));

            v2.setCameraName(QStringLiteral("Camera_Orientation"));
            c2.setSource(QUrl("file:///" ASSETS "Box.gltf"));

            QSignalSpy c1LoadingDoneSpy(&c1, &KuesaUtils::SceneConfiguration::loadingDone);
            QSignalSpy c1UnloadingDoneSpy(&c1, &KuesaUtils::SceneConfiguration::unloadingDone);

            QSignalSpy c2LoadingDoneSpy(&c2, &KuesaUtils::SceneConfiguration::loadingDone);
            QSignalSpy c2UnloadingDoneSpy(&c2, &KuesaUtils::SceneConfiguration::unloadingDone);

            // THEN
            QVERIFY(c1LoadingDoneSpy.isValid());
            QVERIFY(c1UnloadingDoneSpy.isValid());

            QVERIFY(c2LoadingDoneSpy.isValid());
            QVERIFY(c2UnloadingDoneSpy.isValid());

            // WHEN
            view.setActiveScene(&c1);
            c1LoadingDoneSpy.wait();

            // THEN
            QCOMPARE(c1LoadingDoneSpy.count(), 1);
            QCOMPARE(c1UnloadingDoneSpy.count(), 0);
            QCOMPARE(c2LoadingDoneSpy.count(), 0);
            QCOMPARE(c2UnloadingDoneSpy.count(), 0);
            Qt3DRender::QGeometryRenderer *mesh0C1 = view.mesh(QStringLiteral("Mesh_0"));
            QVERIFY(mesh0C1);

            // WHEN
            view.setActiveScene(&c2);
            c2LoadingDoneSpy.wait();

            // THEN
            QCOMPARE(c1LoadingDoneSpy.count(), 1);
            QCOMPARE(c1UnloadingDoneSpy.count(), 1);
            QCOMPARE(c2LoadingDoneSpy.count(), 1);
            QCOMPARE(c2UnloadingDoneSpy.count(), 0);
            Qt3DRender::QGeometryRenderer *mesh0C2 = view.mesh(QStringLiteral("Mesh_0"));
            QVERIFY(mesh0C1);

            // Different pointers because collection was cleared and reloaded
            QVERIFY(mesh0C1 != mesh0C2);
        }

        // THEN
        QVERIFY(view.activeScene() == nullptr);
        QCOMPARE(view.animationPlayers().size(), 0UL);
        QCOMPARE(view.source(), QUrl());
    }

    void checkSceneConfigurationParenting()
    {
        // GIVEN
        KuesaUtils::View3DScene view;

        {
            // WHEN -> Scene with no parent
            KuesaUtils::SceneConfiguration conf;

            // THEN
            QVERIFY(conf.parentNode() == nullptr);
            QVERIFY(conf.parent() == nullptr);

            // WHEN
            view.setActiveScene(&conf);

            // THEN
            QVERIFY(conf.parentNode() == &view);
            QVERIFY(conf.parent() == &view);

            // WHEN -> Switching scene
            view.setActiveScene(nullptr);

            // THEN -> Parent should have been reset
            QVERIFY(conf.parentNode() == nullptr);
            QVERIFY(conf.parent() == nullptr);
        }

        {
            // WHEN -> Scene with QNode parent
            Qt3DCore::QNode node;
            KuesaUtils::SceneConfiguration *conf = new KuesaUtils::SceneConfiguration(&node);

            // THEN
            QVERIFY(conf->parentNode() == &node);
            QVERIFY(conf->parent() == &node);

            // WHEN
            view.setActiveScene(conf);

            // THEN
            QVERIFY(conf->parentNode() == &node);
            QVERIFY(conf->parent() == &node);

            // WHEN -> Switching scene
            view.setActiveScene(nullptr);

            // THEN -> Parent should have been restored/untouched
            QVERIFY(conf->parentNode() == &node);
            QVERIFY(conf->parent() == &node);
        }

        {
            // WHEN -> Scene with QNode parent
            QObject node;
            KuesaUtils::SceneConfiguration *conf = new KuesaUtils::SceneConfiguration();
            static_cast<QObject *>(conf)->setParent(&node);

            // THEN
            QVERIFY(conf->parentNode() == nullptr);
            QVERIFY(conf->parent() == &node);

            // WHEN
            view.setActiveScene(conf);

            // THEN -> Was parented to view to be picked up by Qt 3D
            QVERIFY(conf->parentNode() == &view);
            QVERIFY(conf->parent() == &view);

            // WHEN -> Switching scene
            view.setActiveScene(nullptr);

            // THEN -> Parent should have been restored/untouched
            QVERIFY(conf->parentNode() == nullptr);
            QVERIFY(conf->parent() == &node);
        }
    }

    void checkCameraReloadOnCameraNameChange()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration sceneConfiguration;
        KuesaUtils::ViewConfiguration viewConfiguration;
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadedChanged);

        // THEN
        QVERIFY(loadedSpy.isValid());

        // WHEN
        viewConfiguration.setCameraName(QStringLiteral("Camera_First"));
        sceneConfiguration.setSource(QUrl("file:///" ASSETS "simple_cube_multi_cameras.gltf"));
        view.setActiveScene(&sceneConfiguration);
        sceneConfiguration.addViewConfiguration(&viewConfiguration);
        loadedSpy.wait();

        // THEN
        QCOMPARE(view.frameGraph()->views().size(), size_t(1));
        auto *firstCamera = view.frameGraph()->views()[0]->camera();
        QVERIFY(firstCamera != nullptr);

        // WHEN
        viewConfiguration.setCameraName(QStringLiteral("Camera_Second"));

        // THEN
        auto *secondCamera = view.frameGraph()->views()[0]->camera();
        QVERIFY(secondCamera != nullptr);
        QVERIFY(firstCamera != secondCamera);
    }

    void checkAutoloadReflections()
    {
        // Loading with autoloadReflection set to false initially
        {
            // GIVEN
            KuesaUtils::View3DScene view;
            KuesaUtils::SceneConfiguration scene;
            KuesaUtils::ViewConfiguration viewConfiguration;
            QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadedChanged);

            // THEN
            QVERIFY(loadedSpy.isValid());

            // WHEN
            viewConfiguration.setCameraName(QStringLiteral("Camera_Orientation"));
            scene.setSource(QUrl("file:///" ASSETS "KDAB_reflection_planes/reflections_scene.gltf"));
            scene.addViewConfiguration(&viewConfiguration);
            view.setActiveScene(&scene);
            loadedSpy.wait();

            // THEN
            QVERIFY(view.isLoaded());
            QCOMPARE(view.reflectionPlanes()->size(), 1);
            QCOMPARE(view.frameGraph()->views()[0]->reflectionPlanes().size(), 0UL);

            Kuesa::Iro2PlanarReflectionSemProperties *material = qobject_cast<Kuesa::Iro2PlanarReflectionSemProperties *>(view.material(QStringLiteral("MaterialReflection")));
            QVERIFY(material);

            // WHEN
            view.setReflectionPlaneName(QStringLiteral("ReflectionPlane"));

            // THEN
            QCOMPARE(view.frameGraph()->views()[0]->reflectionPlanes().size(), 1UL);
        }

        // Loading with autoloadReflection set to true
        {
            // GIVEN
            KuesaUtils::View3DScene view;
            KuesaUtils::SceneConfiguration scene;
            KuesaUtils::ViewConfiguration viewConfiguration;
            QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadedChanged);

            // THEN
            QVERIFY(loadedSpy.isValid());

            // WHEN
            view.setReflectionPlaneName(QStringLiteral("ReflectionPlane"));
            viewConfiguration.setCameraName(QStringLiteral("Camera_Orientation"));
            scene.setSource(QUrl("file:///" ASSETS "KDAB_reflection_planes/reflections_scene.gltf"));
            scene.addViewConfiguration(&viewConfiguration);
            view.setActiveScene(&scene);
            loadedSpy.wait();

            // THEN
            QVERIFY(view.isLoaded());
            QCOMPARE(view.reflectionPlanes()->size(), 1);
            QCOMPARE(view.frameGraph()->views()[0]->reflectionPlanes().size(), 1UL);
        }
    }

    void checkLayerNames()
    {
        // GIVEN
        KuesaUtils::View3DScene view;
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy loadedSpy(&view, &KuesaUtils::View3DScene::loadedChanged);

        QVERIFY(loadedSpy.isValid());

        {
            // WHEN
            KuesaUtils::ViewConfiguration viewConfiguration1;
            KuesaUtils::ViewConfiguration viewConfiguration2;
            sceneConfiguration.addViewConfiguration(&viewConfiguration1);
            sceneConfiguration.addViewConfiguration(&viewConfiguration2);
            QSignalSpy layerNames1ChangedSpy(&viewConfiguration1, &KuesaUtils::ViewConfiguration::layerNamesChanged);
            QSignalSpy layerNames2ChangedSpy(&viewConfiguration2, &KuesaUtils::ViewConfiguration::layerNamesChanged);

            // THEN
            QVERIFY(layerNames1ChangedSpy.isValid());
            QVERIFY(layerNames2ChangedSpy.isValid());

            // WHEN
            viewConfiguration1.setCameraName(QStringLiteral("Camera_Orientation"));
            sceneConfiguration.setSource(QUrl("file:///" ASSETS "Box.gltf"));

            view.setActiveScene(&sceneConfiguration);
            loadedSpy.wait();

            // Owned by collection
            Qt3DRender::QLayer *layer1 = new Qt3DRender::QLayer;
            Qt3DRender::QLayer *layer2 = new Qt3DRender::QLayer;
            Qt3DRender::QLayer *layer3 = new Qt3DRender::QLayer;

            view.layers()->add(QStringLiteral("layer1"), layer1);
            view.layers()->add(QStringLiteral("layer2"), layer2);
            view.layers()->add(QStringLiteral("layer3"), layer3);

            // THEN
            QCOMPARE(view.frameGraph()->views().size(), size_t(2));
            QCOMPARE(view.layers()->size(), 3);
            for (Kuesa::View *view : view.frameGraph()->views()) {
                QCOMPARE(view->layers().size(), size_t(0));
            }
            QCOMPARE(view.frameGraph()->layers().size(), size_t(0));
            QCOMPARE(layerNames1ChangedSpy.count(), 0);
            QCOMPARE(layerNames2ChangedSpy.count(), 0);

            // WHEN -> Add layer
            QStringList names1 = { QStringLiteral("layer2") };
            QStringList names2 = { QStringLiteral("layer2"), QStringLiteral("layer3") };
            viewConfiguration1.setLayerNames(names1);
            viewConfiguration2.setLayerNames(names2);

            // THEN
            QCOMPARE(layerNames1ChangedSpy.count(), 1);
            QCOMPARE(layerNames2ChangedSpy.count(), 1);
            QCOMPARE(viewConfiguration1.layerNames(), names1);
            QCOMPARE(viewConfiguration2.layerNames(), names2);

            // View0 should contain layer2
            {
                auto view0 = view.frameGraph()->views()[0];
                auto it2 = std::find(view0->layers().begin(), view0->layers().end(), layer2);
                QVERIFY(it2 != view0->layers().end());
            }

            // View1 should contain layer2 and layer3
            {
                auto view1 = view.frameGraph()->views()[1];
                auto it2 = std::find(view1->layers().begin(), view1->layers().end(), layer2);
                auto it3 = std::find(view1->layers().begin(), view1->layers().end(), layer3);
                QVERIFY(it2 != view1->layers().end());
                QVERIFY(it3 != view1->layers().end());
            }

            // WHEN -> Add and preserve existing
            names1 = QStringList{ QStringLiteral("layer2"), QStringLiteral("layer3") };
            viewConfiguration1.setLayerNames(names1);

            // THEN
            QCOMPARE(layerNames1ChangedSpy.count(), 2);
            QCOMPARE(viewConfiguration1.layerNames(), names1);
            // View0 should now contain layer2 and layer3
            {
                auto view0 = view.frameGraph()->views()[0];
                auto it2 = std::find(view0->layers().begin(), view0->layers().end(), layer2);
                auto it3 = std::find(view0->layers().begin(), view0->layers().end(), layer3);
                QVERIFY(it2 != view0->layers().end());
                QVERIFY(it3 != view0->layers().end());
            }

            // WHEN -> Remove and Add and Preserve
            names1 = QStringList{ QStringLiteral("layer1"), QStringLiteral("layer3") };
            viewConfiguration1.setLayerNames(names1);

            // THEN
            QCOMPARE(layerNames1ChangedSpy.count(), 3);
            QCOMPARE(viewConfiguration1.layerNames(), names1);
            // View0 should now contain layer1 and layer3
            {
                auto view0 = view.frameGraph()->views()[0];
                auto it1 = std::find(view0->layers().begin(), view0->layers().end(), layer1);
                auto it3 = std::find(view0->layers().begin(), view0->layers().end(), layer3);
                QVERIFY(it1 != view0->layers().end());
                QVERIFY(it3 != view0->layers().end());
            }
        }

        // THEN
        // ViewConfigurations are destroyed and framegraph shouldn't contain views
        QCOMPARE(view.frameGraph()->views().size(), size_t(0));
    }
};

QTEST_MAIN(tst_View3DScene)
#include "tst_view3dscene.moc"
