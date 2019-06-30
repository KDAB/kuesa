/*
    tst_meshparser_utils.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QBuffer>
#include <cstring>

#include <iostream>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_MeshParserUtils : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkNeedsTangentAttribute()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QGeometry> geo(new Qt3DRender::QGeometry());

        // WHEN
        bool needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                                    Qt3DRender::QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsTangents, false);

        // GIVEN
        Qt3DRender::QAttribute *posAttr = new Qt3DRender::QAttribute();
        posAttr->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        Qt3DRender::QAttribute *normalAttr = new Qt3DRender::QAttribute();
        normalAttr->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
        Qt3DRender::QAttribute *uvAttr = new Qt3DRender::QAttribute();
        uvAttr->setName(Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName());
        geo->addAttribute(posAttr);
        geo->addAttribute(normalAttr);
        geo->addAttribute(uvAttr);

        // WHEN
        needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                               Qt3DRender::QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsTangents, true);

        // WHEN
        needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                               Qt3DRender::QGeometryRenderer::Lines);

        // THEN
        QCOMPARE(needsTangents, false);

        // WHEN
        Qt3DRender::QAttribute *tangentAttr = new Qt3DRender::QAttribute();
        tangentAttr->setName(Qt3DRender::QAttribute::defaultTangentAttributeName());
        geo->addAttribute(tangentAttr);
        needsTangents = MeshParserUtils::needsTangentAttribute(geo.data(),
                                                               Qt3DRender::QGeometryRenderer::Triangles);
        QCOMPARE(needsTangents, false);
    }

    void checkNeedsNormalAttribute()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QGeometry> geo(new Qt3DRender::QGeometry());

        // WHEN
        bool needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                                  Qt3DRender::QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsNormals, false);

        // WHEN
        Qt3DRender::QAttribute *posAttr = new Qt3DRender::QAttribute();
        posAttr->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        geo->addAttribute(posAttr);
        needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                             Qt3DRender::QGeometryRenderer::Triangles);

        // THEN
        QCOMPARE(needsNormals, true);

        // WHEN
        needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                             Qt3DRender::QGeometryRenderer::Lines);

        // THEN
        QCOMPARE(needsNormals, false);

        // WHEN
        Qt3DRender::QAttribute *normalsAttr = new Qt3DRender::QAttribute();
        normalsAttr->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
        geo->addAttribute(normalsAttr);
        needsNormals = MeshParserUtils::needsNormalAttribute(geo.data(),
                                                             Qt3DRender::QGeometryRenderer::Triangles);
        QCOMPARE(needsNormals, false);
    }

    void checkNormalGeneration_data()
    {
        QTest::addColumn<Qt3DRender::QGeometry *>("geometry");
        QTest::addColumn<Qt3DRender::QGeometryRenderer::PrimitiveType>("primitiveType");
        QTest::addColumn<QVector<QVector3D>>("expectedNormals");

        {
            Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

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

            Qt3DRender::QBuffer *buffer = new Qt3DRender::QBuffer();
            buffer->setData(rawData);

            Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
            posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(buffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            posAttribute->setCount(6);

            geometry->addAttribute(posAttribute);

            QTest::newRow("NonIndexedTriangles") << geometry
                                                 << Qt3DRender::QGeometryRenderer::Triangles
                                                 << normals;
        }
        {
            Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

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

            Qt3DRender::QBuffer *buffer = new Qt3DRender::QBuffer();
            buffer->setData(rawData);

            Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
            posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(buffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            geometry->addAttribute(posAttribute);

            QTest::newRow("NonIndexedTriangleStrip") << geometry
                                                     << Qt3DRender::QGeometryRenderer::TriangleStrip
                                                     << normals;
        }
        {
            Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

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

            Qt3DRender::QBuffer *buffer = new Qt3DRender::QBuffer();
            buffer->setData(rawData);

            Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
            posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(buffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            geometry->addAttribute(posAttribute);

            QTest::newRow("NonIndexedTriangleFan") << geometry
                                                   << Qt3DRender::QGeometryRenderer::TriangleFan
                                                   << normals;
        }
        {
            Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

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

            Qt3DRender::QBuffer *vertexBuffer = new Qt3DRender::QBuffer();
            vertexBuffer->setData(rawPositionData);

            Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer();
            indexBuffer->setData(rawIndexData);

            Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
            posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(vertexBuffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
            indexAttribute->setBuffer(indexBuffer);
            indexAttribute->setByteOffset(0);
            indexAttribute->setByteStride(0);
            indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
            indexAttribute->setVertexSize(1);
            indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
            indexAttribute->setCount(6);

            geometry->addAttribute(indexAttribute);
            geometry->addAttribute(posAttribute);

            QTest::newRow("IndexedTriangles") << geometry
                                              << Qt3DRender::QGeometryRenderer::Triangles
                                              << normals;
        }
        {
            Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

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

            Qt3DRender::QBuffer *vertexBuffer = new Qt3DRender::QBuffer();
            vertexBuffer->setData(rawPositionData);

            Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer();
            indexBuffer->setData(rawIndexData);

            Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
            posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(vertexBuffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
            indexAttribute->setBuffer(indexBuffer);
            indexAttribute->setByteOffset(0);
            indexAttribute->setByteStride(0);
            indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
            indexAttribute->setVertexSize(1);
            indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
            indexAttribute->setCount(4);

            geometry->addAttribute(indexAttribute);
            geometry->addAttribute(posAttribute);

            QTest::newRow("IndexedTriangleStrip") << geometry
                                                  << Qt3DRender::QGeometryRenderer::TriangleStrip
                                                  << normals;
        }
        {
            Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

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

            Qt3DRender::QBuffer *vertexBuffer = new Qt3DRender::QBuffer();
            vertexBuffer->setData(rawPositionData);

            Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer();
            indexBuffer->setData(rawIndexData);

            Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
            posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
            posAttribute->setBuffer(vertexBuffer);
            posAttribute->setByteOffset(0);
            posAttribute->setByteStride(0);
            posAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
            posAttribute->setVertexSize(3);
            posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            posAttribute->setCount(4);

            Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
            indexAttribute->setBuffer(indexBuffer);
            indexAttribute->setByteOffset(0);
            indexAttribute->setByteStride(0);
            indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
            indexAttribute->setVertexSize(1);
            indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
            indexAttribute->setCount(4);

            geometry->addAttribute(indexAttribute);
            geometry->addAttribute(posAttribute);

            QTest::newRow("IndexedTriangleFan") << geometry
                                                << Qt3DRender::QGeometryRenderer::TriangleFan
                                                << normals;
        }
    }

    void checkNormalGeneration()
    {
        // GIVEN
        QFETCH(Qt3DRender::QGeometry *, geometry);
        QFETCH(Qt3DRender::QGeometryRenderer::PrimitiveType, primitiveType);
        QFETCH(QVector<QVector3D>, expectedNormals);

        // THEN
        QVERIFY(MeshParserUtils::needsNormalAttribute(geometry, primitiveType));

        // WHEN
        MeshParserUtils::createNormalsForGeometry(geometry, primitiveType);

        // THEN
        // Check we have no IndexAttribute
        const auto attributes = geometry->attributes();
        QVERIFY(std::find_if(std::begin(attributes), std::end(attributes),
                             [](Qt3DRender::QAttribute *attr) {
                                 return attr->attributeType() == Qt3DRender::QAttribute::IndexAttribute;
                             }) == std::end(attributes));

        QVERIFY(!MeshParserUtils::needsNormalAttribute(geometry, Qt3DRender::QGeometryRenderer::Triangles));

        auto normalAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                         [](Qt3DRender::QAttribute *attr) {
                                             return attr->name() == Qt3DRender::QAttribute::defaultNormalAttributeName();
                                         });
        QVERIFY(normalAttrIt != std::end(attributes));
        Qt3DRender::QAttribute *normalAttribute = *normalAttrIt;
        Qt3DRender::QBuffer *normalBuffer = normalAttribute->buffer();
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
        Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

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

        Qt3DRender::QBuffer *vertexBuffer = new Qt3DRender::QBuffer();
        vertexBuffer->setData(rawPositionData);

        Qt3DRender::QBuffer *morphVertexBuffer = new Qt3DRender::QBuffer();
        morphVertexBuffer->setData(rawMorphPositionData);

        Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
        posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        posAttribute->setBuffer(vertexBuffer);
        posAttribute->setByteOffset(0);
        posAttribute->setByteStride(0);
        posAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        posAttribute->setVertexSize(3);
        posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        posAttribute->setCount(6);

        Qt3DRender::QAttribute *posMorphAttribute = new Qt3DRender::QAttribute();
        posMorphAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName() + QStringLiteral("_1"));
        posMorphAttribute->setBuffer(morphVertexBuffer);
        posMorphAttribute->setByteOffset(0);
        posMorphAttribute->setByteStride(0);
        posMorphAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        posMorphAttribute->setVertexSize(3);
        posMorphAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        posMorphAttribute->setCount(6);

        geometry->addAttribute(posAttribute);
        geometry->addAttribute(posMorphAttribute);

        // THEN
        QVERIFY(MeshParserUtils::needsNormalAttribute(geometry, Qt3DRender::QGeometryRenderer::Triangles));

        // WHEN
        MeshParserUtils::createNormalsForGeometry(geometry, Qt3DRender::QGeometryRenderer::Triangles);

        // THEN
        QVERIFY(!MeshParserUtils::needsNormalAttribute(geometry, Qt3DRender::QGeometryRenderer::Triangles));

        const auto attributes = geometry->attributes();
        auto normalMorphAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                              [](Qt3DRender::QAttribute *attr) {
                                                  return attr->name() == Qt3DRender::QAttribute::defaultNormalAttributeName() + QStringLiteral("_1");
                                              });

        auto normalAttrIt = std::find_if(std::begin(attributes), std::end(attributes),
                                         [](Qt3DRender::QAttribute *attr) {
                                             return attr->name() == Qt3DRender::QAttribute::defaultNormalAttributeName();
                                         });
        QVERIFY(normalAttrIt != std::end(attributes));
        QVERIFY(normalMorphAttrIt != std::end(attributes));

        Qt3DRender::QAttribute *normalAttribute = *normalAttrIt;
        Qt3DRender::QBuffer *normalBuffer = normalAttribute->buffer();
        const QByteArray rawData = normalBuffer->data();
        const QVector3D *normals = reinterpret_cast<const QVector3D *>(rawData.constData());

        Qt3DRender::QAttribute *normalMorphAttribute = *normalMorphAttrIt;
        Qt3DRender::QBuffer *normalMorphBuffer = normalMorphAttribute->buffer();
        const QByteArray rawMorphData = normalMorphBuffer->data();
        const QVector3D *morphNormals = reinterpret_cast<const QVector3D *>(rawMorphData.constData());

        QCOMPARE(normals[0] + morphNormals[0], QVector3D(0, 0, -1));
        QCOMPARE(normals[1] + morphNormals[1], QVector3D(0, 0, -1));
        QCOMPARE(normals[2] + morphNormals[2], QVector3D(0, 0, -1));

        QCOMPARE(normals[3] + morphNormals[3], QVector3D(2, -2, -4).normalized());
        QCOMPARE(normals[4] + morphNormals[4], QVector3D(2, -2, -4).normalized());
        QCOMPARE(normals[5] + morphNormals[5], QVector3D(2, -2, -4).normalized());
    }
};

QTEST_APPLESS_MAIN(tst_MeshParserUtils)

#include "tst_meshparser_utils.moc"
