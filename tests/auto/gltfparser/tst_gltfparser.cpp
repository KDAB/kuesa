/*
    tst_gltfparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include <qtkuesa-config.h>
#include <QtTest/QTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/SceneEntity>
#include <Kuesa/private/gltf2parser_p.h>
#include <Kuesa/private/gltf2uri_p.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>
#include <Kuesa/MetallicRoughnessProperties>
#include <Qt3DCore/QSkeleton>
#include <Qt3DCore/QJoint>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QCamera>
#include <Kuesa/LayerCollection>
#include <Kuesa/DirectionalLight>
#include <Kuesa/SpotLight>
#include <Kuesa/PointLight>
#include <Kuesa/private/kuesa_utils_p.h>
#include <array>
#include <atomic>

using namespace Kuesa;
using namespace GLTF2Import;

class GLTF2ParserNoTextures : public GLTF2Parser
{
public:
    GLTF2ParserNoTextures(SceneEntity *sceneEntity = nullptr)
        : GLTF2Parser(sceneEntity)
    {
    }

    QVector<KeyParserFuncPair> prepareParsers() override
    {
        auto parsers = GLTF2Parser::prepareParsers();
        const auto textureParser = std::find_if(std::begin(parsers), std::end(parsers),
                                                [](const KeyParserFuncPair &parser) -> bool {
                                                    return parser.first == "textures";
                                                });
        if (textureParser != std::end(parsers))
            parsers.erase(textureParser);
        return parsers;
    }
};

class tst_GLTFParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkContextUriHandling_data()
    {
        QTest::addColumn<QString>("uri");
        QTest::addColumn<bool>("success");

        QTest::newRow("Valid URI") << "data:application/octet-stream;base64,S3Vlc2E=" << true;
        QTest::newRow("Missing data scheme") << "foo" << false;
        QTest::newRow("Missing header") << "data:foo" << false;
        QTest::newRow("Not Base64") << "data:application/octet-stream,S3Vlc2E=" << false;
    }

    void checkContextUriHandling()
    {
        // GIVEN
        QFETCH(QString, uri);
        QFETCH(bool, success);

        // WHEN
        const auto res = Uri::parseEmbeddedData(uri);

        // THEN
        QCOMPARE(!res.isNull(), success);
    }

    void checkAsset_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<int>("fileEntityCount");
        QTest::addColumn<int>("fileComponentCount");

        QTest::newRow("Mesh_PrimitiveAttribute_00") << "Mesh_PrimitiveAttribute/Mesh_PrimitiveAttribute_00.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveAttribute_01") << "Mesh_PrimitiveAttribute/Mesh_PrimitiveAttribute_01.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveAttribute_02") << "Mesh_PrimitiveAttribute/Mesh_PrimitiveAttribute_02.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveAttribute_03") << "Mesh_PrimitiveAttribute/Mesh_PrimitiveAttribute_03.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveAttribute_04") << "Mesh_PrimitiveAttribute/Mesh_PrimitiveAttribute_04.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveAttribute_05") << "Mesh_PrimitiveAttribute/Mesh_PrimitiveAttribute_05.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveAttribute_06") << "Mesh_PrimitiveAttribute/Mesh_PrimitiveAttribute_06.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_00") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_00.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_01") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_01.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_02") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_02.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_03") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_03.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_04") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_04.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_05") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_05.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_06") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_06.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_07") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_07.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_08") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_08.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_09") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_09.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_10") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_10.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_11") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_11.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_12") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_12.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_13") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_13.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_14") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_14.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveMode_15") << "Mesh_PrimitiveMode/Mesh_PrimitiveMode_15.gltf" << 2 << 3;
        QTest::newRow("Mesh_Primitives_00") << "Mesh_Primitives/Mesh_Primitives_00.gltf" << 3 << 5;
        QTest::newRow("Mesh_Primitives_01") << "Mesh_Primitives/Mesh_Primitives_01.gltf" << 3 << 5;
        QTest::newRow("Mesh_Primitives_02") << "Mesh_Primitives/Mesh_Primitives_02.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_00") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_00.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_01") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_01.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_02") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_02.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_03") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_03.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_04") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_04.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_05") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_05.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_06") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_06.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_07") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_07.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitivesUV_08") << "Mesh_PrimitivesUV/Mesh_PrimitivesUV_08.gltf" << 3 << 5;
        QTest::newRow("Mesh_PrimitiveVertexColor_00") << "Mesh_PrimitiveVertexColor/Mesh_PrimitiveVertexColor_00.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveVertexColor_01") << "Mesh_PrimitiveVertexColor/Mesh_PrimitiveVertexColor_01.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveVertexColor_02") << "Mesh_PrimitiveVertexColor/Mesh_PrimitiveVertexColor_02.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveVertexColor_03") << "Mesh_PrimitiveVertexColor/Mesh_PrimitiveVertexColor_03.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveVertexColor_04") << "Mesh_PrimitiveVertexColor/Mesh_PrimitiveVertexColor_04.gltf" << 2 << 3;
        QTest::newRow("Mesh_PrimitiveVertexColor_00") << "Mesh_PrimitiveVertexColor/Mesh_PrimitiveVertexColor_05.gltf" << 2 << 3;
        QTest::newRow("Node_Attribute_00") << "Node_Attribute/Node_Attribute_00.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_01") << "Node_Attribute/Node_Attribute_01.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_02") << "Node_Attribute/Node_Attribute_02.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_03") << "Node_Attribute/Node_Attribute_03.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_04") << "Node_Attribute/Node_Attribute_04.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_05") << "Node_Attribute/Node_Attribute_05.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_06") << "Node_Attribute/Node_Attribute_06.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_07") << "Node_Attribute/Node_Attribute_07.gltf" << 4 << 6;
        QTest::newRow("Node_Attribute_08") << "Node_Attribute/Node_Attribute_08.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_00") << "Node_NegativeScale/Node_NegativeScale_00.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_01") << "Node_NegativeScale/Node_NegativeScale_01.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_02") << "Node_NegativeScale/Node_NegativeScale_02.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_03") << "Node_NegativeScale/Node_NegativeScale_03.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_04") << "Node_NegativeScale/Node_NegativeScale_04.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_05") << "Node_NegativeScale/Node_NegativeScale_05.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_06") << "Node_NegativeScale/Node_NegativeScale_06.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_07") << "Node_NegativeScale/Node_NegativeScale_07.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_08") << "Node_NegativeScale/Node_NegativeScale_08.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_09") << "Node_NegativeScale/Node_NegativeScale_09.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_10") << "Node_NegativeScale/Node_NegativeScale_10.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_11") << "Node_NegativeScale/Node_NegativeScale_11.gltf" << 4 << 6;
        QTest::newRow("Node_NegativeScale_12") << "Node_NegativeScale/Node_NegativeScale_12.gltf" << 4 << 6;
    }

    void checkAsset()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(int, fileEntityCount);
        QFETCH(int, fileComponentCount);

        GLTF2Context ctx;
        GLTF2Parser parser;

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "GeneratedAssets/") + filePath);

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();

        // THEN
        auto *res = parser.sceneRoots().first();
        QVERIFY(res);
        res->makeActive();

        QList<Qt3DCore::QEntity *> children = res->findChildren<Qt3DCore::QEntity *>();
        Qt3DCore::QComponentVector components;
        for (Qt3DCore::QEntity *e : children)
            components << e->components();
        QCOMPARE(fileEntityCount, children.size());
        QCOMPARE(fileComponentCount, components.size());
    }

    void checkMeshCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.meshes()->names().size(), 1);
        QVERIFY(scene.mesh(QLatin1String("Cube_0")));
    }

    void checkCameraCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube.gltf"));
        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.cameras()->names().size(), 1);
        const Qt3DRender::QCamera *camera = scene.camera(QStringLiteral("Camera"));
        QVERIFY(camera);
    }

    void checkLayerCollection()
    {
        // GIVEN
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube_with_layers.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.layers()->names().size(), 3);
        QVERIFY(scene.layer(QLatin1String("VL0")));
        QVERIFY(scene.layer(QLatin1String("VL1")));
        QVERIFY(scene.layer(QLatin1String("VL2")));
    }

    void checkEntityCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.entities()->names().size(), 3);
        QVERIFY(scene.entity(QLatin1String("Camera")));
        QVERIFY(scene.entity(QLatin1String("Cube")));
        QVERIFY(scene.entity(QLatin1String("Lamp")));
    }

    void checkEntityCollectionWithDuplicateNames()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube_same_names.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.entities()->names().size(), 3);
        QVERIFY(scene.entity(QLatin1String("Node")));
        QVERIFY(scene.entity(QLatin1String("Node_1")));
        QVERIFY(scene.entity(QLatin1String("Node_2")));
    }

    void checkImageParsing_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("parsingSucceeded");
        QTest::addColumn<int>("imageCount");

        QTest::newRow("Valid") << "simple_cube_with_images.gtlf" << true << 2;
        QTest::newRow("BufferView") << "simple_cube_with_images_buffer_view.gtlf" << false << 0;
        QTest::newRow("WrongKey") << "simple_cube_with_images_wrong_key.gltf" << true << 0;
    }

    void checkImageParsing()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(bool, parsingSucceeded);
        QFETCH(int, imageCount);

        GLTF2Context ctx;
        GLTF2ParserNoTextures parser;

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS + filePath));

        // THEN
        QCOMPARE(parsingSuccessful, parsingSucceeded);
        QCOMPARE(parser.context()->imagesCount(), imageCount);
    }

    void checkTextureSamplerParsing_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("parsingSucceeded");
        QTest::addColumn<int>("samplerCount");
        QTest::addColumn<Qt3DRender::QAbstractTexture::Filter>("minFilter");
        QTest::addColumn<Qt3DRender::QAbstractTexture::Filter>("magFilter");
        QTest::addColumn<Qt3DRender::QTextureWrapMode::WrapMode>("wrapS");
        QTest::addColumn<Qt3DRender::QTextureWrapMode::WrapMode>("wrapT");

        QTest::newRow("NEAREST_LINEAR_CLAMP_CLAMP") << "simple_cube_with_samplers_nearest_linear_clamp_clamp.gltf"
                                                    << true << 2
                                                    << Qt3DRender::QAbstractTexture::Nearest
                                                    << Qt3DRender::QAbstractTexture::Linear
                                                    << Qt3DRender::QTextureWrapMode::ClampToEdge
                                                    << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("NEAREST_LINEAR_REPEAT_CLAMP") << "simple_cube_with_samplers_nearest_linear_repeat_clamp.gltf"
                                                     << true << 2
                                                     << Qt3DRender::QAbstractTexture::Nearest
                                                     << Qt3DRender::QAbstractTexture::Linear
                                                     << Qt3DRender::QTextureWrapMode::Repeat
                                                     << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("NEAREST_MIPMAP_NEAREST_LINEAR_REPEAT_MIRRORED") << "simple_cube_with_samplers_nearest_mipmap_nearest_linear_repeat_mirrored.gltf"
                                                                       << true << 2
                                                                       << Qt3DRender::QAbstractTexture::NearestMipMapNearest
                                                                       << Qt3DRender::QAbstractTexture::Linear
                                                                       << Qt3DRender::QTextureWrapMode::Repeat
                                                                       << Qt3DRender::QTextureWrapMode::MirroredRepeat;

        QTest::newRow("LINEAR_MIPMAP_NEAREST_LINEAR_REPEAT_MIRRORED") << "simple_cube_with_samplers_linear_mipmap_nearest_linear_repeat_mirrored.gltf"
                                                                      << true << 2
                                                                      << Qt3DRender::QAbstractTexture::LinearMipMapNearest
                                                                      << Qt3DRender::QAbstractTexture::Linear
                                                                      << Qt3DRender::QTextureWrapMode::Repeat
                                                                      << Qt3DRender::QTextureWrapMode::MirroredRepeat;

        QTest::newRow("LINEAR_MIPMAP_LINEAR_LINEAR_REPEAT_MIRRORED") << "simple_cube_with_samplers_linear_mipmap_linear_linear_repeat_mirrored.gltf"
                                                                     << true << 2
                                                                     << Qt3DRender::QAbstractTexture::LinearMipMapLinear
                                                                     << Qt3DRender::QAbstractTexture::Linear
                                                                     << Qt3DRender::QTextureWrapMode::Repeat
                                                                     << Qt3DRender::QTextureWrapMode::MirroredRepeat;

        QTest::newRow("NEAREST_MIPMAP_LINEAR_LINEAR_REPEAT_MIRRORED") << "simple_cube_with_samplers_nearest_mipmap_linear_linear_repeat_mirrored.gltf"
                                                                      << true << 2
                                                                      << Qt3DRender::QAbstractTexture::NearestMipMapLinear
                                                                      << Qt3DRender::QAbstractTexture::Linear
                                                                      << Qt3DRender::QTextureWrapMode::Repeat
                                                                      << Qt3DRender::QTextureWrapMode::MirroredRepeat;

        QTest::newRow("INVALID_INVALID_INVALID_INVALID") << "simple_cube_with_samplers_invalid_invalid_invalid_invalid.gltf"
                                                         << false << 0
                                                         << Qt3DRender::QAbstractTexture::Linear
                                                         << Qt3DRender::QAbstractTexture::Linear
                                                         << Qt3DRender::QTextureWrapMode::ClampToEdge
                                                         << Qt3DRender::QTextureWrapMode::ClampToEdge;

        QTest::newRow("WrongKey") << "simple_cube_with_samplers_wrong_key.gltf" << true << 0
                                  << Qt3DRender::QAbstractTexture::Linear
                                  << Qt3DRender::QAbstractTexture::Linear
                                  << Qt3DRender::QTextureWrapMode::ClampToEdge
                                  << Qt3DRender::QTextureWrapMode::ClampToEdge;
    }

    void checkTextureSamplerParsing()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(bool, parsingSucceeded);
        QFETCH(int, samplerCount);
        QFETCH(Qt3DRender::QAbstractTexture::Filter, minFilter);
        QFETCH(Qt3DRender::QAbstractTexture::Filter, magFilter);
        QFETCH(Qt3DRender::QTextureWrapMode::WrapMode, wrapS);
        QFETCH(Qt3DRender::QTextureWrapMode::WrapMode, wrapT);

        GLTF2Context ctx;
        GLTF2ParserNoTextures parser;

        parser.setContext(&ctx);

        // WHEN
       const bool parsingSuccessful = parser.parse(QString(ASSETS + filePath));

       // THEN
       QCOMPARE(parsingSuccessful, parsingSucceeded);
       QCOMPARE(parser.context()->textureSamplersCount(), samplerCount);
       for (int i = 0; i < samplerCount; ++i) {
           const TextureSampler s = parser.context()->textureSampler(i);
           QCOMPARE(s.minificationFilter, minFilter);
           QCOMPARE(s.magnificationFilter, magFilter);
           QCOMPARE(s.textureWrapMode->x(), wrapS);
           QCOMPARE(s.textureWrapMode->y(), wrapT);
       }
    }

    void checkSceneParsing_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("NoDefaultScene") << QString(ASSETS "simple_cube_with_images_no_scene.gltf") << true;
        QTest::newRow("InvalidDefaultScene") << QString(ASSETS "simple_cube_with_images_invalid_scene.gltf") << false;
        QTest::newRow("InvalidDefaultSceneNegative") << QString(ASSETS "simple_cube_with_images_invalid_scene_negative.gltf") << false;
    }

    void checkSceneParsing()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(bool, expectedResult);

        SceneEntity scene;

        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(filePath);

        // THEN
        QCOMPARE(parsingSuccessful, expectedResult);
    }

    void checkTextureImageCollection()
    {
        SceneEntity scene;

        GLTF2Context ctx;
        GLTF2ParserNoTextures parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube_with_images.gtlf"));
        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(parser.context()->imagesCount(), 2);
        QCOMPARE(parser.context()->image(0).name, QStringLiteral("diffuse.png"));
        QCOMPARE(parser.context()->image(1).name, QStringLiteral("normal.png"));
    }

    void checkTextureParsing_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("parsingSucceeded");
        QTest::addColumn<int>("textureCount");

        QTest::newRow("Valid") << "simple_cube_with_textures.gltf" << true << 2;
        QTest::newRow("ValidDDSTexture") << "simple_cube_with_textures_dds.gltf" << true << 1;
        QTest::newRow("NoSource") << "simple_cube_with_textures_no_source.gltf" << true << 2;
        QTest::newRow("NoSampler") << "simple_cube_with_textures_no_sampler.gltf" << true << 2;
        QTest::newRow("InvalidSource") << "simple_cube_with_textures_invalid_source.gltf" << false << 1;
        QTest::newRow("InvalidSampler") << "simple_cube_with_textures_invalid_sampler.gltf" << false << 1;
    }

    void checkTextureParsing()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(bool, parsingSucceeded);
        QFETCH(int, textureCount);

        GLTF2Context ctx;
        GLTF2Parser parser;

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS + filePath));

        // THEN
        QCOMPARE(parsingSuccessful, parsingSucceeded);
        QCOMPARE(parser.context()->texturesCount(), textureCount);
    }

    void checkDDSTextureExtension()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube_with_textures_dds.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.textures()->names().size(), 1);
        auto textureLoader = qobject_cast<Qt3DRender::QTextureLoader *>(scene.texture(QLatin1String("diffuse")));
        QVERIFY(textureLoader);
        QVERIFY(textureLoader->source().toString().toLower().endsWith("dds"));
        QVERIFY(parser.context()->usedExtension().contains(QLatin1String("MSFT_texture_dds")));
    }

    void checkTextureCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube_with_textures.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.textures()->names().size(), 2);
        QVERIFY(scene.texture(QLatin1String("diffuse")));
        QVERIFY(scene.texture(QLatin1String("normal")));
    }

    void checkAnimationClipCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "animated_cube_lot_rot_scale.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.animationClips()->names().size(), 1);
        QVERIFY(scene.animationClip(QStringLiteral("Cube_CubeAction")));
    }

    void checkAnimationMappingCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "animated_cube_lot_rot_scale.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.animationMappings()->names().size(), 1);
        QVERIFY(scene.animationMapping(QStringLiteral("Cube_CubeAction")));
    }

    void checkMaterialParsing_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("parsingSucceeded");
        QTest::addColumn<int>("materialCount");

        QTest::newRow("Valid_PBR") << "simple_cube_with_textures.gltf" << true << 1;
        QTest::newRow("Empty_Material") << "simple_cube_with_default_material.gltf" << true << 1;
    }

    void checkMaterialParsing()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(bool, parsingSucceeded);
        QFETCH(int, materialCount);

        GLTF2Context ctx;
        GLTF2Parser parser;

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS + filePath));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QCOMPARE(res != nullptr, parsingSucceeded);
        QCOMPARE(parser.context()->materialsCount(), materialCount);
    }

    void checkMaterialCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube_with_textures.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.textures()->names().size(), 2);
        QCOMPARE(scene.materials()->names().size(), 1);

        Kuesa::MetallicRoughnessProperties *mat = static_cast<decltype(mat)>(scene.material(QStringLiteral("Material")));
        QVERIFY(mat != nullptr);
        QCOMPARE(mat->metallicFactor(), 0.0f);
        QCOMPARE(mat->roughnessFactor(), 1.0f);
        QCOMPARE(mat->emissiveFactor(), QColor::fromRgbF(0.0, 0.0, 0.0));
        QCOMPARE(mat->baseColorFactor(), QColor::fromRgbF(0.15, 0.64, 0.04, 1.0));
    }

    void checkSkeletonCollection()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "skins_valid.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.skeletons()->names().size(), 1);
        QVERIFY(scene.skeleton(QStringLiteral("Armature")));
    }

    void checkJointsHierarchy_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<QVector<QString>>("expectedNames");

        QVector<QString> expectedNames = { "Armature_Hip",
                                           "Armature_Chest",
                                           "Armature_Shoulder.R",
                                           "Armature_Arm.R",
                                           "Armature_Shoulder.L",
                                           "Armature_Arm.L",
                                           "Armature_Head" };
        QTest::newRow("Check hierarchy") << "joint_hierarchy.gltf" << expectedNames;
    }

    void checkJointsHierarchy()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(QVector<QString>, expectedNames);

        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene, true);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS + filePath));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.skeletons()->names().size(), 1);
        QCOMPARE(scene.skeletons()->names().first(), QStringLiteral("armature"));

        // WHEN
        Qt3DCore::QSkeleton *skeleton = static_cast<Qt3DCore::QSkeleton *>(scene.skeleton(QStringLiteral("armature")));

        // THEN
        QVector<QString> actualNames;
        std::function<void(const Qt3DCore::QJoint *, QVector<QString> &)> acquireNames = [&acquireNames](const Qt3DCore::QJoint *joint, QVector<QString> &actualNames) {
            actualNames.push_back(joint->name());
            for (const Qt3DCore::QJoint *childJoint : joint->childJoints())
                acquireNames(childJoint, actualNames);
        };

        acquireNames(skeleton->rootJoint(), actualNames);
        QCOMPARE(expectedNames, actualNames);
    }

    void checkAssetsHaveObjectName()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        parser.parse(QString(ASSETS "simple_cube_with_textures.gltf"));

        // THEN
        for (auto name : scene.entities()->names())
            QVERIFY(!scene.entity(name)->objectName().isEmpty());
        for (auto name : scene.textures()->names())
            QVERIFY(!scene.texture(name)->objectName().isEmpty());
        for (auto name : scene.materials()->names())
            QVERIFY(!scene.material(name)->objectName().isEmpty());
        for (auto name : scene.meshes()->names())
            QVERIFY(!scene.mesh(name)->objectName().isEmpty());
    }

    void checkExtensionsInvalid()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "extensions_invalid.gltf"));

        // THEN
        QVERIFY(!parsingSuccessful);

        // WHEN
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QCOMPARE(res, nullptr);
        QCOMPARE(parser.context()->requiredExtensions().size(), 1);
        QCOMPARE(parser.context()->requiredExtensions().front(), QLatin1String("KDAB_Some_Unknown_Extension"));
    }

    void checkExtensionsValid()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "extensions_valid.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);
        QCOMPARE(parser.context()->requiredExtensions().size(), 3);
        QCOMPARE(parser.context()->usedExtension().size(), 3);
        QCOMPARE(parser.context()->usedExtension().front(), QLatin1String("KDAB_kuesa_layers"));
        QCOMPARE(parser.context()->usedExtension()[1], QLatin1String("EXT_property_animation"));
        QCOMPARE(parser.context()->usedExtension().back(), QLatin1String("KDAB_kuesa_reflection_planes"));
    }

    void checkLayers_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<QStringList>("expectedLayersName");
        QTest::addColumn<QStringList>("expectedAssignedLayers");

        QTest::newRow("No layers") << ASSETS "no_layers.gltf" << QStringList{} << QStringList{};
        QTest::newRow("Several layers") << ASSETS "several_layers.gltf" << QStringList{ "vl1", "vl2", "vl3" } << QStringList{};
        QTest::newRow("One layer assigned") << ASSETS "one_layer_assigned.gltf" << QStringList{ "vl1", "vl2", "vl3" } << QStringList{ "vl1" };
        QTest::newRow("Several layers assigned") << ASSETS "several_layers_assigned.gltf" << QStringList{ "vl1", "vl2", "vl3" } << QStringList{ "vl1", "vl3" };
    }

    void checkLayers()
    {
        // GIVEN
        QFETCH(QString, filePath);
        QFETCH(QStringList, expectedLayersName);
        QFETCH(QStringList, expectedAssignedLayers);

        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(filePath);

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);

        QCOMPARE(expectedLayersName.size(), qint32(ctx.layersCount()));
        if (ctx.layersCount()) {
            for (const auto &layerName : qAsConst(expectedAssignedLayers)) {
                const auto *layer = scene.layer(layerName);
                QVERIFY(layer != nullptr);
            }
        }

        auto *entity = scene.entity("entity");
        if (entity) {
            const auto assignedLayers = componentsFromEntity<Qt3DRender::QLayer>(entity);
            QCOMPARE(assignedLayers.size(), expectedAssignedLayers.size());
            if (expectedAssignedLayers.size()) {
                for (const auto &layerName : qAsConst(expectedAssignedLayers)) {
                    QVERIFY(std::find_if(std::begin(assignedLayers), std::end(assignedLayers), [layerName](const Qt3DRender::QLayer *value) {
                                return value->objectName() == layerName;
                            }) != std::end(assignedLayers));
                }
            }
        }
    }

    void checkLights()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "KHR_lights_punctual/lights.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);

        const auto lights = scene.lights();
        QCOMPARE(lights->size(), 3);

        auto pointLight = qobject_cast<PointLight *>(lights->find("Point"));
        QVERIFY(pointLight);
        QCOMPARE(pointLight->color(), QColor::fromRgbF(1.0f, 0.0f, 0.0f));
        QCOMPARE(pointLight->intensity(), 20.0f);
        QCOMPARE(pointLight->objectName(), QStringLiteral("Point"));
        QCOMPARE(pointLight->range(), 0.0f);

        auto directionalLight = qobject_cast<DirectionalLight *>(lights->find("Directional"));
        QVERIFY(directionalLight);
        QCOMPARE(directionalLight->color(), QColor::fromRgbF(1.0f, 0.9f, 0.7f));
        QCOMPARE(directionalLight->intensity(), 3.0f);
        QCOMPARE(directionalLight->objectName(), QStringLiteral("Directional"));
        QCOMPARE(directionalLight->direction(), QVector3D(0.0, 0.0, -1.0));
        QCOMPARE(directionalLight->directionMode(), DirectionalLight::Local);

        auto spotLight = qobject_cast<SpotLight *>(lights->find("Spot"));
        QVERIFY(spotLight);
        QCOMPARE(spotLight->color(), QColor::fromRgbF(.3f, 0.7f, 1.0f));
        QCOMPARE(spotLight->intensity(), 40.0f);
        QCOMPARE(spotLight->objectName(), QStringLiteral("Spot"));
        QCOMPARE(spotLight->innerConeAngle(), 45);
        QCOMPARE(spotLight->outerConeAngle(), 90);
        QCOMPARE(spotLight->localDirection(), QVector3D(0.0, 0.0, -1.0));
    }

    void checkReflectionPlanes()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "KDAB_reflection_planes/reflections.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);
        QCOMPARE(scene.reflectionPlanes()->size(), 1);

        Kuesa::ReflectionPlane *reflectionPlane = qobject_cast<ReflectionPlane *>(scene.reflectionPlane("ReflectionPlane"));
        QVERIFY(reflectionPlane);
        QCOMPARE(reflectionPlane->equation(), QVector4D(1.0f, 0.0f, 0.0f, 883.0f));
    }

    void checkPlaceholder()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "KDAB_placeholder/placeholder.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);
        QCOMPARE(scene.placeholders()->size(), 1);

        Kuesa::Placeholder *placeholder = qobject_cast<Kuesa::Placeholder *>(scene.placeholder("placeholder"));
        QVERIFY(placeholder);
        QCOMPARE(placeholder->camera(), scene.camera("camera"));
        QCOMPARE(placeholder->viewport(), QRect());
        QCOMPARE(placeholder->target(), nullptr);
    }

#if defined(KUESA_DRACO_COMPRESSION)
    void checkDracoCompression()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "draco/Box.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);
    }
#endif

    void checkUseDracoCompressionIfAvailable()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "draco/Box_optional.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);
    }

    void checkEmbeddedImages()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "BoxTextured.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);
    }

    void checkEmbeddedBufferImages()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(QString(ASSETS "BoxTexturedEmbeddedBuffer.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);
    }

    void checkSparseAccessors_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<int>("bufferView");
        QTest::addColumn<float>("originalValue");

        QTest::newRow("With Original BufferView") << QString(ASSETS "SimpleSparseAccessor.gltf") << 1 << 1.f;
        QTest::newRow("Without Original BufferView") << QString(ASSETS "SimpleSparseAccessorNoBufferView.gltf") << -1 << 0.f; // original should be 0s
    }

    void checkSparseAccessors()
    {
        QFETCH(QString, filePath);
        QFETCH(int, bufferView);
        QFETCH(float, originalValue);

        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.parse(filePath);

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        Qt3DCore::QEntity *res = parser.contentRoot();

        // THEN
        QVERIFY(res != nullptr);

        auto accessor = parser.context()->accessor(1);
        QCOMPARE(accessor.sparseCount, 3);
        QCOMPARE(accessor.sparseValues.bufferViewIndex, 3);
        QCOMPARE(accessor.sparseIndices.bufferViewIndex, 2);
        QVERIFY(!accessor.bufferData.isEmpty());

        QCOMPARE(accessor.bufferViewIndex, bufferView);
        const float *data = reinterpret_cast<const float *>(accessor.bufferData.constData());
        QCOMPARE(data[3], originalValue);
    }

    void checkGLBParsing_data()
    {
        struct GLBHeader {
            quint32 magic = 0x46546C67;
            quint32 version = 2;
            quint32 length = 0;
        };
        struct ChunkHeader {
            quint32 chunkLength = 0;
            quint32 chunkType = 0x4E4F534A; // or 0x004E4942
        };

        QTest::addColumn<QByteArray>("data");
        QTest::addColumn<bool>("success");

        QFile f(QString(ASSETS + QLatin1String("Box.glb")));
        f.open(QFile::ReadOnly);
        const QByteArray fileData = f.readAll();
        QTest::addRow("Valid glb") << fileData << true;
        QTest::addRow("Empty glb") << QByteArray() << false;
        QTest::addRow("Too small glb") << QByteArray(3, 0) << false;
        {
            QByteArray data(sizeof(GLBHeader), 0);
            GLBHeader header;
            header.magic = 0x01010101;
            memcpy(data.data(), &header, sizeof(GLBHeader));
            QTest::addRow("Wrong Header Magic Id") << data << false;
        }
        {
            QByteArray data(sizeof(GLBHeader), 0);
            GLBHeader header;
            header.version = 1;
            memcpy(data.data(), &header, sizeof(GLBHeader));
            QTest::addRow("Wrong Header Version") << data << false;
        }
        {
            QByteArray data(sizeof(GLBHeader) + 10, 0);
            GLBHeader header;
            header.length = sizeof(GLBHeader) + 5;
            memcpy(data.data(), &header, sizeof(GLBHeader));
            QTest::addRow("Wrong GLB Size") << data << false;
        }
        {
            QByteArray data(sizeof(GLBHeader) + sizeof(ChunkHeader) + 10, 0);
            GLBHeader header;
            header.length = data.size();
            memcpy(data.data(), &header, sizeof(GLBHeader));
            ChunkHeader chunck;
            chunck.chunkLength = 5;
            memcpy(data.data() + sizeof(GLBHeader), &chunck, sizeof(ChunkHeader));
            QTest::addRow("Wrong Chunk Length") << data << false;
        }
        {
            QByteArray data(sizeof(GLBHeader) + sizeof(ChunkHeader) + 10, 0);
            GLBHeader header;
            header.length = data.size();
            memcpy(data.data(), &header, sizeof(GLBHeader));
            ChunkHeader chunck;
            chunck.chunkType = 0x004E4942;
            chunck.chunkLength = 10;
            memcpy(data.data() + sizeof(GLBHeader), &chunck, sizeof(ChunkHeader));
            QTest::addRow("Missing JSON chunk") << data << false;
        }
        {
            QByteArray data(sizeof(GLBHeader) + (sizeof(ChunkHeader) + 10) * 2, 0);
            GLBHeader header;
            header.length = data.size();
            memcpy(data.data(), &header, sizeof(GLBHeader));
            ChunkHeader chunck;
            chunck.chunkType = 0x004E4942;
            chunck.chunkLength = 10;
            memcpy(data.data() + sizeof(GLBHeader), &chunck, sizeof(ChunkHeader));
            memcpy(data.data() + sizeof(GLBHeader) + sizeof(ChunkHeader) + 10, &chunck, sizeof(ChunkHeader));
            QTest::addRow("Multiple BIN chunk") << data << false;
        }
        {
            QByteArray data(fileData.size() + sizeof(ChunkHeader) + 10, 0);
            memcpy(data.data(), fileData.constData(), fileData.size());
            GLBHeader *header = reinterpret_cast<GLBHeader *>(data.data());
            header->length = data.size();
            ChunkHeader chunk;
            chunk.chunkLength = 10;
            chunk.chunkType = 42;
            memcpy(data.data() + fileData.size(), &chunk, sizeof(ChunkHeader));
            QTest::addRow("Skip unknown chunk") << data << true;
        }
    }

    void checkGLBParsing()
    {
        QFETCH(QByteArray, data);
        QFETCH(bool, success);

        // GIVEN
        GLTF2Context ctx;
        GLTF2Parser parser;

        parser.setContext(&ctx);

        // WHEN
        const bool parsingSuccessful = parser.doParse(data, {});

        // THEN
        QCOMPARE(parsingSuccessful, success);
    }

    void checkThreadedParser()
    {
        // GIVEN
        constexpr int N = 64;
        struct GLTFParsingTester {
            SceneEntity scene;
            GLTF2Context ctx;
            GLTF2Parser parser;
        };

        // WHEN
        std::atomic_int amount_successful{};
        std::atomic_int amount_failed{};
        std::array<GLTFParsingTester, N> testers;
        for (auto &tester : testers) {
            connect(&tester.parser, &GLTF2Parser::gltfFileParsingCompleted,
                    [&](bool parsingSucceeded) {
                        if (parsingSucceeded)
                            ++amount_successful;
                        else
                            ++amount_failed;
                    });
            tester.parser.setContext(&tester.ctx);
            tester.parser.parse(ASSETS "car/DodgeViper.gltf", true);
        }

        // THEN
        while (amount_successful < N && amount_failed == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            qApp->processEvents();
        }
        QCOMPARE(int(amount_failed), 0);
        QCOMPARE(int(amount_successful), 64);
    }
};

QTEST_MAIN(tst_GLTFParser)

#include "tst_gltfparser.moc"
