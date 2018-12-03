/*
    tst_bufferparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Jose Casafranca <juan.casafranca@kdab.com>

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
#include <Kuesa/private/bufferparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_BufferParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("bufferCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "bufferparser_valid.gltf")
                               << true
                               << 1;

        QTest::newRow("Empty")<< QStringLiteral(ASSETS "bufferparser_empty.gltf")
                              << false
                              << 0;

        QTest::newRow("Non Existent")<< QStringLiteral(ASSETS "bufferparser_nothere.gltf")
                              << false
                              << 0;

        QTest::newRow("PartiallyIncomplete") << QStringLiteral(ASSETS "bufferparser_incomplete.gltf")
                                             << false
                                             << 1;

        QTest::newRow("EmptyUri") << QStringLiteral(ASSETS "bufferparser_empty_uri.gltf")
                                             << false
                                             << 1;

        QTest::newRow("WrongSize") << QStringLiteral(ASSETS "bufferparser_wrong_size.gltf")
                                             << false
                                             << 0;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, bufferCount);


        // GIVEN
        GLTF2Context context;
        BufferParser parser(QDir(ASSETS));
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        // THEN
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.bufferCount(), bufferCount);
    }

};

QTEST_APPLESS_MAIN(tst_BufferParser)

#include "tst_bufferparser.moc"
