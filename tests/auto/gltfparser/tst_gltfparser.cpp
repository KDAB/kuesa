/*
    tst_gltfparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/SceneEntity>
#include <Kuesa/GLTF2Context>
#include <Kuesa/private/gltf2parser_p.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>
#include <Kuesa/MetallicRoughnessMaterial>
#include <Qt3DCore/QSkeleton>
#include <Qt3DCore/QJoint>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QCamera>
#include <Kuesa/LayerCollection>
#include <Kuesa/private/kuesa_utils_p.h>

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

        GLTF2ContextPrivate ctx;
        GLTF2Parser parser;

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "GeneratedAssets/") + filePath);

        // THEN
        QVERIFY(res);

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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube.gltf"));

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.meshes()->names().size(), 1);
        QVERIFY(scene.mesh(QLatin1String("Cube_0")));
    }

    void checkCameraCollection()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube.gltf"));

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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube_with_layers.gltf"));

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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube.gltf"));

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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube_same_names.gltf"));

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

        GLTF2ContextPrivate ctx;
        GLTF2ParserNoTextures parser;

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS + filePath));

        // THEN
        QCOMPARE(res != nullptr, parsingSucceeded);
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

        GLTF2ContextPrivate ctx;
        GLTF2ParserNoTextures parser;

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS + filePath));

        // THEN
        QCOMPARE(res != nullptr, parsingSucceeded);
        QCOMPARE(parser.context()->textureSamplersCount(), samplerCount);
        for (int i = 0; i < samplerCount; ++i) {
            const TextureSampler s = parser.context()->textureSampler(i);
            QCOMPARE(s.minificationFilter, minFilter);
            QCOMPARE(s.magnificationFilter, magFilter);
            QCOMPARE(s.textureWrapMode->x(), wrapS);
            QCOMPARE(s.textureWrapMode->y(), wrapT);
        }
    }

    void checkTextureImageCollection()
    {
        SceneEntity scene;

        GLTF2ContextPrivate ctx;
        GLTF2ParserNoTextures parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube_with_images.gtlf"));

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

        GLTF2ContextPrivate ctx;
        GLTF2Parser parser;

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS + filePath));

        // THEN
        QCOMPARE(res != nullptr, parsingSucceeded);
        QCOMPARE(parser.context()->texturesCount(), textureCount);
    }

    void checkDDSTextureExtension()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube_with_textures_dds.gltf"));

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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube_with_textures.gltf"));

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.textures()->names().size(), 2);
        QVERIFY(scene.texture(QLatin1String("diffuse")));
        QVERIFY(scene.texture(QLatin1String("normal")));
    }

    void checkAnimationClipCollection()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "animated_cube_lot_rot_scale.gltf"));

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.animationClips()->names().size(), 1);
        QVERIFY(scene.animationClip(QStringLiteral("Cube_CubeAction")));
    }

    void checkAnimationMappingCollection()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "animated_cube_lot_rot_scale.gltf"));

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

        GLTF2ContextPrivate ctx;
        GLTF2Parser parser;

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS + filePath));

        // THEN
        QCOMPARE(res != nullptr, parsingSucceeded);
        QCOMPARE(parser.context()->materialsCount(), materialCount);
    }

    void checkMaterialCollection()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "simple_cube_with_textures.gltf"));

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.textures()->names().size(), 2);
        QCOMPARE(scene.materials()->names().size(), 1);

        Kuesa::MetallicRoughnessMaterial *mat = static_cast<decltype(mat)>(scene.material(QStringLiteral("Material")));
        QVERIFY(mat != nullptr);
        QCOMPARE(mat->isDoubleSided(), false);
        QCOMPARE(mat->metallicFactor(), 0.0f);
        QCOMPARE(mat->roughnessFactor(), 1.0f);
        QCOMPARE(mat->emissiveFactor(), QColor::fromRgbF(0.0, 0.0, 0.0));
        QCOMPARE(mat->baseColorFactor(), QColor::fromRgbF(0.15, 0.64, 0.04, 1.0));
    }

    void checkSkeletonCollection()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS "skins_valid.gltf"));

        // THEN
        QVERIFY(res);
        QCOMPARE(scene.skeletons()->names().size(), 1);
        QVERIFY(scene.skeleton(QStringLiteral("Armature")));
    }

    void checkJointsHierarchy_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<QVector<QString>>("expectedNames");

        QVector<QString> expectedNames = { "Armature",
                                           "Armature_Hip",
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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene, true);

        parser.setContext(&ctx);

        // WHEN
        Qt3DCore::QEntity *res = parser.parse(QString(ASSETS + filePath));

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
        GLTF2ContextPrivate ctx;
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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        auto res = parser.parse(QString(ASSETS "extensions_invalid.gltf"));

        // THEN
        QCOMPARE(res, nullptr);
        QCOMPARE(parser.context()->requiredExtensions().size(), 1);
        QCOMPARE(parser.context()->requiredExtensions().front(), QLatin1String("KDAB_Some_Unknown_Extension"));
    }

    void checkExtensionsValid()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        auto res = parser.parse(QString(ASSETS "extensions_valid.gltf"));

        // THEN
        QVERIFY(res != nullptr);
        QCOMPARE(parser.context()->requiredExtensions().size(), 1);
        QCOMPARE(parser.context()->usedExtension().size(), 1);
        QCOMPARE(parser.context()->usedExtension().front(), QLatin1String("KDAB_Kuesa_Layers"));
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
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        auto res = parser.parse(filePath);

        // THEN
        QVERIFY(res != nullptr);

        QCOMPARE(expectedLayersName.size(), scene.layers()->size());
        if (scene.layers()->size()) {
            for (const auto &layerName : qAsConst(expectedLayersName)) {
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

#if defined(KUESA_DRACO_COMPRESSION)
    void checkDracoCompression()
    {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(GLTF2Import::GLTF2ContextPrivate::get(&ctx));

        // WHEN
        auto res = parser.parse(QString(ASSETS "draco/Box.gltf"));

        // THEN
        QVERIFY(res != nullptr);
    }
#endif

    void checkUseDracoCompressionIfAvailable()
    {
        SceneEntity scene;
        GLTF2ContextPrivate ctx;
        GLTF2Parser parser(&scene);

        parser.setContext(&ctx);

        // WHEN
        auto res = parser.parse(QString(ASSETS "draco/Box_optional.gltf"));

        // THEN
        QVERIFY(res != nullptr);
    }
};

QTEST_APPLESS_MAIN(tst_GLTFParser)

#include "tst_gltfparser.moc"
