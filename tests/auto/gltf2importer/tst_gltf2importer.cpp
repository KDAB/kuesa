/*
    tst_gltf2importer.cpp

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

#include <qtkuesa-config.h>
#include <QtTest/QtTest>
#include <Kuesa/GLTF2Importer>
#include <QUrl>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_GLTF2Importer : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkStatus()
    {
        // GIVEN
        GLTF2Importer importer;

        // THEN
        QCOMPARE(importer.status(), GLTF2Importer::None);

        // WHEN
        importer.setSource(QUrl("file:///" ASSETS "Box.gltf"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(importer.status(), GLTF2Importer::Ready);

        // WHEN
        importer.setSource(QUrl());
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(importer.status(), GLTF2Importer::None);

        // WHEN
        importer.setSource(QUrl("file:///" ASSETS "simple_cube_with_images_invalid_scene_negative.gltf"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(importer.status(), GLTF2Importer::Error);
    }

    void checkScenes()
    {
        const QStringList expectedSceneNames = {
            QStringLiteral(""),
            QStringLiteral("Scene 1"),
            QStringLiteral("Scene 2")
        };

        {
            // GIVEN
            GLTF2Importer importer;

            // WHEN
            importer.setActiveSceneIndex(GLTF2Importer::DefaultScene);
            importer.setSource(QUrl("file:///" ASSETS "BoxMultipleScenes.gltf"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(importer.status(), GLTF2Importer::Ready);

            QCOMPARE(importer.availableScenes().size(), 3);
            QCOMPARE(importer.availableScenes(), expectedSceneNames);
            QCOMPARE(importer.activeSceneIndex(), 2);
        }
        {
            // GIVEN
            GLTF2Importer importer;

            // WHEN
            importer.setActiveSceneIndex(GLTF2Importer::EmptyScene);
            importer.setSource(QUrl("file:///" ASSETS "BoxMultipleScenes.gltf"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(importer.status(), GLTF2Importer::Ready);

            QCOMPARE(importer.availableScenes().size(), 3);
            QCOMPARE(importer.availableScenes(), expectedSceneNames);
            QCOMPARE(importer.activeSceneIndex(), -1);
        }
        {
            // GIVEN
            GLTF2Importer importer;

            // WHEN
            importer.setActiveSceneIndex(0);
            importer.setSource(QUrl("file:///" ASSETS "BoxMultipleScenes.gltf"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(importer.status(), GLTF2Importer::Ready);

            QCOMPARE(importer.availableScenes().size(), 3);
            QCOMPARE(importer.availableScenes(), expectedSceneNames);
            QCOMPARE(importer.activeSceneIndex(), 0);
        }
    }
};

QTEST_MAIN(tst_GLTF2Importer)

#include "tst_gltf2importer.moc"
