/*
    tst_sceneparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <Kuesa/private/nodeparser_p.h>
#include <Kuesa/private/sceneparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

namespace {
const QLatin1String KEY_NODES = QLatin1String("nodes");
const QLatin1String KEY_SCENES = QLatin1String("scenes");

} // namespace

class tst_SceneParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("sceneCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "scenes_valid.gltf")
                               << true
                               << 1;

        QTest::newRow("Invalid-Missing-Nodes") << QStringLiteral(ASSETS "scenes_missing_nodes.gltf")
                                               << false
                                               << 0;

        QTest::newRow("Invalid-Empty-Nodes") << QStringLiteral(ASSETS "scenes_empty_nodes.gltf")
                                             << false
                                             << 0;

        QTest::newRow("Invalid-Duplicate-Nodes") << QStringLiteral(ASSETS "scenes_duplicate_nodes.gltf")
                                                 << false
                                                 << 0;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, sceneCount);

        // GIVEN
        GLTF2Context context;
        NodeParser nodeParser;

        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonObject rootObj = json.object();
        // THEN
        QVERIFY(!json.isNull() && json.isObject() && !rootObj.isEmpty());

        // WHEN
        const bool initSuccess = nodeParser.parse(rootObj.value(KEY_NODES).toArray(), &context);
        // THEN
        QVERIFY(initSuccess);

        // WHEN
        SceneParser parser;
        bool success = parser.parse(rootObj.value(KEY_SCENES).toArray(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(sceneCount, context.scenesCount());
    }
};

QTEST_APPLESS_MAIN(tst_SceneParser)

#include "tst_sceneparser.moc"
