/*
    tst_imageparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Qt3DRender/QTextureImage>
#include <Kuesa/private/imageparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_ImageParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<size_t>("imageCount");
        QTest::addColumn<QString>("imageFile");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "imageparser_valid.gltf")
                               << true
                               << size_t(1)
                               << ASSETS "aBasePath/anImage.png";

        QTest::newRow("Empty") << QStringLiteral(ASSETS "imageparser_empty.gltf")
                               << false
                               << size_t(0)
                               << QString();

        QTest::newRow("PartiallyIncomplete") << QStringLiteral(ASSETS "imageparser_incomplete.gltf")
                                             << false
                                             << size_t(0)
                                             << QString();

        QTest::newRow("UriAndBufferView") << QStringLiteral(ASSETS "imageparser_uri_and_bufferview.gltf")
                                          << false
                                          << size_t(0)
                                          << QString();

        QTest::newRow("UriAndBufferView") << QStringLiteral(ASSETS "imageparser_bufferview.gltf")
                                          << false
                                          << size_t(0)
                                          << QString();
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(size_t, imageCount);
        QFETCH(QString, imageFile);

        // GIVEN
        GLTF2Context context;
        ImageParser parser(QDir(QString(ASSETS "aBasePath")));
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        const bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.imagesCount(), imageCount);
        if (success)
            QCOMPARE(QUrl::fromLocalFile(imageFile), context.image(0).url.toString());
    }

    void checkParseEmbedded_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("dataSize");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "imageparser_valid_datauri.gltf")
                               << true
                               << 5;
    }

    void checkParseEmbedded()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, dataSize);

        // GIVEN
        GLTF2Context context;
        ImageParser parser({});
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        const bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        if (success)
            QCOMPARE(context.image(0).data.size(), dataSize);
    }

    void checkAssetsInResources()
    {
        // GIVEN
        GLTF2Context context;
        ImageParser parser(QDir(QString(":")));
        QFile file(QStringLiteral(ASSETS "imageparser_valid.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        const bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.imagesCount(), size_t(1));
        Image image = context.image(0);
        QCOMPARE(image.url.scheme(), QStringLiteral("qrc"));
        QCOMPARE(image.url, QUrl("qrc:/anImage.png"));
    }

    void checkAssetsMixedUris()
    {
        // GIVEN
        GLTF2Context context;
        ImageParser parser(QDir(QString(ASSETS "aBasePath")));
        QFile file(QStringLiteral(ASSETS "imageparser_mixed.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        const bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.imagesCount(), size_t(7));
        {
            Image image = context.image(0);
            QCOMPARE(image.url.scheme(), QStringLiteral("file"));
            QCOMPARE(image.url, QUrl::fromLocalFile(ASSETS "aBasePath/anImage.png"));
        }
        {
            Image image = context.image(1);
            QCOMPARE(image.url.scheme(), QStringLiteral("file"));
            QCOMPARE(image.url, QUrl::fromLocalFile(ASSETS "aBasePath/anImage.png"));
        }
        {
            Image image = context.image(2);
            QCOMPARE(image.url.scheme(), QStringLiteral("file"));
#ifdef Q_OS_WINDOWS
            QCOMPARE(image.url, QUrl("file:///C:/anImage.png"));
#else
            QCOMPARE(image.url, QUrl("file:///anImage.png"));
#endif
        }
        {
            Image image = context.image(3);
            QCOMPARE(image.url.scheme(), QStringLiteral("file"));
#ifdef Q_OS_WINDOWS
            QCOMPARE(image.url, QUrl("file:///C:/anImage.png"));
#else
            QCOMPARE(image.url, QUrl("file:///anImage.png"));
#endif
        }
        {
            Image image = context.image(4);
            QCOMPARE(image.url.scheme(), QStringLiteral("qrc"));
            QCOMPARE(image.url, QUrl("qrc:/anImage.png"));
        }
        {
            Image image = context.image(5);
            QCOMPARE(image.url.scheme(), QStringLiteral("qrc"));
            QCOMPARE(image.url, QUrl("qrc:/anImage.png"));
        }
        {
            Image image = context.image(6);
            QCOMPARE(image.url.scheme(), QStringLiteral("qrc"));
            QCOMPARE(image.url, QUrl("qrc:///anImage.png"));
        }
    }
};

QTEST_APPLESS_MAIN(tst_ImageParser)

#include "tst_imageparser.moc"
