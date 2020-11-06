/*
    tst_meshparser.cpp

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

#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/private/bufferparser_p.h>
#include <Kuesa/private/gltf2context_p.h>
#include <Kuesa/private/bufferviewsparser_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
#else
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#endif
#include <Qt3DRender/QGeometryRenderer>

using namespace Kuesa;
using namespace GLTF2Import;
using namespace Qt3DGeometry;

class tst_MeshParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParseAndGenerate_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("parsingSucceeded");
        QTest::addColumn<bool>("generationSucceeded");
        QTest::addColumn<int>("meshesCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "simple_cube.gltf")
                               << true
                               << true
                               << 1;

        QTest::newRow("Empty") << QStringLiteral(ASSETS "meshparser_empty.gltf")
                               << false
                               << false
                               << 0;

        QTest::newRow("No_Primitive") << QStringLiteral(ASSETS "meshparser_no_primitive.gltf")
                                      << false
                                      << false
                                      << 0;

        QTest::newRow("Empty_Primitive") << QStringLiteral(ASSETS "meshparser_empty_primitive.gltf")
                                         << false
                                         << false
                                         << 0;

        QTest::newRow("No_Attribute") << QStringLiteral(ASSETS "meshparser_no_attribute.gltf")
                                      << false
                                      << false
                                      << 0;

        QTest::newRow("Valid_MorphTargets") << QStringLiteral(ASSETS "meshparser_valid_morphtargets.gltf")
                                            << true
                                            << true
                                            << 1;

        QTest::newRow("Invalid_MorphTargets_Mistmatch_Count_Primitive") << QStringLiteral(ASSETS "meshparser_morphtargets_mismatch_count_primitive.gltf")
                                                                        << false
                                                                        << false
                                                                        << 0;

        QTest::newRow("Invalid_MorphTargets_Mistmatch_Content_Primitive") << QStringLiteral(ASSETS "meshparser_morphtargets_mismatch_content_primitive.gltf")
                                                                          << false
                                                                          << false
                                                                          << 0;

        QTest::newRow("Invalid_MorphTargets_Mistmatch_Attributes") << QStringLiteral(ASSETS "meshparser_morphtargets_mismatch_attributes.gltf")
                                                                   << false
                                                                   << false
                                                                   << 0;

        QTest::newRow("Invalid_MorphTargets_Invalid_Attribute") << QStringLiteral(ASSETS "meshparser_morphtargets_invalid_attribute.gltf")
                                                                << false
                                                                << false
                                                                << 0;

        QTest::newRow("Invalid_MorphTargets_Invalid_Accessor") << QStringLiteral(ASSETS "meshparser_morphtargets_invalid_accessor.gltf")
                                                               << false
                                                               << false
                                                               << 0;

        QTest::newRow("Invalid_MorphTargets_Attribute_Not_In_Primitive") << QStringLiteral(ASSETS "meshparser_morphtargets_attribute_not_in_primitive.gltf")
                                                                         << true
                                                                         << false
                                                                         << 1;

        QTest::newRow("Invalid_MorphTargets_Default_Weights_Size_Mismatch") << QStringLiteral(ASSETS "meshparser_morphtargets_default_weights_size_mismatch.gltf")
                                                                            << false
                                                                            << false
                                                                            << 0;
    }

    void checkParseAndGenerate()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, parsingSucceeded);
        QFETCH(bool, generationSucceeded);
        QFETCH(int, meshesCount);

        // GIVEN
        GLTF2Context context;

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferViewIndex = 0;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferViewIndex = 0;

        context.addAccessor(normalAccessor);
        context.addAccessor(positionAccessor);
        context.addAccessor(indicesAccessor);

        BufferView view0;
        context.addBufferView(view0);

        MeshParser parser;
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        QJsonParseError e;
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &e);
        const QJsonObject gltfJson = json.object();

        // THEN
        QVERIFY(!json.isNull() && json.isObject());
        QVERIFY(gltfJson.contains(QLatin1String("meshes")));

        // WHEN
        bool success = parser.parse(gltfJson.value(QLatin1String("meshes")).toArray(), &context);

        // THEN
        QCOMPARE(success, parsingSucceeded);
        QCOMPARE(context.meshesCount(), meshesCount);

        if (parsingSucceeded) {
            // WHEN
            PrimitiveBuilder builder(&context);

            for (size_t i = 0; i < size_t(meshesCount); ++i) {
                Mesh &mesh = context.mesh(i);
                for (Primitive &p : mesh.meshPrimitives) {
                    if (!builder.generateGeometryRendererForPrimitive(p)) {
                        success = false;
                        break;
                    }
                }
            }

            // THEN
            QCOMPARE(success, generationSucceeded);
        }
    }

    // The glTF editor needs extra info from GLTF file that's not stored in the Qt3D classes
    void checkForGltfEditorProperties()
    {
        // GIVEN
        GLTF2Context context;

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferViewIndex = 0;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferViewIndex = 0;

        context.addAccessor(normalAccessor);
        context.addAccessor(positionAccessor);
        context.addAccessor(indicesAccessor);

        BufferView view0;
        context.addBufferView(view0);

        MeshParser parser;

        QFile file(QStringLiteral(ASSETS "simple_cube.gltf"));
        file.open(QIODevice::ReadOnly);

        // THEN
        QVERIFY(file.isOpen());

        // WHEN
        QJsonParseError e;
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &e);
        const QJsonObject gltfJson = json.object();
        bool success = parser.parse(gltfJson.value(QLatin1String("meshes")).toArray(), &context);

        // THEN
        QVERIFY(success);

        // WHEN
        for (size_t i = 0; i < size_t(context.meshesCount()); ++i) {
            Mesh &mesh = context.mesh(i);
            for (Primitive &p : mesh.meshPrimitives)
                QVERIFY(context.getOrAllocateGeometryRenderer(p));
        }

        // THEN
        for (size_t meshNo = 0; meshNo < context.meshesCount(); ++meshNo) {
            auto parsedMesh = context.mesh(meshNo);
            QCOMPARE(meshNo, parsedMesh.meshIdx);
            for (auto primitive : parsedMesh.meshPrimitives) {
                auto qMesh = primitive.primitiveRenderer;
                QVERIFY(qMesh);
                for (auto attribute : qMesh->geometry()->attributes()) {
                    QVERIFY(attribute->property("bufferIndex").isValid());
                    QVERIFY(attribute->property("bufferViewIndex").isValid());
                    QVERIFY(attribute->property("bufferViewOffset").isValid());
                    QVERIFY(attribute->property("bufferName").isValid());
                }
            }
        }
    }

    void checkNoBufferSplittingOnSmallAccessorOffsets()
    {
        // GIVEN
        GLTF2Context context;

        QByteArray fakeVertexData(2400, '\0');
        QByteArray fakeIndexData(256, '1');

        BufferView view;
        view.bufferData = fakeVertexData;
        view.byteStride = 24;
        view.byteOffset = 0;

        BufferView view2;
        view2.bufferData = fakeIndexData;
        view2.byteStride = 0;
        view2.byteOffset = 0;

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferData = fakeVertexData;
        positionAccessor.offset = 0;
        positionAccessor.count = 100;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferData = fakeVertexData;
        normalAccessor.offset = 12;
        normalAccessor.count = 100;
        normalAccessor.bufferViewIndex = 0;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferData = fakeIndexData;
        indicesAccessor.offset = 0;
        indicesAccessor.count = 128;
        indicesAccessor.bufferViewIndex = 1;

        context.addBufferView(view);
        context.addBufferView(view2);
        context.addAccessor(positionAccessor);
        context.addAccessor(normalAccessor);
        context.addAccessor(indicesAccessor);

        const QByteArray json = QByteArrayLiteral("["
                                                  "    {"
                                                  "        \"primitives\": [{"
                                                  "             \"attributes\": {"
                                                  "                 \"POSITION\": 0,"
                                                  "                 \"NORMAL\": 1"
                                                  "             },"
                                                  "             \"indices\": 2"
                                                  "        }]"
                                                  "    }"
                                                  "]");
        // THEN
        const QJsonArray meshesArray =  QJsonDocument::fromJson(json).array();
        QVERIFY(!meshesArray.empty());

        // WHEN
        MeshParser parser;
        const bool success = parser.parse(meshesArray, &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.meshesCount(), 1);

        // WHEN

        for (size_t i = 0; i < size_t(context.meshesCount()); ++i) {
            Mesh &mesh = context.mesh(i);
            for (Primitive &p : mesh.meshPrimitives)
                QVERIFY(context.getOrAllocateGeometryRenderer(p));
        }

        // THEN
        const Mesh &mesh = context.mesh(0);
        QCOMPARE(mesh.meshPrimitives.size(), 1);

        const Primitive &p = mesh.meshPrimitives.first();
        QCOMPARE(p.hasColorAttr, false);
        QCOMPARE(p.materialIdx, -1);
        QVERIFY(p.primitiveRenderer != nullptr);

        Qt3DRender::QGeometryRenderer *renderer = p.primitiveRenderer;
        QVERIFY(renderer->geometry() != nullptr);

        Qt3DGeometry::QGeometry *geometry = renderer->geometry();
        QCOMPARE(geometry->attributes().size(), 3);

        auto findAttribute = [&geometry] (const QString &name) -> Qt3DGeometry::QAttribute* {
            const auto &attributes = geometry->attributes();
            auto it = std::find_if(attributes.begin(), attributes.end(),
                         [&name] (const Qt3DGeometry::QAttribute *a) {
                return a->name() == name;
            });
            if (it != attributes.end())
                return *it;
            return nullptr;
        };

        const Qt3DGeometry::QAttribute *positionAttribute = findAttribute(Qt3DGeometry::QAttribute::defaultPositionAttributeName());
        const Qt3DGeometry::QAttribute *normalAttribute = findAttribute(Qt3DGeometry::QAttribute::defaultNormalAttributeName());
        const Qt3DGeometry::QAttribute *indexAttribute = findAttribute({});

        QCOMPARE(positionAttribute->name(), Qt3DGeometry::QAttribute::defaultPositionAttributeName());
        QCOMPARE(positionAttribute->attributeType(), Qt3DGeometry::QAttribute::VertexAttribute);
        QCOMPARE(positionAttribute->vertexBaseType(), Qt3DGeometry::QAttribute::Float);
        QCOMPARE(positionAttribute->vertexSize(), 3U);
        QCOMPARE(positionAttribute->byteStride(), 24U);
        QCOMPARE(positionAttribute->byteOffset(), 0U);
        QCOMPARE(positionAttribute->count(), 100U);

        QCOMPARE(normalAttribute->name(), Qt3DGeometry::QAttribute::defaultNormalAttributeName());
        QCOMPARE(normalAttribute->attributeType(), Qt3DGeometry::QAttribute::VertexAttribute);
        QCOMPARE(normalAttribute->vertexBaseType(), Qt3DGeometry::QAttribute::Float);
        QCOMPARE(normalAttribute->vertexSize(), 3U);
        QCOMPARE(normalAttribute->byteStride(), 24U);
        QCOMPARE(normalAttribute->byteOffset(), 12U);
        QCOMPARE(normalAttribute->count(), 100U);

        QCOMPARE(positionAttribute->buffer(), normalAttribute->buffer());
        QCOMPARE(positionAttribute->buffer()->data(), fakeVertexData);

        QVERIFY(indexAttribute->name().isEmpty());
        QCOMPARE(indexAttribute->attributeType(), Qt3DGeometry::QAttribute::IndexAttribute);
        QCOMPARE(indexAttribute->vertexBaseType(), Qt3DGeometry::QAttribute::UnsignedShort);
        QCOMPARE(indexAttribute->vertexSize(), 1U);
        QCOMPARE(indexAttribute->byteStride(), 0U);
        QCOMPARE(indexAttribute->byteOffset(), 0U);
        QCOMPARE(indexAttribute->buffer()->data(), fakeIndexData);
        QCOMPARE(indexAttribute->count(), 128);

    }

    void checkBufferSplittingForLargetAccessorOffsets()
    {
        GLTF2Context context;

        QByteArray fakeVertexData(4800, '\0');
        QByteArray fakeIndexData(2856, '1');

        BufferView view;
        view.bufferData = fakeVertexData;
        view.byteStride = 0;
        view.byteOffset = 0;

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferData = fakeVertexData;
        positionAccessor.offset = 2400;
        positionAccessor.count = 100;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferData = fakeVertexData;
        normalAccessor.offset = 0;
        normalAccessor.count = 100;
        normalAccessor.bufferViewIndex = 0;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferData = fakeIndexData;
        indicesAccessor.offset = 2500;
        indicesAccessor.count = 128;

        context.addBufferView(view);
        context.addAccessor(positionAccessor);
        context.addAccessor(normalAccessor);
        context.addAccessor(indicesAccessor);

        const QByteArray json = QByteArrayLiteral("["
                                                  "    {"
                                                  "        \"primitives\": [{"
                                                  "             \"attributes\": {"
                                                  "                 \"POSITION\": 0,"
                                                  "                 \"NORMAL\": 1"
                                                  "             },"
                                                  "             \"indices\": 2"
                                                  "        }]"
                                                  "    }"
                                                  "]");
        // THEN
        const QJsonArray meshesArray =  QJsonDocument::fromJson(json).array();
        QVERIFY(!meshesArray.empty());

        // WHEN
        MeshParser parser;
        const bool success = parser.parse(meshesArray, &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.meshesCount(), 1);

        // WHEN
        PrimitiveBuilder builder(&context);

        for (size_t i = 0; i < size_t(context.meshesCount()); ++i) {
            Mesh &mesh = context.mesh(i);
            for (Primitive &p : mesh.meshPrimitives)
                QVERIFY(context.getOrAllocateGeometryRenderer(p));
        }

        // THEN
        const Mesh &mesh = context.mesh(0);
        QCOMPARE(mesh.meshPrimitives.size(), 1);

        const Primitive &p = mesh.meshPrimitives.first();
        QCOMPARE(p.hasColorAttr, false);
        QCOMPARE(p.materialIdx, -1);
        QVERIFY(p.primitiveRenderer != nullptr);

        Qt3DRender::QGeometryRenderer *renderer = p.primitiveRenderer;
        QVERIFY(renderer->geometry() != nullptr);

        Qt3DGeometry::QGeometry *geometry = renderer->geometry();
        QCOMPARE(geometry->attributes().size(), 3);

        auto findAttribute = [&geometry] (const QString &name) -> Qt3DGeometry::QAttribute* {
            const auto &attributes = geometry->attributes();
            auto it = std::find_if(attributes.begin(), attributes.end(),
                         [&name] (const Qt3DGeometry::QAttribute *a) {
                return a->name() == name;
            });
            if (it != attributes.end())
                return *it;
            return nullptr;
        };

        const Qt3DGeometry::QAttribute *positionAttribute = findAttribute(Qt3DGeometry::QAttribute::defaultPositionAttributeName());
        const Qt3DGeometry::QAttribute *normalAttribute = findAttribute(Qt3DGeometry::QAttribute::defaultNormalAttributeName());
        const Qt3DGeometry::QAttribute *indexAttribute = findAttribute({});

        QCOMPARE(positionAttribute->name(), Qt3DGeometry::QAttribute::defaultPositionAttributeName());
        QCOMPARE(positionAttribute->attributeType(), Qt3DGeometry::QAttribute::VertexAttribute);
        QCOMPARE(positionAttribute->vertexBaseType(), Qt3DGeometry::QAttribute::Float);
        QCOMPARE(positionAttribute->vertexSize(), 3U);
        QCOMPARE(positionAttribute->byteStride(), 0U);
        QCOMPARE(positionAttribute->byteOffset(), 0U);
        QCOMPARE(positionAttribute->count(), 100U);

        QCOMPARE(normalAttribute->name(), Qt3DGeometry::QAttribute::defaultNormalAttributeName());
        QCOMPARE(normalAttribute->attributeType(), Qt3DGeometry::QAttribute::VertexAttribute);
        QCOMPARE(normalAttribute->vertexBaseType(), Qt3DGeometry::QAttribute::Float);
        QCOMPARE(normalAttribute->vertexSize(), 3U);
        QCOMPARE(normalAttribute->byteStride(), 0U);
        QCOMPARE(normalAttribute->byteOffset(), 0U);
        QCOMPARE(normalAttribute->count(), 100U);

        QVERIFY(positionAttribute->buffer() != normalAttribute->buffer());

        QCOMPARE(normalAttribute->buffer()->data(), fakeVertexData);
        QCOMPARE(normalAttribute->buffer()->data().size(), 4800);

        QCOMPARE(positionAttribute->buffer()->data().size(), 2400);
        QCOMPARE(positionAttribute->buffer()->data(), fakeVertexData.mid(2400));

        QVERIFY(indexAttribute->name().isEmpty());
        QCOMPARE(indexAttribute->attributeType(), Qt3DGeometry::QAttribute::IndexAttribute);
        QCOMPARE(indexAttribute->vertexBaseType(), Qt3DGeometry::QAttribute::UnsignedShort);
        QCOMPARE(indexAttribute->vertexSize(), 1U);
        QCOMPARE(indexAttribute->byteStride(), 0U);
        QCOMPARE(indexAttribute->byteOffset(), 0U);
        QCOMPARE(indexAttribute->buffer()->data(), fakeIndexData.mid(2500));
        QCOMPARE(indexAttribute->buffer()->data().size(), 356);
        QCOMPARE(indexAttribute->count(), 128);
    }

    void checkPrimitiveNonSharingWithDifferentKeys()
    {
        // GIVEN
        Kuesa::SceneEntity s;
        GLTF2Context context;
        context.reset(&s);

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferViewIndex = 0;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferViewIndex = 0;

        context.addAccessor(normalAccessor);
        context.addAccessor(positionAccessor);
        context.addAccessor(indicesAccessor);

        BufferView view0;
        context.addBufferView(view0);

        MeshParser parser;
        QFile file(ASSETS "simple_cube_primitive_no_sharing.gltf");
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        QJsonParseError e;
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &e);
        const QJsonObject gltfJson = json.object();

        // THEN
        QVERIFY(!json.isNull() && json.isObject());
        QVERIFY(gltfJson.contains(QLatin1String("meshes")));

        // WHEN
        bool success = parser.parse(gltfJson.value(QLatin1String("meshes")).toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.meshesCount(), 1);

        // WHEN
        Mesh &mesh = context.mesh(0);

        // THEN
        QCOMPARE(mesh.meshPrimitives.size(), 2);

        for (Primitive &p : mesh.meshPrimitives) {
            if (!context.getOrAllocateGeometryRenderer(p)) {
                success = false;
                break;
            }
        }

        // THEN
        const Primitive &p1 = mesh.meshPrimitives[0];
        const Primitive &p2 = mesh.meshPrimitives[1];
        QVERIFY(p1.primitiveRenderer);
        QVERIFY(p2.primitiveRenderer);
        QVERIFY(p1.primitiveRenderer != p2.primitiveRenderer);
    }

    void checkPrimitiveSharingWithSameKeys()
    {
        // GIVEN
        Kuesa::SceneEntity s;
        GLTF2Context context;
        context.reset(&s);

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferViewIndex = 0;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferViewIndex = 0;

        context.addAccessor(normalAccessor);
        context.addAccessor(positionAccessor);
        context.addAccessor(indicesAccessor);

        BufferView view0;
        context.addBufferView(view0);

        MeshParser parser;
        QFile file(ASSETS "simple_cube_primitive_sharing.gltf");
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        QJsonParseError e;
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &e);
        const QJsonObject gltfJson = json.object();

        // THEN
        QVERIFY(!json.isNull() && json.isObject());
        QVERIFY(gltfJson.contains(QLatin1String("meshes")));

        // WHEN
        bool success = parser.parse(gltfJson.value(QLatin1String("meshes")).toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.meshesCount(), 1);

        // WHEN
        Mesh &mesh = context.mesh(0);

        // THEN
        QCOMPARE(mesh.meshPrimitives.size(), 2);

        for (Primitive &p : mesh.meshPrimitives) {
            if (!context.getOrAllocateGeometryRenderer(p)) {
                success = false;
                break;
            }
        }

        // THEN
        const Primitive &p1 = mesh.meshPrimitives[0];
        const Primitive &p2 = mesh.meshPrimitives[1];
        QVERIFY(p1.primitiveRenderer);
        QVERIFY(p2.primitiveRenderer);
        QCOMPARE(p1.primitiveRenderer, p2.primitiveRenderer);
    }

    void checkBufferSharingWithSameKeys()
    {
        // GIVEN
        Kuesa::SceneEntity s;
        GLTF2Context context;
        context.reset(&s);

        QByteArray fakeVertexData(4800, '\0');
        QByteArray fakeIndexData(2856, '1');

        BufferView view;
        view.bufferData = fakeVertexData;
        view.byteStride = 0;
        view.byteOffset = 0;
        view.key = QStringLiteral("buffer_key_1");

        BufferView view2;
        view2.bufferData = fakeVertexData;
        view2.byteStride = 0;
        view2.byteOffset = 0;
        view2.key = QStringLiteral("buffer_key_1");

        BufferView view3;
        view3.bufferData = fakeIndexData;
        view3.byteStride = 0;
        view3.byteOffset = 0;

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferViewIndex = 1;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferViewIndex = 2;

        context.addBufferView(view);
        context.addBufferView(view2);
        context.addBufferView(view3);
        context.addAccessor(normalAccessor);
        context.addAccessor(positionAccessor);
        context.addAccessor(indicesAccessor);

        MeshParser parser;
        QFile file(ASSETS "simple_cube.gltf");
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        QJsonParseError e;
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &e);
        const QJsonObject gltfJson = json.object();

        // THEN
        QVERIFY(!json.isNull() && json.isObject());
        QVERIFY(gltfJson.contains(QLatin1String("meshes")));

        // WHEN
        bool success = parser.parse(gltfJson.value(QLatin1String("meshes")).toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.meshesCount(), 1);

        // WHEN
        Mesh &mesh = context.mesh(0);

        // THEN
        QCOMPARE(mesh.meshPrimitives.size(), 1);

        // WHEN
        Primitive &p1 = mesh.meshPrimitives[0];
        QVERIFY(context.getOrAllocateGeometryRenderer(p1));

        // THEN
        QVERIFY(p1.primitiveRenderer);

        const QVector<QAttribute *> attributes = p1.primitiveRenderer->geometry()->attributes();
        QCOMPARE(attributes.size(), 3);

        auto findAttributeWithName = [&] (const QString &name) {
            return *std::find_if(attributes.begin(), attributes.end(), [&] (Qt3DGeometry::QAttribute *a) {
                return a->name() == name;
            });
        };

        auto normalAttribute = findAttributeWithName(Qt3DGeometry::QAttribute::defaultNormalAttributeName());
        auto positionAttribute = findAttributeWithName(Qt3DGeometry::QAttribute::defaultPositionAttributeName());

        QVERIFY(normalAttribute);
        QVERIFY(positionAttribute);
        QVERIFY(positionAttribute != normalAttribute);

        QVERIFY(normalAttribute->buffer());
        QVERIFY(positionAttribute->buffer());
        QCOMPARE(positionAttribute->buffer(), normalAttribute->buffer());
    }

    void checkBufferNoSharing()
    {
        // GIVEN
        Kuesa::SceneEntity s;
        GLTF2Context context;
        context.reset(&s);

        QByteArray fakeVertexData(4800, '\0');
        QByteArray fakeIndexData(2856, '1');

        BufferView view;
        view.bufferData = fakeVertexData;
        view.byteStride = 0;
        view.byteOffset = 0;

        BufferView view2;
        view2.bufferData = fakeVertexData;
        view2.byteStride = 0;
        view2.byteOffset = 0;

        BufferView view3;
        view3.bufferData = fakeIndexData;
        view3.byteStride = 0;
        view3.byteOffset = 0;

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;
        positionAccessor.bufferViewIndex = 0;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;
        normalAccessor.bufferViewIndex = 1;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = QAttribute::VertexBaseType::UnsignedShort;
        indicesAccessor.bufferViewIndex = 2;

        context.addBufferView(view);
        context.addBufferView(view2);
        context.addBufferView(view3);
        context.addAccessor(normalAccessor);
        context.addAccessor(positionAccessor);
        context.addAccessor(indicesAccessor);

        MeshParser parser;
        QFile file(ASSETS "simple_cube.gltf");
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        QJsonParseError e;
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &e);
        const QJsonObject gltfJson = json.object();

        // THEN
        QVERIFY(!json.isNull() && json.isObject());
        QVERIFY(gltfJson.contains(QLatin1String("meshes")));

        // WHEN
        bool success = parser.parse(gltfJson.value(QLatin1String("meshes")).toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.meshesCount(), 1);

        // WHEN
        Mesh &mesh = context.mesh(0);

        // THEN
        QCOMPARE(mesh.meshPrimitives.size(), 1);

        // WHEN
        Primitive &p1 = mesh.meshPrimitives[0];
        QVERIFY(context.getOrAllocateGeometryRenderer(p1));

        // THEN
        QVERIFY(p1.primitiveRenderer);

        const QVector<QAttribute *> attributes = p1.primitiveRenderer->geometry()->attributes();
        QCOMPARE(attributes.size(), 3);

        auto findAttributeWithName = [&] (const QString &name) {
            return *std::find_if(attributes.begin(), attributes.end(), [&] (Qt3DGeometry::QAttribute *a) {
                return a->name() == name;
            });
        };

        auto normalAttribute = findAttributeWithName(Qt3DGeometry::QAttribute::defaultNormalAttributeName());
        auto positionAttribute = findAttributeWithName(Qt3DGeometry::QAttribute::defaultPositionAttributeName());

        QVERIFY(normalAttribute);
        QVERIFY(positionAttribute);
        QVERIFY(positionAttribute != normalAttribute);

        QVERIFY(normalAttribute->buffer());
        QVERIFY(positionAttribute->buffer());
        QVERIFY(positionAttribute->buffer() != normalAttribute->buffer());
    }
};

QTEST_GUILESS_MAIN(tst_MeshParser)

#include "tst_meshparser.moc"
