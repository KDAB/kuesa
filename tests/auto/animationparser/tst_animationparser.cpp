/*
    tst_animationparser.cpp

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
#include <Kuesa/private/animationparser_p.h>
#include <Kuesa/private/bufferviewsparser_p.h>
#include <Kuesa/private/bufferparser_p.h>
#include <Kuesa/private/bufferaccessorparser_p.h>
#include <Kuesa/private/nodeparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

#include <Qt3DAnimation/QAnimationClip>

using namespace Kuesa;
using namespace GLTF2Import;

const QLatin1String KEY_BUFFERS = QLatin1String("buffers");
const QLatin1String KEY_BUFFERVIEWS = QLatin1String("bufferViews");
const QLatin1String KEY_ACCESSORS = QLatin1String("accessors");
const QLatin1String KEY_NODES = QLatin1String("nodes");
const QLatin1String KEY_ANIMATIONS = QLatin1String("animations");

class tst_AnimationParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("animationCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "animated_cube_lot_rot_scale.gltf")
                               << true
                               << 1;
        QTest::newRow("Empty") << QStringLiteral(ASSETS "animated_cube_empty.gltf")
                               << false
                               << 0;
        QTest::newRow("Missing-Channels") << QStringLiteral(ASSETS "animated_cube_missing_channels.gltf")
                                          << false
                                          << 0;
        QTest::newRow("Missing-Samplers") << QStringLiteral(ASSETS "animated_cube_missing_samplers.gltf")
                                          << false
                                          << 0;
        QTest::newRow("Invalid-Sampler") << QStringLiteral(ASSETS "animated_cube_invalid_sampler_index.gltf")
                                         << false
                                         << 0;
        QTest::newRow("Invalid-Input-Accessor") << QStringLiteral(ASSETS "animated_cube_invalid_input_accessor_index.gltf")
                                                << false
                                                << 0;
        QTest::newRow("Invalid-Output-Accessor") << QStringLiteral(ASSETS "animated_cube_invalid_output_accessor_index.gltf")
                                                 << false
                                                 << 0;
        QTest::newRow("Invalid-Target-Node") << QStringLiteral(ASSETS "animated_cube_invalid_target_node_index.gltf")
                                             << false
                                             << 0;
        QTest::newRow("Invalid-Interpolation") << QStringLiteral(ASSETS "animated_cube_invalid_sampler_interpolation.gltf")
                                               << false
                                               << 0;
        QTest::newRow("Sampler-Missing-InputAccessor") << QStringLiteral(ASSETS "animated_cube_sampler_missing_input.gltf")
                                                       << false
                                                       << 0;
        QTest::newRow("Sampler-Missing-OutputAccessor") << QStringLiteral(ASSETS "animated_cube_sampler_missing_output.gltf")
                                                        << false
                                                        << 0;
        QTest::newRow("Sampler-Missing-Interpolation") << QStringLiteral(ASSETS "animated_cube_sampler_missing_interpolation.gltf")
                                                       << true
                                                       << 1;
        QTest::newRow("Channel-Missing-Target") << QStringLiteral(ASSETS "animated_cube_channel_missing_target.gltf")
                                                << false
                                                << 0;
        QTest::newRow("Channel-Missing-Sampler") << QStringLiteral(ASSETS "animated_cube_channel_missing_sampler.gltf")
                                                 << false
                                                 << 0;
        QTest::newRow("Incompatible-Input-Output-Accessors") << QStringLiteral(ASSETS "animated_cube_sampler_incompatible_input_output.gltf")
                                                             << false
                                                             << 0;
        QTest::newRow("Cubic-Animation") << QStringLiteral(ASSETS "animationparser_cubic.gltf")
                                         << true
                                         << 1;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, animationCount);

        // GIVEN
        GLTF2Context context;
        BufferParser bufferParser(QDir(ASSETS));
        BufferViewsParser bufferViewParser;
        BufferAccessorParser accessorParser;
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
        const bool initSuccess = bufferParser.parse(rootObj.value(KEY_BUFFERS).toArray(), &context) &&
                bufferViewParser.parse(rootObj.value(KEY_BUFFERVIEWS).toArray(), &context) &&
                accessorParser.parse(rootObj.value(KEY_ACCESSORS).toArray(), &context) &&
                nodeParser.parse(rootObj.value(KEY_NODES).toArray(), &context);
        // THEN
        QVERIFY(initSuccess);

        // WHEN
        AnimationParser parser;
        bool success = parser.parse(rootObj.value(KEY_ANIMATIONS).toArray(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.animationsCount(), animationCount);
    }

    void checkCorrectChannelName_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<QString>("path");
        QTest::addColumn<int>("componentCount");

        QTest::newRow("Translation") << QStringLiteral(ASSETS "animated_cube_loc.gltf")
                                     << true
                                     << "Location_1"
                                     << 3;

        QTest::newRow("Rotation") << QStringLiteral(ASSETS "animated_cube_rot.gltf")
                                  << true
                                  << "Rotation_1"
                                  << 4;

        QTest::newRow("Scale") << QStringLiteral(ASSETS "animated_cube_scale.gltf")
                               << true
                               << "Scale3D_1"
                               << 3;
    }

    void checkCorrectChannelName()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(QString, path);
        QFETCH(int, componentCount);

        // GIVEN
        GLTF2Context context;
        BufferParser bufferParser(QDir(ASSETS));
        BufferViewsParser bufferViewParser;
        BufferAccessorParser accessorParser;
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
        const bool initSuccess = bufferParser.parse(rootObj.value(KEY_BUFFERS).toArray(), &context) &&
                bufferViewParser.parse(rootObj.value(KEY_BUFFERVIEWS).toArray(), &context) &&
                accessorParser.parse(rootObj.value(KEY_ACCESSORS).toArray(), &context) &&
                nodeParser.parse(rootObj.value(KEY_NODES).toArray(), &context);
        // THEN
        QVERIFY(initSuccess);

        // WHEN
        AnimationParser parser;
        bool success = parser.parse(rootObj.value(KEY_ANIMATIONS).toArray(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.animationsCount(), 1);

        Animation animation = context.animation(0);
        QCOMPARE(animation.clipData.channelCount(), 1);
        QCOMPARE(animation.clipData.begin()->channelComponentCount(), componentCount);
        QCOMPARE(animation.clipData.begin()->name(), path);
    }
};

QTEST_APPLESS_MAIN(tst_AnimationParser)

#include "tst_animationparser.moc"
