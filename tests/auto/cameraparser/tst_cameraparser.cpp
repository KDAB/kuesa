/*
    tst_cameraparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/private/cameraparser_p.h>
#include <Kuesa/private/gltf2context_p.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_CameraParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPerspectiveCamera()
    {
        // GIVEN
        GLTF2Context context;
        CameraParser parser;
        QFile file(QStringLiteral(ASSETS "camera_perspective.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.cameraCount(), 1);

        Camera camera = context.camera(0);
        QVERIFY(camera.lens);
        QVERIFY(!camera.name.isEmpty());
        QCOMPARE(camera.lens->projectionType(), Qt3DRender::QCameraLens::PerspectiveProjection);

        QCOMPARE(camera.lens->farPlane(), 100.f);
        QCOMPARE(camera.lens->nearPlane(), 1.f);
        QCOMPARE(camera.lens->aspectRatio(), 2.f);
        QCOMPARE(camera.lens->fieldOfView(), 180.f / (float) M_PI);
    }

    void checkOrthographicCamera()
    {
        // GIVEN
        GLTF2Context context;
        CameraParser parser;
        QFile file(QStringLiteral(ASSETS "camera_orthographic.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.cameraCount(), 1);

        Camera camera = context.camera(0);
        QVERIFY(camera.lens);
        QVERIFY(!camera.name.isEmpty());
        QCOMPARE(camera.lens->projectionType(), Qt3DRender::QCameraLens::CustomProjection);

        QCOMPARE(camera.lens->farPlane(), 100.f);
        QCOMPARE(camera.lens->nearPlane(), 1.f);

        const QMatrix4x4 pm = camera.lens->projectionMatrix();
        QCOMPARE(pm(0, 0), 1.f / 2.f);
        QCOMPARE(pm(1, 1), 1.f / 3.f);
        QCOMPARE(pm(2, 2), 2.f / -99.f);
        QCOMPARE(pm(2, 3), 101.f / -99.f);
    }

    void checkParseErrors_data()
    {
        QTest::addColumn<QString>("filePath");

        QTest::addRow("typeless") << QStringLiteral(ASSETS "camera_typeless.gltf");
        QTest::addRow("perspective_missing_yfov") << QStringLiteral(ASSETS "camera_perspective_missing_yfov.gltf");
        QTest::addRow("perspective_missing_znear") << QStringLiteral(ASSETS "camera_perspective_missing_znear.gltf");
        QTest::addRow("perspective_oor_znear") << QStringLiteral(ASSETS "camera_perspective_oor_znear.gltf");
        QTest::addRow("perspective_oor_zfar") << QStringLiteral(ASSETS "camera_perspective_oor_zfar.gltf");
        QTest::addRow("perspective_oor_yfov") << QStringLiteral(ASSETS "camera_perspective_oor_yfov.gltf");
        QTest::addRow("perspective_oor_ar") << QStringLiteral(ASSETS "camera_perspective_oor_ar.gltf");
        QTest::addRow("orthographic_missing_znear") << QStringLiteral(ASSETS "camera_orthographic_missing_znear.gltf");
        QTest::addRow("orthographic_missing_zfar") << QStringLiteral(ASSETS "camera_orthographic_missing_zfar.gltf");
        QTest::addRow("orthographic_missing_xmag") << QStringLiteral(ASSETS "camera_orthographic_missing_xmag.gltf");
        QTest::addRow("orthographic_missing_ymag") << QStringLiteral(ASSETS "camera_orthographic_missing_ymag.gltf");
        QTest::addRow("orthographic_oor_znear") << QStringLiteral(ASSETS "camera_orthographic_oor_znear.gltf");
        QTest::addRow("orthographic_oor_zfar") << QStringLiteral(ASSETS "camera_orthographic_oor_zfar.gltf");
        QTest::addRow("orthographic_oor_xmag") << QStringLiteral(ASSETS "camera_orthographic_oor_xmag.gltf");
        QTest::addRow("orthographic_oor_ymag") << QStringLiteral(ASSETS "camera_orthographic_oor_ymag.gltf");
    }

    void checkParseErrors()
    {
        QFETCH(QString, filePath);

        // GIVEN
        GLTF2Context context;
        CameraParser parser;
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(!success);
        QCOMPARE(context.cameraCount(), 0);
    }
};

QTEST_APPLESS_MAIN(tst_CameraParser)

#include "tst_cameraparser.moc"
