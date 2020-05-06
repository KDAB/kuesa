/*
    tst_meshparser_utils.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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
#include <Kuesa/private/meshparser_utils_p.h>
#include <Kuesa/private/kuesa_global_p.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QAttribute>
#else
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#endif
#include <cstring>

#include <iostream>

using namespace Kuesa;
using namespace GLTF2Import;
using namespace Qt3DRender;
using namespace Qt3DGeometry;

class tst_MeshParserUtils : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkNeedsTangentAttribute()
    {
        // GIVEN
        QScopedPointer<QGeometry> geo(new QGeometry());

        // WHEN
        bool needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                                    QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsTangents, false);

        // GIVEN
        QAttribute *posAttr = new QAttribute();
        posAttr->setName(QAttribute::defaultPositionAttributeName());
        QAttribute *normalAttr = new QAttribute();
        normalAttr->setName(QAttribute::defaultNormalAttributeName());
        QAttribute *uvAttr = new QAttribute();
        uvAttr->setName(QAttribute::defaultTextureCoordinateAttributeName());
        geo->addAttribute(posAttr);
        geo->addAttribute(normalAttr);
        geo->addAttribute(uvAttr);

        // WHEN
        needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                               QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsTangents, true);

        // WHEN
        needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                               QGeometryRenderer::Lines);

        // THEN
        QCOMPARE(needsTangents, false);

        // WHEN
        QAttribute *tangentAttr = new QAttribute();
        tangentAttr->setName(QAttribute::defaultTangentAttributeName());
        geo->addAttribute(tangentAttr);
        needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                               QGeometryRenderer::Triangles);
        QCOMPARE(needsTangents, false);
    }

    void checkNeedsNormalAttribute()
    {
        // GIVEN
        QScopedPointer<QGeometry> geo(new QGeometry());

        // WHEN
        bool needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                                  QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsNormals, false);

        // WHEN
        QAttribute *posAttr = new QAttribute();
        posAttr->setName(QAttribute::defaultPositionAttributeName());
        geo->addAttribute(posAttr);
        needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                             QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsNormals, true);

        // WHEN
        needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                             QGeometryRenderer::Lines);

        // THEN
        QCOMPARE(needsNormals, false);

        // WHEN
        QAttribute *normalsAttr = new QAttribute();
        normalsAttr->setName(QAttribute::defaultNormalAttributeName());
        geo->addAttribute(normalsAttr);
        needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                             QGeometryRenderer::Triangles);
        QCOMPARE(needsNormals, false);
    }

    void checkNormalGeneration_data()
    {
        QTest::addColumn<QGeometry *>("geometry");
        QTest::addColumn<QGeometryRenderer::PrimitiveType>("primitiveType");
        QTest::addColumn<QVector<QVector3D>>("expectedNormals");

        {
            QGeometry *geometry = new QGeometry();

            QVector<QVector3D> positions;
            QVector<QVector3D> normals;

            positions.reserve(6);
            normals.reserve(6);

            const QVector3D a(-1.0f, 1.0, 0.0f);
            const QVector3D b(1.0f, 1.0, 0.0f);
            const QVector3D c(-1.0f, -1.0, 0.0f);
            const QVector3D d(1.0f, -1.0, 0.0f);

            positions << a << c << b
                      << c << d << b;

            const QVector3D n(0.0f, 0.0f, 1.0f);
            normals << n << n << n
                    << n << n << n;

            QByteArray rawData;
            rawData.resize(positions.size() * sizeof(QVector3D));
            std::memcpy(rawData.data(), positions.data(), positions.size() * sizeof(QVector3D));

            Qt3DGeometry::QBuffer *buffer = new Qt3DGeometry::QBuffer();
            buffer->setData(rawData);

            QAttribute *posAttribute = new QAttribute();
            posAttribute->setName(QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(buffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(QAttribute::VertexAttribute);
            posAttribute->setCount(6);

            geometry->addAttribute(posAttribute);

            QTest::newRow("NonIndexedTriangles") << geometry
                                                 << QGeometryRenderer::Triangles
                                                 << normals;
        }
        {
            QGeometry *geometry = new QGeometry();

            QVector<QVector3D> positions;
            QVector<QVector3D> normals;

            positions.reserve(4);
            normals.reserve(6);

            const QVector3D a(-1.0f, 1.0, 0.0f);
            const QVector3D b(1.0f, 1.0, 0.0f);
            const QVector3D c(-1.0f, -1.0, 0.0f);
            const QVector3D d(1.0f, -1.0, 0.0f);

            positions << a << c << b << d;

            const QVector3D n(0.0f, 0.0f, 1.0f);
            normals << n << n << n
                    << n << n << n;

            QByteArray rawData;
            rawData.resize(positions.size() * sizeof(QVector3D));
            std::memcpy(rawData.data(), positions.data(), positions.size() * sizeof(QVector3D));

            Qt3DGeometry::QBuffer *buffer = new Qt3DGeometry::QBuffer();
            buffer->setData(rawData);

            QAttribute *posAttribute = new QAttribute();
            posAttribute->setName(QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(buffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            geometry->addAttribute(posAttribute);

            QTest::newRow("NonIndexedTriangleStrip") << geometry
                                                     << QGeometryRenderer::TriangleStrip
                                                     << normals;
        }
        {
            QGeometry *geometry = new QGeometry();

            QVector<QVector3D> positions;
            QVector<QVector3D> normals;

            positions.reserve(4);
            normals.reserve(6);

            const QVector3D a(-1.0f, 1.0, 0.0f);
            const QVector3D b(1.0f, 1.0, 0.0f);
            const QVector3D c(-1.0f, -1.0, 0.0f);
            const QVector3D d(1.0f, -1.0, 0.0f);

            positions << c << b << a << d;

            const QVector3D n(0.0f, 0.0f, 1.0f);
            normals << n << n << n
                    << n << n << n;

            QByteArray rawData;
            rawData.resize(positions.size() * sizeof(QVector3D));
            std::memcpy(rawData.data(), positions.data(), positions.size() * sizeof(QVector3D));

            Qt3DGeometry::QBuffer *buffer = new Qt3DGeometry::QBuffer();
            buffer->setData(rawData);

            QAttribute *posAttribute = new QAttribute();
            posAttribute->setName(QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(buffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            geometry->addAttribute(posAttribute);

            QTest::newRow("NonIndexedTriangleFan") << geometry
                                                   << QGeometryRenderer::TriangleFan
                                                   << normals;
        }
        {
            QGeometry *geometry = new QGeometry();

            QVector<QVector3D> positions;
            QVector<ushort> indices;
            QVector<QVector3D> normals;

            positions.reserve(4);
            indices.reserve(6);
            normals.reserve(6);

            const QVector3D a(-1.0f, 1.0, 0.0f);
            const QVector3D b(1.0f, 1.0, 0.0f);
            const QVector3D c(-1.0f, -1.0, 0.0f);
            const QVector3D d(1.0f, -1.0, 0.0f);

            positions << a << b << c << d;

            indices << 0 << 2 << 1
                    << 2 << 3 << 1;

            const QVector3D n(0.0f, 0.0f, 1.0f);
            normals << n << n << n
                    << n << n << n;

            QByteArray rawPositionData;
            rawPositionData.resize(positions.size() * sizeof(QVector3D));
            std::memcpy(rawPositionData.data(), positions.data(), positions.size() * sizeof(QVector3D));

            QByteArray rawIndexData;
            rawIndexData.resize(indices.size() * sizeof(indices[0]));
            std::memcpy(rawIndexData.data(), indices.data(), indices.size() * sizeof(indices[0]));

            Qt3DGeometry::QBuffer *vertexBuffer = new Qt3DGeometry::QBuffer();
            vertexBuffer->setData(rawPositionData);

            Qt3DGeometry::QBuffer *indexBuffer = new Qt3DGeometry::QBuffer();
            indexBuffer->setData(rawIndexData);

            QAttribute *posAttribute = new QAttribute();
            posAttribute->setName(QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(vertexBuffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            QAttribute *indexAttribute = new QAttribute();
            indexAttribute->setBuffer(indexBuffer);
            indexAttribute->setByteOffset(0);
            indexAttribute->setByteStride(0);
            indexAttribute->setVertexBaseType(QAttribute::UnsignedShort);
            indexAttribute->setVertexSize(1);
            indexAttribute->setAttributeType(QAttribute::IndexAttribute);
            indexAttribute->setCount(6);

            geometry->addAttribute(indexAttribute);
            geometry->addAttribute(posAttribute);

            QTest::newRow("IndexedTriangles") << geometry
                                              << QGeometryRenderer::Triangles
                                              << normals;
        }
        {
            QGeometry *geometry = new QGeometry();

            QVector<QVector3D> positions;
            QVector<ushort> indices;
            QVector<QVector3D> normals;

            positions.reserve(4);
            indices.reserve(4);
            normals.reserve(6);

            const QVector3D a(-1.0f, 1.0, 0.0f);
            const QVector3D b(1.0f, 1.0, 0.0f);
            const QVector3D c(-1.0f, -1.0, 0.0f);
            const QVector3D d(1.0f, -1.0, 0.0f);

            positions << a << b << c << d;

            indices << 0 << 2 << 1 << 3;

            const QVector3D n(0.0f, 0.0f, 1.0f);
            normals << n << n << n
                    << n << n << n;

            QByteArray rawPositionData;
            rawPositionData.resize(positions.size() * sizeof(QVector3D));
            std::memcpy(rawPositionData.data(), positions.data(), positions.size() * sizeof(QVector3D));

            QByteArray rawIndexData;
            rawIndexData.resize(indices.size() * sizeof(indices[0]));
            std::memcpy(rawIndexData.data(), indices.data(), indices.size() * sizeof(indices[0]));

            Qt3DGeometry::QBuffer *vertexBuffer = new Qt3DGeometry::QBuffer();
            vertexBuffer->setData(rawPositionData);

            Qt3DGeometry::QBuffer *indexBuffer = new Qt3DGeometry::QBuffer();
            indexBuffer->setData(rawIndexData);

            QAttribute *posAttribute = new QAttribute();
            posAttribute->setName(QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(vertexBuffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            QAttribute *indexAttribute = new QAttribute();
            indexAttribute->setBuffer(indexBuffer);
            indexAttribute->setByteOffset(0);
            indexAttribute->setByteStride(0);
            indexAttribute->setVertexBaseType(QAttribute::UnsignedShort);
            indexAttribute->setVertexSize(1);
            indexAttribute->setAttributeType(QAttribute::IndexAttribute);
            indexAttribute->setCount(4);

            geometry->addAttribute(indexAttribute);
            geometry->addAttribute(posAttribute);

            QTest::newRow("IndexedTriangleStrip") << geometry
                                                  << QGeometryRenderer::TriangleStrip
                                                  << normals;
        }
        {
            QGeometry *geometry = new QGeometry();

            QVector<QVector3D> positions;
            QVector<ushort> indices;
            QVector<QVector3D> normals;

            positions.reserve(4);
            indices.reserve(4);
            normals.reserve(6);

            const QVector3D a(-1.0f, 1.0, 0.0f);
            const QVector3D b(1.0f, 1.0, 0.0f);
            const QVector3D c(-1.0f, -1.0, 0.0f);
            const QVector3D d(1.0f, -1.0, 0.0f);

            positions << a << b << c << d;

            indices << 2 << 1 << 0 << 3;

            const QVector3D n(0.0f, 0.0f, 1.0f);
            normals << n << n << n
                    << n << n << n;

            QByteArray rawPositionData;
            rawPositionData.resize(positions.size() * sizeof(QVector3D));
            std::memcpy(rawPositionData.data(), positions.data(), positions.size() * sizeof(QVector3D));

            QByteArray rawIndexData;
            rawIndexData.resize(indices.size() * sizeof(indices[0]));
            std::memcpy(rawIndexData.data(), indices.data(), indices.size() * sizeof(indices[0]));

            Qt3DGeometry::QBuffer *vertexBuffer = new Qt3DGeometry::QBuffer();
            vertexBuffer->setData(rawPositionData);

            Qt3DGeometry::QBuffer *indexBuffer = new Qt3DGeometry::QBuffer();
            indexBuffer->setData(rawIndexData);

            QAttribute *posAttribute = new QAttribute();
            posAttribute->setName(QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(vertexBuffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            QAttribute *indexAttribute = new QAttribute();
            indexAttribute->setBuffer(indexBuffer);
            indexAttribute->setByteOffset(0);
            indexAttribute->setByteStride(0);
            indexAttribute->setVertexBaseType(QAttribute::UnsignedShort);
            indexAttribute->setVertexSize(1);
            indexAttribute->setAttributeType(QAttribute::IndexAttribute);
            indexAttribute->setCount(4);

            geometry->addAttribute(indexAttribute);
            geometry->addAttribute(posAttribute);

            QTest::newRow("IndexedTriangleFan") << geometry
                                                << QGeometryRenderer::TriangleFan
                                                << normals;
        }
    }

    void checkNormalGeneration()
    {
        // GIVEN
        QFETCH(QGeometry *, geometry);
        QFETCH(QGeometryRenderer::PrimitiveType, primitiveType);
        QFETCH(QVector<QVector3D>, expectedNormals);

        // THEN
        QVERIFY(MeshParserUtils::needsNormalAttribute(geometry, primitiveType));

        // WHEN
        MeshParserUtils::createNormalsForGeometry(geometry, primitiveType);

        // THEN
        // Check we have no IndexAttribute
        const auto attributes = geometry->attributes();
        QVERIFY(std::find_if(std::begin(attributes), std::end(attributes),
                             [](QAttribute *attr) {
                                 return attr->attributeType() == QAttribute::IndexAttribute;
                             }) == std::end(attributes));

        QVERIFY(!MeshParserUtils::needsNormalAttribute(geometry, QGeometryRenderer::Triangles));

        auto normalAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                         [](QAttribute *attr) {
                                             return attr->name() == QAttribute::defaultNormalAttributeName();
                                         });
        QVERIFY(normalAttrIt != std::end(attributes));
        QAttribute *normalAttribute = *normalAttrIt;
        Qt3DGeometry::QBuffer *normalBuffer = normalAttribute->buffer();
        const QByteArray rawData = normalBuffer->data();
        const QVector3D *normals = reinterpret_cast<const QVector3D *>(rawData.constData());

        for (const QVector3D &expectedNormal : expectedNormals) {
            QCOMPARE(*normals, expectedNormal);
            ++normals;
        }
    }

    void checkNormalMorphTargetGeneration()
    {
        // GIVEN
        QGeometry *geometry = new QGeometry();

        QVector<QVector3D> positions;
        QVector<QVector3D> morphPositions;

        positions.reserve(6);
        morphPositions.reserve(6);

        QVector3D a(-1.0f, 1.0, 0.0f);
        QVector3D b(1.0f, 1.0, 0.0f);
        QVector3D c(-1.0f, -1.0, 0.0f);
        QVector3D d(1.0f, -1.0, 0.0f);
        positions << a << b << c
                  << c << b << d;

        a = QVector3D(0, 0, 0);
        b = QVector3D(0, 0, 0);
        c = QVector3D(0, 0, 0);
        d = QVector3D(0, 0, 1.0f);
        morphPositions << a << b << c
                       << c << b << d;

        QByteArray rawPositionData;
        rawPositionData.resize(positions.size() * sizeof(QVector3D));
        std::memcpy(rawPositionData.data(), positions.data(), positions.size() * sizeof(QVector3D));

        QByteArray rawMorphPositionData;
        rawMorphPositionData.resize(morphPositions.size() * sizeof(QVector3D));
        std::memcpy(rawMorphPositionData.data(), morphPositions.data(), morphPositions.size() * sizeof(QVector3D));

        Qt3DGeometry::QBuffer *vertexBuffer = new Qt3DGeometry::QBuffer();
        vertexBuffer->setData(rawPositionData);

        Qt3DGeometry::QBuffer *morphVertexBuffer = new Qt3DGeometry::QBuffer();
        morphVertexBuffer->setData(rawMorphPositionData);

        QAttribute *posAttribute = new QAttribute();
        posAttribute->setName(QAttribute::defaultPositionAttributeName());
        posAttribute->setBuffer(vertexBuffer);
        posAttribute->setByteOffset(0);
        posAttribute->setByteStride(0);
        posAttribute->setVertexBaseType(QAttribute::Float);
        posAttribute->setVertexSize(3);
        posAttribute->setAttributeType(QAttribute::VertexAttribute);
        posAttribute->setCount(6);

        QAttribute *posMorphAttribute = new QAttribute();
        posMorphAttribute->setName(QAttribute::defaultPositionAttributeName() + QStringLiteral("_1"));
        posMorphAttribute->setBuffer(morphVertexBuffer);
        posMorphAttribute->setByteOffset(0);
        posMorphAttribute->setByteStride(0);
        posMorphAttribute->setVertexBaseType(QAttribute::Float);
        posMorphAttribute->setVertexSize(3);
        posMorphAttribute->setAttributeType(QAttribute::VertexAttribute);
        posMorphAttribute->setCount(6);

        geometry->addAttribute(posAttribute);
        geometry->addAttribute(posMorphAttribute);

        // THEN
        QVERIFY(MeshParserUtils::needsNormalAttribute(geometry, QGeometryRenderer::Triangles));

        // WHEN
        MeshParserUtils::createNormalsForGeometry(geometry, QGeometryRenderer::Triangles);

        // THEN
        QVERIFY(!MeshParserUtils::needsNormalAttribute(geometry, QGeometryRenderer::Triangles));

        const auto attributes = geometry->attributes();
        auto normalMorphAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                              [](QAttribute *attr) {
                                                  return attr->name() == QAttribute::defaultNormalAttributeName() + QStringLiteral("_1");
                                              });

        auto normalAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                         [](QAttribute *attr) {
                                             return attr->name() == QAttribute::defaultNormalAttributeName();
                                         });
        QVERIFY(normalAttrIt != std::end(attributes));
        QVERIFY(normalMorphAttrIt != std::end(attributes));

        QAttribute *normalAttribute = *normalAttrIt;
        Qt3DGeometry::QBuffer *normalBuffer = normalAttribute->buffer();
        const QByteArray rawData = normalBuffer->data();
        const QVector3D *normals = reinterpret_cast<const QVector3D *>(rawData.constData());

        QAttribute *normalMorphAttribute = *normalMorphAttrIt;
        Qt3DGeometry::QBuffer *normalMorphBuffer = normalMorphAttribute->buffer();
        const QByteArray rawMorphData = normalMorphBuffer->data();
        const QVector3D *morphNormals = reinterpret_cast<const QVector3D *>(rawMorphData.constData());

        QCOMPARE(normals[0] + morphNormals[0], QVector3D(0, 0, -1));
        QCOMPARE(normals[1] + morphNormals[1], QVector3D(0, 0, -1));
        QCOMPARE(normals[2] + morphNormals[2], QVector3D(0, 0, -1));

        QCOMPARE(normals[3] + morphNormals[3], QVector3D(2, -2, -4).normalized());
        QCOMPARE(normals[4] + morphNormals[4], QVector3D(2, -2, -4).normalized());
        QCOMPARE(normals[5] + morphNormals[5], QVector3D(2, -2, -4).normalized());
    }

    void checkTangentMorphTargetGeneration()
    {
        // GIVEN
        QGeometry *geometry = new QGeometry();

        QVector<QVector3D> positions;
        QVector<QVector2D> uv;
        QVector<QVector3D> morphPositions;

        positions.reserve(6);
        morphPositions.reserve(6);

        QVector3D a(-1.0f, 1.0, 0.0f);
        QVector3D b(1.0f, 1.0, 0.0f);
        QVector3D c(-1.0f, -1.0, 0.0f);
        QVector3D d(1.0f, -1.0, 0.0f);
        positions << a << b << c
                  << c << b << d;

        a = QVector3D(0, 0, 0);
        b = QVector3D(0, 0, 0);
        c = QVector3D(0, 0, 0);
        d = QVector3D(0, 0, 1.0f);
        morphPositions << a << b << c
                       << c << b << d;

        auto a2D = QVector2D(0, 0);
        auto b2D = QVector2D(0, 1);
        auto c2D = QVector2D(1, 0);
        auto d2D = QVector2D(1, 1);
        uv << a2D << b2D << c2D
           << c2D << b2D << d2D;

        QByteArray rawPositionData;
        rawPositionData.resize(positions.size() * sizeof(QVector3D));
        std::memcpy(rawPositionData.data(), positions.data(), positions.size() * sizeof(QVector3D));

        QByteArray rawMorphPositionData;
        rawMorphPositionData.resize(morphPositions.size() * sizeof(QVector3D));
        std::memcpy(rawMorphPositionData.data(), morphPositions.data(), morphPositions.size() * sizeof(QVector3D));

        QByteArray rawUVData;
        rawUVData.resize(uv.size() * sizeof(QVector2D));
        std::memcpy(rawUVData.data(), uv.data(), uv.size() * sizeof(QVector2D));

        Qt3DGeometry::QBuffer *vertexBuffer = new Qt3DGeometry::QBuffer();
        vertexBuffer->setData(rawPositionData);

        Qt3DGeometry::QBuffer *morphVertexBuffer = new Qt3DGeometry::QBuffer();
        morphVertexBuffer->setData(rawMorphPositionData);

        Qt3DGeometry::QBuffer *uvBuffer = new Qt3DGeometry::QBuffer();
        uvBuffer->setData(rawUVData);

        QAttribute *posAttribute = new QAttribute();
        posAttribute->setName(QAttribute::defaultPositionAttributeName());
        posAttribute->setBuffer(vertexBuffer);
        posAttribute->setByteOffset(0);
        posAttribute->setByteStride(0);
        posAttribute->setVertexBaseType(QAttribute::Float);
        posAttribute->setVertexSize(3);
        posAttribute->setAttributeType(QAttribute::VertexAttribute);
        posAttribute->setCount(6);

        QAttribute *posMorphAttribute = new QAttribute();
        posMorphAttribute->setName(QAttribute::defaultPositionAttributeName() + QStringLiteral("_1"));
        posMorphAttribute->setBuffer(morphVertexBuffer);
        posMorphAttribute->setByteOffset(0);
        posMorphAttribute->setByteStride(0);
        posMorphAttribute->setVertexBaseType(QAttribute::Float);
        posMorphAttribute->setVertexSize(3);
        posMorphAttribute->setAttributeType(QAttribute::VertexAttribute);
        posMorphAttribute->setCount(6);

        QAttribute *uvAttribute = new QAttribute();
        uvAttribute->setName(QAttribute::defaultTextureCoordinateAttributeName());
        uvAttribute->setBuffer(vertexBuffer);
        uvAttribute->setByteOffset(0);
        uvAttribute->setByteStride(0);
        uvAttribute->setVertexBaseType(QAttribute::Float);
        uvAttribute->setVertexSize(2);
        uvAttribute->setAttributeType(QAttribute::VertexAttribute);
        uvAttribute->setCount(6);

        geometry->addAttribute(posAttribute);
        geometry->addAttribute(posMorphAttribute);
        geometry->addAttribute(uvAttribute);

        // THEN
        QVERIFY(MeshParserUtils::needsNormalAttribute(geometry, QGeometryRenderer::Triangles));

        // WHEN
        MeshParserUtils::createNormalsForGeometry(geometry, QGeometryRenderer::Triangles);

        // THEN
        QVERIFY(MeshParserUtils::needsTangentAttribute(geometry, QGeometryRenderer::Triangles));

        // WHEN
        MeshParserUtils::createTangentForGeometry(geometry, QGeometryRenderer::Triangles);

        // THEN
        QVERIFY(!MeshParserUtils::needsTangentAttribute(geometry, QGeometryRenderer::Triangles));

        const auto attributes = geometry->attributes();
        auto tangentMorphAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                               [](QAttribute *attr) {
                                                   return attr->name() == QAttribute::defaultTangentAttributeName() + QStringLiteral("_1");
                                               });

        auto tangentAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                          [](QAttribute *attr) {
                                              return attr->name() == QAttribute::defaultTangentAttributeName();
                                          });
        QVERIFY(tangentAttrIt != std::end(attributes));
        QVERIFY(tangentMorphAttrIt != std::end(attributes));

        QAttribute *tangentAttribute = *tangentAttrIt;
        QVERIFY(tangentAttribute->vertexSize() == 4);
        Qt3DGeometry::QBuffer *tangentBuffer = tangentAttribute->buffer();
        const QByteArray rawData = tangentBuffer->data();
        const QVector4D *tangents = reinterpret_cast<const QVector4D *>(rawData.constData());

        QAttribute *tangentMorphAttribute = *tangentMorphAttrIt;
        Qt3DGeometry::QBuffer *tangentMorphBuffer = tangentMorphAttribute->buffer();
        QVERIFY(tangentMorphAttribute->vertexSize() == 3);
        const QByteArray rawMorphData = tangentMorphBuffer->data();
        const QVector3D *morphTangents = reinterpret_cast<const QVector3D *>(rawMorphData.constData());

        QVERIFY(qFuzzyCompare(tangents[0] + morphTangents[0], QVector4D(1, 0, 0, 1)));
        QVERIFY(qFuzzyCompare(tangents[1] + morphTangents[1], QVector4D(1, 0, 0, 1)));
        QVERIFY(qFuzzyCompare(tangents[2] + morphTangents[2], QVector4D(1, 0, 0, 1)));

        QVERIFY(qFuzzyCompare(tangents[3] + morphTangents[3], QVector4D(std::sqrt(2.0f) / 2.0f, std::sqrt(2.0f) / 2.0f, 0, -1)));
        QVERIFY(qFuzzyCompare(tangents[4] + morphTangents[4], QVector4D(std::sqrt(2.0f) / 2.0f, std::sqrt(2.0f) / 2.0f, 0, -1)));
        QVERIFY(qFuzzyCompare(tangents[5] + morphTangents[5], QVector4D(std::sqrt(2.0f) / 2.0f, std::sqrt(2.0f) / 2.0f, 0, -1)));
    }
};

QTEST_APPLESS_MAIN(tst_MeshParserUtils)

#include "tst_meshparser_utils.moc"
