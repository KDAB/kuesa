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
#include <KuesaUtils/viewconfiguration.h>

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
        QVERIFY(sceneConfiguration.animationPlayers().empty());
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

    void checkViewConfigurations()
    {
        // GIVEN
        KuesaUtils::SceneConfiguration sceneConfiguration;
        QSignalSpy viewConfigurationAddedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::viewConfigurationAdded);
        QSignalSpy viewConfigurationRemovedSpy(&sceneConfiguration, &KuesaUtils::SceneConfiguration::viewConfigurationRemoved);

        // THEN
        QVERIFY(viewConfigurationAddedSpy.isValid());
        QVERIFY(viewConfigurationRemovedSpy.isValid());

        {
            // WHEN
            KuesaUtils::ViewConfiguration t1;
            KuesaUtils::ViewConfiguration t2;

            sceneConfiguration.addViewConfiguration(&t1);
            sceneConfiguration.addViewConfiguration(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(2));
            QCOMPARE(viewConfigurationAddedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash and should have removed the viewConfigurations
        QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(0));
        QCOMPARE(viewConfigurationRemovedSpy.count(), 2);

        viewConfigurationAddedSpy.clear();
        viewConfigurationRemovedSpy.clear();

        {
            // WHEN
            KuesaUtils::ViewConfiguration t1;
            KuesaUtils::ViewConfiguration t2;

            sceneConfiguration.addViewConfiguration(&t1);
            sceneConfiguration.addViewConfiguration(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(1));
            QCOMPARE(viewConfigurationAddedSpy.count(), 1);

            // WHEN
            sceneConfiguration.removeViewConfiguration(&t2);
            QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(1));
            QCOMPARE(viewConfigurationRemovedSpy.count(), 0);
        }

        // THEN -> Shouldn't crash and should have removed the viewConfigurations
        QCOMPARE(sceneConfiguration.animationPlayers().size(), size_t(0));
        QCOMPARE(viewConfigurationRemovedSpy.count(), 1);

        viewConfigurationAddedSpy.clear();
        viewConfigurationRemovedSpy.clear();

        {
            // WHEN
            KuesaUtils::ViewConfiguration t1;
            KuesaUtils::ViewConfiguration t2;

            sceneConfiguration.addViewConfiguration(&t1);
            sceneConfiguration.addViewConfiguration(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(2));
            QCOMPARE(viewConfigurationAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.clearViewConfigurations();

            // THEN
            QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(0));
            QCOMPARE(viewConfigurationRemovedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash
        viewConfigurationAddedSpy.clear();
        viewConfigurationRemovedSpy.clear();

        {
            // WHEN
            KuesaUtils::ViewConfiguration t1;
            KuesaUtils::ViewConfiguration t2;

            sceneConfiguration.addViewConfiguration(&t1);
            sceneConfiguration.addViewConfiguration(&t2);

            // THEN
            QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(2));
            QCOMPARE(viewConfigurationAddedSpy.count(), 2);

            // WHEN
            sceneConfiguration.removeViewConfiguration(&t1);

            // THEN
            QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(1));
            QCOMPARE(sceneConfiguration.viewConfigurations().front(), &t2);
            QCOMPARE(viewConfigurationRemovedSpy.count(), 1);
        }

        // THEN -> Shouldn't crash
        QCOMPARE(sceneConfiguration.viewConfigurations().size(), size_t(0));
        QCOMPARE(viewConfigurationRemovedSpy.count(), 2);

        viewConfigurationAddedSpy.clear();
        viewConfigurationRemovedSpy.clear();
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
};

QTEST_MAIN(tst_SceneConfiguration)
#include "tst_sceneconfiguration.moc"
