/*
    tst_sceneconfiguration.cpp

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
#include <KuesaUtils/sceneconfiguration.h>
#include <Kuesa/animationplayer.h>
#include <Kuesa/transformtracker.h>
#include <Kuesa/placeholdertracker.h>

class tst_SceneConfiguration : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;

        // THEN
        QCOMPARE(sceneConfiguration.source(), QUrl());
        QCOMPARE(sceneConfiguration.cameraName(), QString());
        QVERIFY(sceneConfiguration.animationPlayers().empty());
        QVERIFY(sceneConfiguration.transformTrackers().empty());
        QVERIFY(sceneConfiguration.placeholderTrackers().empty());
    }

    void checkSetSource()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy sourceChangedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::sourceChanged);

        // THEN
        QVERIFY(sourceChangedSpy.isValid());

        // WHEN
        const QUrl newSource = QUrl("some/bad/path");
        sceneConfiguration.setSource(newSource);

        // THEN
        QCOMPARE(sceneConfiguration.source(), newSource);
        QCOMPARE(sourceChangedSpy.count(), 1);
    }

    void checkSetCameraName()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy cameraNameChangedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::cameraNameChanged);

        // THEN
        QVERIFY(cameraNameChangedSpy.isValid());

        // WHEN
        const QString newCameraName = QStringLiteral("Camera_Orientation");
        sceneConfiguration.setCameraName(newCameraName);

        // THEN
        QCOMPARE(sceneConfiguration.cameraName(), newCameraName);
        QCOMPARE(cameraNameChangedSpy.count(), 1);
    }

    void checkTransformTrackers()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy trackerAddedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::transformTrackerAdded);
        QSignalSpy trackerRemovedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::transformTrackerRemoved);

        // THEN
        QVERIFY(trackerAddedSpy.isValid());
        QVERIFY(trackerRemovedSpy.isValid());

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(2));
            QCOMPARE(trackerAddedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(0));
        QCOMPARE(trackerRemovedSpy.count(), 2);

        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(1));
            QCOMPARE(trackerAddedSpy.count(), 1);

            //WHEN
            sceneConfiguration.removeTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(1));
            QCOMPARE(trackerRemovedSpy.count(), 0);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(0));
        QCOMPARE(trackerRemovedSpy.count(), 1);

        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(2));
            QCOMPARE(trackerAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.clearTransformTrackers();

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(0));
            QCOMPARE(trackerRemovedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash
        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(2));
            QCOMPARE(trackerAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.removeTransformTracker(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(1));
            QCOMPARE(sceneConfiguration.transformTrackers().front(), &t2);
            QCOMPARE(trackerRemovedSpy.count(), 1);
        }

        // THEN -> Shouldn't crash
        QCOMPARE(sceneConfiguration.transformTrackers().size(), size_t(0));
        QCOMPARE(trackerRemovedSpy.count(), 2);

        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();
    }

    void checkAnimationPlayers()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy animationPlayerAddedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::animationPlayerAdded);
        QSignalSpy animationPlayerRemovedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::animationPlayerRemoved);

        // THEN
        QVERIFY(animationPlayerAddedSpy.isValid());
        QVERIFY(animationPlayerRemovedSpy.isValid());

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(2));
            QCOMPARE(animationPlayerAddedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash and should have removed animation
        QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(0));
        QCOMPARE(animationPlayerRemovedSpy.count(), 2);

        animationPlayerAddedSpy.clear();
        animationPlayerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(1));
            QCOMPARE(animationPlayerAddedSpy.count(), 1);

            // WHEN
            sceneConfiguration.removeAnimationPlayer(&t2);
            QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(1));
            QCOMPARE(animationPlayerRemovedSpy.count(), 0);
        }

        // THEN -> Shouldn't crash and should have removed animation
        QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(0));
        QCOMPARE(animationPlayerRemovedSpy.count(), 1);

        animationPlayerAddedSpy.clear();
        animationPlayerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(2));
            QCOMPARE(animationPlayerAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.clearAnimationPlayers();

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(0));
            QCOMPARE(animationPlayerRemovedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash
        animationPlayerAddedSpy.clear();
        animationPlayerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(2));
            QCOMPARE(animationPlayerAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.removeAnimationPlayer(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(1));
            QCOMPARE(sceneConfiguration.animationPlayers().front(), &t2);
            QCOMPARE(animationPlayerRemovedSpy.count(), 1);
        }

        // THEN -> Shouldn't crash
        QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(0));
        QCOMPARE(animationPlayerRemovedSpy.count(), 2);

        animationPlayerAddedSpy.clear();
        animationPlayerRemovedSpy.clear();
    }

    void checkPlaceholderTrackers()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy placeholderTrackerAddedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::placeholderTrackerAdded);
        QSignalSpy placeholderTrackerRemovedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::placeholderTrackerRemoved);

        // THEN
        QVERIFY(placeholderTrackerAddedSpy.isValid());
        QVERIFY(placeholderTrackerRemovedSpy.isValid());

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            sceneConfiguration.addPlaceholderTracker(&p1);
            sceneConfiguration.addPlaceholderTracker(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(2));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash and should have removed placeholderTrackers
        QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(0));
        QCOMPARE(placeholderTrackerRemovedSpy.count(), 2);

        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            sceneConfiguration.addPlaceholderTracker(&p1);
            sceneConfiguration.addPlaceholderTracker(&p1);

            // THEN
            QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(1));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 1);

            // WHEN
            sceneConfiguration.removePlaceholderTracker(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(1));
            QCOMPARE(placeholderTrackerRemovedSpy.count(), 0);
        }

        // THEN -> Shouldn't crash and should have removed placeholderTrackers
        QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(0));
        QCOMPARE(placeholderTrackerRemovedSpy.count(), 1);

        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            sceneConfiguration.addPlaceholderTracker(&p1);
            sceneConfiguration.addPlaceholderTracker(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(2));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.clearPlaceholderTrackers();

            // THEN
            QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(0));
            QCOMPARE(placeholderTrackerRemovedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash
        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            sceneConfiguration.addPlaceholderTracker(&p1);
            sceneConfiguration.addPlaceholderTracker(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(2));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.removePlaceholderTracker(&p1);

            // THEN
            QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(1));
            QCOMPARE(sceneConfiguration.placeholderTrackers().front(), &p2);
            QCOMPARE(placeholderTrackerRemovedSpy.count(), 1);
        }

        // THEN -> Shouldn't crash
        QCOMPARE(sceneConfiguration.placeholderTrackers().size(), size_t(0));
        QCOMPARE(placeholderTrackerRemovedSpy.count(), 2);

        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();
    }
};

QTEST_MAIN(tst_SceneConfiguration)
#include "tst_sceneconfiguration.moc"
