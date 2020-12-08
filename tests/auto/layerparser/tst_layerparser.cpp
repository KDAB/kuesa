/*
    tst_layerparser.cpp

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

#include <QtTest/QTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/private/layerparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_LayerParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("layersCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "layerparser_valid.gltf")
                               << true
                               << 3;

        QTest::newRow("Empty") << QStringLiteral(ASSETS "layerparser_empty.gltf")
                               << false
                               << 0;

        QTest::newRow("InvalidContent") << QStringLiteral(ASSETS "layerparser_invalid.gltf")
                                        << false
                                        << 0;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, layersCount);

        // GIVEN
        GLTF2Context context;
        LayerParser parser;
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonObject gltfJson = json.object();
        QVERIFY(!json.isNull() && json.isObject());
        QVERIFY(gltfJson.contains(QLatin1String("layers")));

        // WHEN
        bool success = parser.parse(gltfJson.value(QLatin1String("layers")).toArray(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.layersCount(), layersCount);
    }
};

QTEST_GUILESS_MAIN(tst_LayerParser)

#include "tst_layerparser.moc"
