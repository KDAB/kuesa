/*
    tst_textureparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/QTexture>
#include <Kuesa/private/texturesamplerparser_p.h>
#include <Kuesa/private/imageparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

namespace {
const QLatin1String KEY_IMAGES = QLatin1String("images");
const QLatin1String KEY_TEXTURES = QLatin1String("textures");
const QLatin1String KEY_TEXTURE_SAMPLERS = QLatin1String("samplers");
} // namespace

class tst_TextureParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("texturesCount");
        QTest::addColumn<bool>("shouldHaveImage");

        QTest::newRow("Complete") << QStringLiteral(ASSETS "texture_complete.gltf")
                                  << true
                                  << 1
                                  << true;

        QTest::newRow("Empty") << QStringLiteral(ASSETS "texture_empty.gltf")
                               << false
                               << 0
                               << false;

        QTest::newRow("NoSource") << QStringLiteral(ASSETS "texture_no_source.gltf")
                                  << true
                                  << 1
                                  << false;

        QTest::newRow("NoSampler") << QStringLiteral(ASSETS "texture_no_sampler.gltf")
                                   << true
                                   << 1
                                   << true;

        QTest::newRow("InvalidSource") << QStringLiteral(ASSETS "texture_invalid_source.gltf")
                                       << false
                                       << 0
                                       << false;

        QTest::newRow("InvalidSampler") << QStringLiteral(ASSETS "texture_invalid_sampler.gltf")
                                        << false
                                        << 0
                                        << false;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, texturesCount);
        QFETCH(bool, shouldHaveImage);

        // GIVEN
        GLTF2Context context;
        TextureParser parser;
        QFile file(filePath);
        Image fakeImage;
        fakeImage.url = QUrl(QStringLiteral("file:///something_fake.png"));
        TextureSampler fakeSampler;
        fakeSampler.textureWrapMode = std::make_shared<Qt3DRender::QTextureWrapMode>();
        context.addImage(fakeImage);
        context.addTextureSampler(fakeSampler);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.texturesCount(), texturesCount);
        if (texturesCount > 0 && shouldHaveImage) {
            QVERIFY(!context.texture(0).texture);

            // WHEN
            context.getOrAllocateTexture(0);

            // THEN
            QVERIFY(context.texture(0).texture->textureImages()[0]);
        }
    }

    void checkTextureImageSharing()
    {
        // GIVEN
        GLTF2Context context;
        QFile file(QStringLiteral(ASSETS "simple_cube_with_textures_images_shared.gltf"));
        file.open(QIODevice::ReadOnly);

        // THEN
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull());

        ImageParser imgParser(QDir(QString(ASSETS)));
        bool success = imgParser.parse(json[KEY_IMAGES].toArray(), &context);

        // THEN
        QVERIFY(success);

        // WHEN
        TextureSamplerParser samplerParser;
        success = samplerParser.parse(json[KEY_TEXTURE_SAMPLERS].toArray(), &context);

        // THEN
        QVERIFY(success);

        // WHEN
        TextureParser parser;
        success = parser.parse(json[KEY_TEXTURES].toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.texturesCount(), 2);

        QVERIFY(!context.texture(0).texture);
        QVERIFY(!context.texture(1).texture);

        // WHEN
        context.getOrAllocateTexture(0);
        context.getOrAllocateTexture(1);

        // THEN
        QVERIFY(context.texture(0).texture);
        QVERIFY(context.texture(1).texture);
        QVERIFY(context.texture(0).texture != context.texture(1).texture);
        QCOMPARE(context.texture(0).texture->textureImages().size(),
                 context.texture(1).texture->textureImages().size());
        QCOMPARE(context.texture(0).texture->textureImages()[0],
                 context.texture(1).texture->textureImages()[0]);
    }

    void checkIncompatibleTextureImage()
    {
        // GIVEN
        Qt3DRender::QTexture2D tex2d;
        Qt3DRender::QTexture2DArray tex2dArray;
        Qt3DRender::QTextureImage img;
        Qt3DRender::QTextureImage img2;

        // WHEN
        img.setLayer(10);

        // THEN
        QVERIFY(!TextureParser::ensureImageIsCompatibleWithTexture(&img, &tex2d));

        // WHEN

        // THEN
        QVERIFY(TextureParser::ensureImageIsCompatibleWithTexture(&img, &tex2dArray));

        // WHEN
        img2.setLayer(10);
        tex2dArray.addTextureImage(&img);

        // THEN -> Should issue a warning but still return true
        QVERIFY(TextureParser::ensureImageIsCompatibleWithTexture(&img2, &tex2dArray));
    }

    void checkTextureSharing()
    {
        // GIVEN
        GLTF2Context context;
        QFile file(QStringLiteral(ASSETS "simple_cube_with_textures_shared.gltf"));
        file.open(QIODevice::ReadOnly);

        // THEN
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull());

        ImageParser imgParser(QDir(QString(ASSETS)));
        bool success = imgParser.parse(json[KEY_IMAGES].toArray(), &context);

        // THEN
        QVERIFY(success);

        // WHEN
        TextureSamplerParser samplerParser;
        success = samplerParser.parse(json[KEY_TEXTURE_SAMPLERS].toArray(), &context);

        // THEN
        QVERIFY(success);

        // WHEN
        TextureParser parser;
        success = parser.parse(json[KEY_TEXTURES].toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.texturesCount(), 2);

        QVERIFY(!context.texture(0).texture);
        QVERIFY(!context.texture(1).texture);

        // WHEN
        context.getOrAllocateTexture(0);
        context.getOrAllocateTexture(1);

        // THEN
        QVERIFY(context.texture(0).texture);
        QVERIFY(context.texture(1).texture);
        QCOMPARE(context.texture(0).texture, context.texture(1).texture);
    }
};

QTEST_MAIN(tst_TextureParser)

#include "tst_textureparser.moc"
