/*
    tst_assetitem.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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
#include <QQmlComponent>
#include <QQmlEngine>
#include <Kuesa/gltf2importer.h>

class tst_AssetItem : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkAsset()
    {
        // WITH
        QQmlEngine engine;
        QQmlComponent cmp(&engine);

        cmp.setData(QByteArrayLiteral(R"*(
                          import Qt3D.Core 2.10
                          import Kuesa 1.0

                          SceneEntity {
                              id: scene

                              GLTF2Importer {
                                  sceneEntity: scene
                                  objectName: "importer"
                              }

                              Asset {
                                  id: cameraAsset
                                  objectName: "cameraAsset"
                                  collection: scene.cameras
                              }
                          }
                      )*"),
                    QUrl());

        auto errors = cmp.errors();
        qDebug() << errors;

        // THEN
        QVERIFY(errors.isEmpty());

        // WHEN
        QObject *rootObj = cmp.create();

        // THEN
        QVERIFY(rootObj);

        // WITH
        Kuesa::GLTF2Importer *importer = rootObj->findChild<Kuesa::GLTF2Importer *>("importer");

        // THEN
        QVERIFY(importer);

        // WITH
        QObject *assetItem = rootObj->findChild<QObject *>(QLatin1String("cameraAsset"));

        // THEN
        QVERIFY(assetItem != nullptr);

        // WHEN
        assetItem->setProperty("name", QLatin1String("Camera"));

        // WHEN
        importer->setSource(QUrl("file:///" ASSETS "simple_cube.gltf"));
        QCoreApplication::processEvents();

        // THEN
        QObject *asset = assetItem->property("node").value<QObject *>();
        QVERIFY(asset != nullptr);
        QCOMPARE(asset->objectName(), QLatin1String("Camera"));

        // WHEN
        assetItem->setProperty("name", QLatin1String("UndefinedCameraName"));

        // THEN
        asset = assetItem->property("node").value<QObject *>();
        QVERIFY(asset == nullptr);
    }
};

QTEST_MAIN(tst_AssetItem)

#include "tst_assetitem.moc"
