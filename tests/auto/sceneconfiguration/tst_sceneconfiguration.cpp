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
#include <Kuesa/placeholder.h>

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
        QVERIFY(sceneConfiguration.placeholders().empty());
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

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), 2);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(sceneConfiguration.transformTrackers().size(), 0);

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), 1);

            //WHEN
            sceneConfiguration.removeTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), 1);
        }

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), 2);

            // WHEN
            sceneConfiguration.clearTransformTrackers();

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), 0);
        }

        // THEN -> Shouldn't crash

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            sceneConfiguration.addTransformTracker(&t1);
            sceneConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), 2);

            // WHEN
            sceneConfiguration.removeTransformTracker(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.transformTrackers().size(), 1);
            QCOMPARE(sceneConfiguration.transformTrackers().front(), &t2);
        }

        // THEN -> Shouldn't crash
    }

    void checkAnimationPlayers()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), 2);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(sceneConfiguration.animationPlayers().size(), 0);

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), 1);

            // WHEN
            sceneConfiguration.removeAnimationPlayer(&t2);
            QCOMPARE(sceneConfiguration.animationPlayers().size(), 1);
        }

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), 2);

            // WHEN
            sceneConfiguration.clearAnimationPlayers();

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), 0);
        }

        // THEN -> Shouldn't crash

        {
            // WHEN
            Kuesa::AnimationPlayer t1;
            Kuesa::AnimationPlayer t2;

            sceneConfiguration.addAnimationPlayer(&t1);
            sceneConfiguration.addAnimationPlayer(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), 2);

            // WHEN
            sceneConfiguration.removeAnimationPlayer(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.animationPlayers().size(), 1);
            QCOMPARE(sceneConfiguration.animationPlayers().front(), &t2);
        }

        // THEN -> Shouldn't crash
    }

    void checkPlaceholders()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy placeholderAddedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::placeholderAdded);
        QSignalSpy placeholderRemovedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::placeholderRemoved);

        // THEN
        QVERIFY(placeholderAddedSpy.isValid());
        QVERIFY(placeholderRemovedSpy.isValid());

        {
            // WHEN
            Kuesa::Placeholder p1;
            Kuesa::Placeholder p2;

            sceneConfiguration.addPlaceholder(&p1);
            sceneConfiguration.addPlaceholder(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholders().size(), 2);
            QCOMPARE(placeholderAddedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash and should have removed placeholders
        QCOMPARE(sceneConfiguration.placeholders().size(), 0);
        QCOMPARE(placeholderRemovedSpy.count(), 2);

        placeholderAddedSpy.clear();
        placeholderRemovedSpy.clear();

        {
            // WHEN
            Kuesa::Placeholder p1;
            Kuesa::Placeholder p2;

            sceneConfiguration.addPlaceholder(&p1);
            sceneConfiguration.addPlaceholder(&p1);

            // THEN
            QCOMPARE(sceneConfiguration.placeholders().size(), 1);
            QCOMPARE(placeholderAddedSpy.count(), 1);

            // WHEN
            sceneConfiguration.removePlaceholder(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholders().size(), 1);
            QCOMPARE(placeholderRemovedSpy.count(), 0);
        }

        // THEN -> Shouldn't crash and should have removed placeholders
        QCOMPARE(sceneConfiguration.placeholders().size(), 0);
        QCOMPARE(placeholderRemovedSpy.count(), 1);

        placeholderAddedSpy.clear();
        placeholderRemovedSpy.clear();

        {
            // WHEN
            Kuesa::Placeholder p1;
            Kuesa::Placeholder p2;

            sceneConfiguration.addPlaceholder(&p1);
            sceneConfiguration.addPlaceholder(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholders().size(), 2);
            QCOMPARE(placeholderAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.clearPlaceholders();

            // THEN
            QCOMPARE(sceneConfiguration.placeholders().size(), 0);
            QCOMPARE(placeholderRemovedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash
        placeholderAddedSpy.clear();
        placeholderRemovedSpy.clear();

        {
            // WHEN
            Kuesa::Placeholder p1;
            Kuesa::Placeholder p2;

            sceneConfiguration.addPlaceholder(&p1);
            sceneConfiguration.addPlaceholder(&p2);

            // THEN
            QCOMPARE(sceneConfiguration.placeholders().size(), 2);
            QCOMPARE(placeholderAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.removePlaceholder(&p1);

            // THEN
            QCOMPARE(sceneConfiguration.placeholders().size(), 1);
            QCOMPARE(sceneConfiguration.placeholders().front(), &p2);
            QCOMPARE(placeholderRemovedSpy.count(), 1);
        }

        // THEN -> Shouldn't crash
        QCOMPARE(sceneConfiguration.placeholders().size(), 0);
        QCOMPARE(placeholderRemovedSpy.count(), 2);

        placeholderAddedSpy.clear();
        placeholderRemovedSpy.clear();
    }
};

QTEST_MAIN(tst_SceneConfiguration)
#include "tst_sceneconfiguration.moc"
