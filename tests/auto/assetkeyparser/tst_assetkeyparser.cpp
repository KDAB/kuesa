/*
    tst_assetkeyparser.cpp

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
#include <Kuesa/private/assetkeyparser_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

struct FakeAsset
{
    QString key;
};

class tst_AssetKeyParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<QString>("expectedKey");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "valid_asset_key.gltf")
                               << QStringLiteral("valid_key");

        QTest::newRow("Empty") << QStringLiteral(ASSETS "empty_asset_key.gltf")
                               << QStringLiteral("");

        QTest::newRow("NoExtension") << QStringLiteral(ASSETS "asset_key_no_extension.gltf")
                                     << QStringLiteral("");
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(QString, expectedKey);

        // GIVEN
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        QJsonParseError e;
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &e);
        const QJsonObject jsonObj = json.object();

        // THEN
        QVERIFY(!json.isNull() && json.isObject());

        // WHEN
        FakeAsset asset;
        Kuesa::GLTF2Import::AssetKeyParser::parse(asset, jsonObj);

        // THEN
        QCOMPARE(asset.key, expectedKey);
    }
};

QTEST_GUILESS_MAIN(tst_AssetKeyParser)

#include "tst_assetkeyparser.moc"
