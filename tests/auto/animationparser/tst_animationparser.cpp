/*
    tst_animationparser.cpp

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
#include <Kuesa/private/animationparser_p.h>
#include <Kuesa/private/bufferviewsparser_p.h>
#include <Kuesa/private/bufferparser_p.h>
#include <Kuesa/private/bufferaccessorparser_p.h>
#include <Kuesa/private/nodeparser_p.h>
#include <Kuesa/private/gltf2context_p.h>
#include <Kuesa/private/gltf2keys_p.h>

#include <Qt3DAnimation/QAnimationClip>

using namespace Kuesa;
using namespace GLTF2Import;


class tst_AnimationParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<size_t>("animationCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "animated_cube_lot_rot_scale.gltf")
                               << true
                               << size_t(1);
        QTest::newRow("Empty") << QStringLiteral(ASSETS "animated_cube_empty.gltf")
                               << false
                               << size_t(0);
        QTest::newRow("Missing-Channels") << QStringLiteral(ASSETS "animated_cube_missing_channels.gltf")
                                          << false
                                          << size_t(0);
        QTest::newRow("Missing-Samplers") << QStringLiteral(ASSETS "animated_cube_missing_samplers.gltf")
                                          << false
                                          << size_t(0);
        QTest::newRow("Invalid-Sampler") << QStringLiteral(ASSETS "animated_cube_invalid_sampler_index.gltf")
                                         << false
                                         << size_t(0);
        QTest::newRow("Invalid-Input-Accessor") << QStringLiteral(ASSETS "animated_cube_invalid_input_accessor_index.gltf")
                                                << false
                                                << size_t(0);
        QTest::newRow("Invalid-Output-Accessor") << QStringLiteral(ASSETS "animated_cube_invalid_output_accessor_index.gltf")
                                                 << false
                                                 << size_t(0);
        QTest::newRow("Invalid-Target-Node") << QStringLiteral(ASSETS "animated_cube_invalid_target_node_index.gltf")
                                             << false
                                             << size_t(0);
        QTest::newRow("Invalid-Interpolation") << QStringLiteral(ASSETS "animated_cube_invalid_sampler_interpolation.gltf")
                                               << false
                                               << size_t(0);
        QTest::newRow("Sampler-Missing-InputAccessor") << QStringLiteral(ASSETS "animated_cube_sampler_missing_input.gltf")
                                                       << false
                                                       << size_t(0);
        QTest::newRow("Sampler-Missing-OutputAccessor") << QStringLiteral(ASSETS "animated_cube_sampler_missing_output.gltf")
                                                        << false
                                                        << size_t(0);
        QTest::newRow("Sampler-Missing-Interpolation") << QStringLiteral(ASSETS "animated_cube_sampler_missing_interpolation.gltf")
                                                       << true
                                                       << size_t(1);
        QTest::newRow("Channel-Missing-Target") << QStringLiteral(ASSETS "animated_cube_channel_missing_target.gltf")
                                                << false
                                                << size_t(0);
        QTest::newRow("Channel-Missing-Sampler") << QStringLiteral(ASSETS "animated_cube_channel_missing_sampler.gltf")
                                                 << false
                                                 << size_t(0);
        QTest::newRow("Incompatible-Input-Output-Accessors") << QStringLiteral(ASSETS "animated_cube_sampler_incompatible_input_output.gltf")
                                                             << false
                                                             << size_t(0);
        QTest::newRow("Cubic-Animation") << QStringLiteral(ASSETS "animationparser_cubic.gltf")
                                         << true
                                         << size_t(1);
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(size_t, animationCount);

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
                                     << "translation_1"
                                     << 3;

        QTest::newRow("Rotation") << QStringLiteral(ASSETS "animated_cube_rot.gltf")
                                  << true
                                  << "Rotation_1"
                                  << 4;

        QTest::newRow("Scale") << QStringLiteral(ASSETS "animated_cube_scale.gltf")
                               << true
                               << "scale3D_1"
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
        QCOMPARE(context.animationsCount(), size_t(1));

        Animation animation = context.animation(0);
        QCOMPARE(animation.clipData.channelCount(), 1);
        QCOMPARE(animation.clipData.begin()->channelComponentCount(), componentCount);
        QCOMPARE(animation.clipData.begin()->name(), path);
    }

    void checkParseEXTPropertyAnimation_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<QVector<ChannelInfo>>("expectedChannelsInfo");
        QTest::addColumn<bool>("expectedSuccess");

        QTest::addRow("simple_material_target")
                << QStringLiteral(ASSETS "ext_property_animation_simple_material.json")
                << QVector<ChannelInfo> {
                      { 883, { AnimationTarget::Material, 883, QStringLiteral("alphaCutoff") } }
                   }
                << true;
        QTest::addRow("simple_to_complex_all_targets")
                << QStringLiteral(ASSETS "ext_property_animation_simple_to_complex.json")
                << QVector<ChannelInfo> {
                        { 26, { AnimationTarget::Node, 1, QStringLiteral("translation") } },
                        { 25, { AnimationTarget::Node, 2, QStringLiteral("rotation") } },
                        { 24, { AnimationTarget::Node, 3, QStringLiteral("scale") } },
                        { 23, { AnimationTarget::Camera, 4, QStringLiteral("orthographic/xmag") } },
                        { 22, { AnimationTarget::Camera, 5, QStringLiteral("orthographic/ymag") } },
                        { 21, { AnimationTarget::Camera, 6, QStringLiteral("orthographic/zfar") } },
                        { 20, { AnimationTarget::Camera, 7, QStringLiteral("orthographic/znear") } },
                        { 19, { AnimationTarget::Camera, 8, QStringLiteral("perspective/aspectRatio") } },
                        { 18, { AnimationTarget::Camera, 9, QStringLiteral("perspective/yfov") } },
                        { 17, { AnimationTarget::Camera, 10, QStringLiteral("perspective/zfar") } },
                        { 16, { AnimationTarget::Camera, 11, QStringLiteral("perspective/znear") } },
                        { 15, { AnimationTarget::Material, 12, QStringLiteral("pbrMetallicRoughness/baseColorFactor") } },
                        { 14, { AnimationTarget::Material, 13, QStringLiteral("pbrMetallicRoughness/metallicFactor") } },
                        { 13, { AnimationTarget::Material, 14, QStringLiteral("pbrMetallicRoughness/roughnessFactor") } },
                        { 12, { AnimationTarget::Material, 15, QStringLiteral("alphaCutoff") } },
                        { 11, { AnimationTarget::Material, 16, QStringLiteral("emissiveFactor") } },
                        { 10, { AnimationTarget::Material, 17, QStringLiteral("normalTexture/scale") } },
                        { 9, { AnimationTarget::Material, 18, QStringLiteral("occlusionTexture/strength") } },
                        { 8, { AnimationTarget::Material, 19, QStringLiteral("extensions/KHR_materials_pbrSpecularGlossiness/diffuseFactor") } },
                        { 7, { AnimationTarget::Material, 20, QStringLiteral("extensions/KHR_materials_pbrSpecularGlossiness/specularFactor") } },
                        { 6, { AnimationTarget::Material, 21, QStringLiteral("extensions/KHR_materials_pbrSpecularGlossiness/glossinessFactor") } },
                        { 5, { AnimationTarget::Light, 22, QStringLiteral("color") } },
                        { 4, { AnimationTarget::Light, 23, QStringLiteral("intensity") } },
                        { 3, { AnimationTarget::Light, 24, QStringLiteral("innerConeAngle") } },
                        { 2, { AnimationTarget::Light, 25, QStringLiteral("outerConeAngle") } },
                        { 1, { AnimationTarget::Node, 26, QStringLiteral("extensions/AVR_lights_static/strength") } }
                   }
                << true;
    }

    void checkParseEXTPropertyAnimation()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(QVector<ChannelInfo>, expectedChannelsInfo);
        QFETCH(bool, expectedSuccess);

        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonObject rootObj = json.object();

        // THEN
        QVERIFY(!json.isNull() && json.isObject() && !rootObj.isEmpty());

        // WHEN
        ExtPropertyAnimationHandler handler;
        const bool success = handler.parse(rootObj);

        // THEN
        QCOMPARE(success, expectedSuccess);

        const QVector<ChannelInfo> actualChannelsInfo = handler.channelsInfo();
        QCOMPARE(actualChannelsInfo.size(), expectedChannelsInfo.size());

        for (int i = 0, m = actualChannelsInfo.size(); i < m; ++i) {
            const ChannelInfo &actual = actualChannelsInfo.at(i);
            const ChannelInfo &expected = expectedChannelsInfo.at(i);

            QCOMPARE(actual.sampler, expected.sampler);

            const AnimationTarget &actualTarget = actual.target;
            const AnimationTarget &expectedTarget = expected.target;

            QCOMPARE(actualTarget.targetNodeId, expectedTarget.targetNodeId);
            QCOMPARE(actualTarget.type, expectedTarget.type);
            QCOMPARE(actualTarget.path, expectedTarget.path);
        }
    }
};

QTEST_APPLESS_MAIN(tst_AnimationParser)

#include "tst_animationparser.moc"
