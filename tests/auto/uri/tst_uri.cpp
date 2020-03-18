/*
    tst_uri.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

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
#include <QFile>
#include <QString>
#include <Kuesa/private/gltf2uri_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_Uri : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkUriDetection()
    {
        QCOMPARE(Uri::kind("foo/bar"), Uri::Kind::Path);
        QCOMPARE(Uri::kind("dat"), Uri::Kind::Path);
        QCOMPARE(Uri::kind("/a/file.jpg"), Uri::Kind::Path);
        QCOMPARE(Uri::kind("c:\\test.bin"), Uri::Kind::Path);
        QCOMPARE(Uri::kind("qrc:foo.jpg"), Uri::Kind::Path);
        QCOMPARE(Uri::kind("qrc:/foo.jpg"), Uri::Kind::Path);
        QCOMPARE(Uri::kind("qrc:///foo.jpg"), Uri::Kind::Path);
        QCOMPARE(Uri::kind(":/foo.jpg"), Uri::Kind::Path);

        QCOMPARE(Uri::kind("data:"), Uri::Kind::Data);
        QCOMPARE(Uri::kind("Data:"), Uri::Kind::Data);
        QCOMPARE(Uri::kind("Data:abcdef"), Uri::Kind::Data);
    }

    void checkUriParsing_data()
    {
        QTest::addColumn<QString>("uri");
        QTest::addColumn<int>("size");
        QTest::addColumn<QByteArray>("bufferData");
        QTest::addColumn<bool>("succeeded");

        QTest::newRow("Valid Data URI") << QStringLiteral("data:application/octet-stream;base64,S3Vlc2E=")
                                        << 5
                                        << QByteArray("Kuesa")
                                        << true;
        QTest::newRow("Valid File URI") << QStringLiteral("bufferparser.bin")
                                        << 1
                                        << QByteArray("1")
                                        << true;
        QTest::newRow("Non Existent File") << QStringLiteral("bufferparser_nothere.bin")
                                           << 0
                                           << QByteArray()
                                           << false;
        QTest::newRow("QRC File") << QStringLiteral(":/uri_qrc_test.bin")
                                  << 5
                                  << QByteArray("Kuesa")
                                  << true;
        QTest::newRow("QRC short URL") << QStringLiteral("qrc:/uri_qrc_test.bin")
                                       << 5
                                       << QByteArray("Kuesa")
                                       << true;
        QTest::newRow("QRC full URL") << QStringLiteral("qrc:///uri_qrc_test.bin")
                                      << 5
                                      << QByteArray("Kuesa")
                                      << true;
    }

    void checkUriParsing()
    {
        // GIVEN
        QFETCH(QString, uri);
        QFETCH(int, size);
        QFETCH(QByteArray, bufferData);
        QFETCH(bool, succeeded);

        // WHEN
        GLTF2Context context;
        bool success;
        auto data = Uri::fetchData(uri, QDir(ASSETS), success);

        // THEN
        QCOMPARE(success, succeeded);
        if (success) {
            QCOMPARE(size, data.size());
            QCOMPARE(bufferData, data);
        }
    }
};

QTEST_APPLESS_MAIN(tst_Uri)

#include "tst_uri.moc"
