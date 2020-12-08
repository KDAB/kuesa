/*
    tst_lightparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <QAbstractLight>
#include <Kuesa/private/lightparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_LightParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<QString>("name");
        QTest::addColumn<int>("type");
        QTest::addColumn<QColor>("color");
        QTest::addColumn<float>("intensity");
        QTest::addColumn<QVariant>("range");
        QTest::addColumn<float>("innerConeAngle");
        QTest::addColumn<float>("outerConeAngle");

        QTest::newRow("Valid Directional Light") << QStringLiteral(ASSETS "KHR_lights_punctual/directional_valid.gltf")
                                                 << true
                                                 << "Directional"
                                                 << static_cast<int>(Qt3DRender::QAbstractLight::DirectionalLight)
                                                 << QColor::fromRgbF(1.0f, .9f, .7f)
                                                 << 3.0f
                                                 << QVariant()
                                                 << 0.0f
                                                 << (float)M_PI_4;
        ;

        QTest::newRow("Valid Point Light") << QStringLiteral(ASSETS "KHR_lights_punctual/point_valid.gltf")
                                           << true
                                           << "Point"
                                           << static_cast<int>(Qt3DRender::QAbstractLight::PointLight)
                                           << QColor::fromRgbF(1.0f, 0.0f, 0.0f)
                                           << 20.0f
                                           << QVariant(23.0f)
                                           << 0.0f
                                           << (float)M_PI_4;
        ;

        QTest::newRow("Valid Spot Light") << QStringLiteral(ASSETS "KHR_lights_punctual/spotlight_valid.gltf")
                                          << true
                                          << "Spot"
                                          << static_cast<int>(Qt3DRender::QAbstractLight::SpotLight)
                                          << QColor::fromRgbF(0.3f, 0.7f, 1.0f)
                                          << 40.0f
                                          << QVariant(10.0f)
                                          << 0.785398163397448f
                                          << 1.57079632679f;
        QTest::newRow("Default Values") << QStringLiteral(ASSETS "KHR_lights_punctual/default_test.gltf")
                                        << true
                                        << QString()
                                        << static_cast<int>(Qt3DRender::QAbstractLight::SpotLight)
                                        << QColor::fromRgbF(1.0f, 1.0f, 1.0f)
                                        << 1.0f
                                        << QVariant()
                                        << 0.0f
                                        << (float)M_PI_4;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(QString, name);
        QFETCH(int, type);
        QFETCH(QColor, color);
        QFETCH(float, intensity);
        QFETCH(QVariant, range);
        QFETCH(float, innerConeAngle);
        QFETCH(float, outerConeAngle);

        // GIVEN
        GLTF2Context context;
        LightParser parser;
        QFile file(filePath);
        qDebug() << "File path " << file.fileName();
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        // THEN
        QVERIFY(!json.isNull());
        QVERIFY(json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.lightCount(), 1);
        QCOMPARE(context.light(0).name, name);
        QCOMPARE(context.light(0).type, type);
        QCOMPARE(context.light(0).color, color);
        QCOMPARE(context.light(0).intensity, intensity);
        QCOMPARE(context.light(0).range, range);
        QCOMPARE(context.light(0).innerConeAngleRadians, innerConeAngle);
        QCOMPARE(context.light(0).outerConeAngleRadians, outerConeAngle);
    }
};

QTEST_APPLESS_MAIN(tst_LightParser)

#include "tst_lightparser.moc"
