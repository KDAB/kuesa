/*
    tst_shadowparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

class tst_ShadowParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<bool>("castsShadows");
        QTest::addColumn<bool>("softShadows");
        QTest::addColumn<QSize>("textureSize");
        QTest::addColumn<float>("shadowMapBias");
        QTest::addColumn<float>("nearPlane");

        QTest::newRow("No Extension") << QStringLiteral(ASSETS "shadows_default.gltf")
                                      << true
                                      << false
                                      << false
                                      << QSize(512, 512)
                                      << .005f
                                      << 0.0f;
        QTest::newRow("Shadows Extension") << QStringLiteral(ASSETS "/shadows_valid.gltf")
                                           << true
                                           << true
                                           << true
                                           << QSize(333, 444)
                                           << .1234f
                                           << .5678f;
    }
    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(bool, castsShadows);
        QFETCH(bool, softShadows);
        QFETCH(QSize, textureSize);
        QFETCH(float, shadowMapBias);
        QFETCH(float, nearPlane);

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
        QCOMPARE(context.light(0).castsShadows, castsShadows);
        QCOMPARE(context.light(0).softShadows, softShadows);
        QCOMPARE(context.light(0).shadowMapTextureSize, textureSize);
        QCOMPARE(context.light(0).shadowMapBias, shadowMapBias);
        QCOMPARE(context.light(0).nearPlane, nearPlane);
    }
};

QTEST_APPLESS_MAIN(tst_ShadowParser)

#include "tst_shadowparser.moc"
