/*
    tst_texturesamplerparser.cpp

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

#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/private/texturesamplerparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_TextureSamplerParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("samplersCount");
        QTest::addColumn<Qt3DRender::QAbstractTexture::Filter>("minFilter");
        QTest::addColumn<Qt3DRender::QAbstractTexture::Filter>("magFilter");
        QTest::addColumn<Qt3DRender::QTextureWrapMode::WrapMode>("wrapS");
        QTest::addColumn<Qt3DRender::QTextureWrapMode::WrapMode>("wrapT");

        QTest::newRow("Complete") << QStringLiteral(ASSETS "texturesampler_complete.gltf")
                                  << true
                                  << 1
                                  << Qt3DRender::QAbstractTexture::Linear
                                  << Qt3DRender::QAbstractTexture::Nearest
                                  << Qt3DRender::QTextureWrapMode::Repeat
                                  << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("Invalid_Mag") << QStringLiteral(ASSETS "texturesampler_invalid_magnification.gltf")
                                     << false
                                     << 0
                                     << Qt3DRender::QAbstractTexture::Linear
                                     << Qt3DRender::QAbstractTexture::Nearest
                                     << Qt3DRender::QTextureWrapMode::Repeat
                                     << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("Invalid_Min") << QStringLiteral(ASSETS "texturesampler_invalid_minification.gltf")
                                     << false
                                     << 0
                                     << Qt3DRender::QAbstractTexture::Linear
                                     << Qt3DRender::QAbstractTexture::Nearest
                                     << Qt3DRender::QTextureWrapMode::Repeat
                                     << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("Invalid_WrapModeS") << QStringLiteral(ASSETS "texturesampler_invalid_wraps.gltf")
                                           << false
                                           << 0
                                           << Qt3DRender::QAbstractTexture::Linear
                                           << Qt3DRender::QAbstractTexture::Nearest
                                           << Qt3DRender::QTextureWrapMode::Repeat
                                           << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("Invalid_WrapModeT") << QStringLiteral(ASSETS "texturesampler_invalid_wrapt.gltf")
                                           << false
                                           << 0
                                           << Qt3DRender::QAbstractTexture::Linear
                                           << Qt3DRender::QAbstractTexture::Nearest
                                           << Qt3DRender::QTextureWrapMode::Repeat
                                           << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("Empty") << QStringLiteral(ASSETS "texturesampler_empty.gltf")
                               << false
                               << 0
                               << Qt3DRender::QAbstractTexture::Linear
                               << Qt3DRender::QAbstractTexture::Nearest
                               << Qt3DRender::QTextureWrapMode::Repeat
                               << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("Empty_Sampler") << QStringLiteral(ASSETS "texturesampler_empty_sampler.gltf")
                                       << true
                                       << 1
                                       << Qt3DRender::QAbstractTexture::Linear
                                       << Qt3DRender::QAbstractTexture::Linear
                                       << Qt3DRender::QTextureWrapMode::Repeat
                                       << Qt3DRender::QTextureWrapMode::Repeat;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, samplersCount);
        QFETCH(Qt3DRender::QAbstractTexture::Filter, minFilter);
        QFETCH(Qt3DRender::QAbstractTexture::Filter, magFilter);
        QFETCH(Qt3DRender::QTextureWrapMode::WrapMode, wrapS);
        QFETCH(Qt3DRender::QTextureWrapMode::WrapMode, wrapT);

        // GIVEN
        GLTF2Context context;
        TextureSamplerParser parser;
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.textureSamplersCount(), samplersCount);
        if (success) {
            const TextureSampler &textureSampler = context.textureSampler(0);
            QCOMPARE(textureSampler.minificationFilter, minFilter);
            QCOMPARE(textureSampler.magnificationFilter, magFilter);
            QCOMPARE(textureSampler.textureWrapMode->x(), wrapS);
            QCOMPARE(textureSampler.textureWrapMode->y(), wrapT);
        }
    }
};

QTEST_APPLESS_MAIN(tst_TextureSamplerParser)

#include "tst_texturesamplerparser.moc"
