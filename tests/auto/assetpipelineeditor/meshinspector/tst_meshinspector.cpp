/*
    tst_meshinspector.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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
#include <Kuesa/SceneEntity>
#include <Kuesa/private/gltf2parser_p.h>
#include <QAttribute>
#include <QGeometryRenderer>
#include <QScopedPointer>
#include <QSignalSpy>
#include "meshinspector.h"

using namespace Kuesa;
using namespace GLTF2Import;
using namespace Qt3DRender;

class tst_MeshInspector : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init()
    {
        m_scene.reset(new SceneEntity);
        m_boxMesh = loadMesh(QString(ASSETS "Box.gltf"), QStringLiteral("Mesh_0"));
        QVERIFY(m_boxMesh);
        m_inspector.reset(new MeshInspector(this));
        m_inspector->setMesh(m_boxMesh);
    }

    void testInspectorParameters()
    {
        // THEN
        QCOMPARE(m_inspector->assetName(), QLatin1String("Mesh_0"));
        QCOMPARE(m_inspector->vertexCount(), 24);
        QCOMPARE(m_inspector->primitiveCount(), 12);
        QCOMPARE(m_inspector->primitiveType(), QLatin1String("Triangles"));
        //expected total size is sum of byteLength for each bufferView in GLTF
        QCOMPARE(m_inspector->totalSize(), 576 + 72);
    }

    void testMeshDestruction()
    {
        // GIVEN
        QSignalSpy spy(m_inspector.data(), SIGNAL(meshParamsChanged()));

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(spy.count(), 0);
        QVERIFY(!m_inspector->assetName().isEmpty());
        QVERIFY(m_inspector->vertexCount() > 0);
        QVERIFY(m_inspector->primitiveCount() > 0);
        QVERIFY(m_inspector->totalSize() > 0);
        QVERIFY(!m_inspector->primitiveType().isEmpty());
        QVERIFY(m_inspector->bufferModel()->rowCount() > 0);

        // WHEN
        m_scene.reset(); // will delete the mesh

        //THEN
        QCOMPARE(spy.count(), 1);
        QVERIFY(m_inspector->assetName().isEmpty());
        QVERIFY(m_inspector->vertexCount() == 0);
        QVERIFY(m_inspector->primitiveCount() == 0);
        QVERIFY(m_inspector->totalSize() == 0);
        QVERIFY(m_inspector->primitiveType().isEmpty());
        QVERIFY(m_inspector->bufferModel()->rowCount() == 0);
    }

    void testBufferModelStructure()
    {
        // GIVEN
        auto bufferModel = m_inspector->bufferModel();

        // THEN
        QVERIFY(bufferModel);
        QCOMPARE(bufferModel->rowCount(), m_boxMesh->geometry()->attributes().count());
        QVERIFY(bufferModel->columnCount() == BufferModel::LastColumn);
    }

    void testBufferModelData()
    {
        // GIVEN
        auto bufferModel = m_inspector->bufferModel();

        // WHEN
        int posRow = findBufferModelRow(bufferModel, QStringLiteral("vertexPosition"));

        // THEN
        QVERIFY(posRow != -1);
        QCOMPARE(bufferModel->index(posRow, BufferModel::BufferViewIndexColumn).data(Qt::EditRole).toInt(), 1);
        QCOMPARE(bufferModel->index(posRow, BufferModel::BufferTypeColumn).data(Qt::EditRole).toInt(), QAttribute::VertexAttribute);
        QCOMPARE(bufferModel->index(posRow, BufferModel::VertexTypeColumn).data(Qt::EditRole).toString(), QStringLiteral("Float"));
        QCOMPARE(bufferModel->index(posRow, BufferModel::CountColumn).data(Qt::EditRole).toInt(), 24);
        QCOMPARE(bufferModel->index(posRow, BufferModel::VertexSizeColumn).data(Qt::EditRole).toInt(), 3);
        QCOMPARE(bufferModel->index(posRow, BufferModel::OffsetColumn).data(Qt::EditRole).toInt(), 288);
        QCOMPARE(bufferModel->index(posRow, BufferModel::StrideColumn).data(Qt::EditRole).toInt(), 12);

        // WHEN
        int normalRow = findBufferModelRow(bufferModel, QStringLiteral("vertexNormal"));

        // THEN
        QVERIFY(normalRow != -1);
        QCOMPARE(bufferModel->index(normalRow, BufferModel::BufferViewIndexColumn).data(Qt::EditRole).toInt(), 1);
        QCOMPARE(bufferModel->index(normalRow, BufferModel::BufferTypeColumn).data(Qt::EditRole).toInt(), QAttribute::VertexAttribute);
        QCOMPARE(bufferModel->index(normalRow, BufferModel::VertexTypeColumn).data(Qt::EditRole).toString(), QStringLiteral("Float"));
        QCOMPARE(bufferModel->index(normalRow, BufferModel::CountColumn).data(Qt::EditRole).toInt(), 24);
        QCOMPARE(bufferModel->index(normalRow, BufferModel::VertexSizeColumn).data(Qt::EditRole).toInt(), 3);
        QCOMPARE(bufferModel->index(normalRow, BufferModel::OffsetColumn).data(Qt::EditRole).toInt(), 0);
        QCOMPARE(bufferModel->index(normalRow, BufferModel::StrideColumn).data(Qt::EditRole).toInt(), 12);

        // WHEN
        int idxBufferRow = findBufferModelRow(bufferModel, QStringLiteral("Index"));

        // THEN
        QVERIFY(idxBufferRow != -1);
        QCOMPARE(bufferModel->index(idxBufferRow, BufferModel::BufferViewIndexColumn).data(Qt::EditRole).toInt(), 0);
        QCOMPARE(bufferModel->index(idxBufferRow, BufferModel::BufferTypeColumn).data(Qt::EditRole).toInt(), QAttribute::IndexAttribute);
        QCOMPARE(bufferModel->index(idxBufferRow, BufferModel::VertexTypeColumn).data(Qt::EditRole).toString(), QStringLiteral("U-Short"));
        QCOMPARE(bufferModel->index(idxBufferRow, BufferModel::CountColumn).data(Qt::EditRole).toInt(), 36);
        QCOMPARE(bufferModel->index(idxBufferRow, BufferModel::VertexSizeColumn).data(Qt::EditRole).toInt(), 1);
        QCOMPARE(bufferModel->index(idxBufferRow, BufferModel::OffsetColumn).data(Qt::EditRole).toInt(), 0);
        QCOMPARE(bufferModel->index(idxBufferRow, BufferModel::StrideColumn).data(Qt::EditRole).toInt(), 0);
    }

    void testBufferModelRoles()
    {
        // GIVEN
        auto bufferModel = m_inspector->bufferModel();

        // THEN
        // Data for EditRole must be definied and equal DisplayRole because we're displaying using QDataWidgetMapper
        for (int row = 0; row < bufferModel->rowCount(); ++row) {
            for (int col = 0; col < BufferModel::LastColumn; ++col) {
                const auto index = bufferModel->index(row, col);
                QVERIFY(index.data(Qt::DisplayRole) == index.data(Qt::EditRole));
            }
        }
    }

    void testVertexBaseTypeName()
    {
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::Byte), QStringLiteral("Byte"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::UnsignedByte), QStringLiteral("U-Byte"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::Short), QStringLiteral("Short"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::UnsignedShort), QStringLiteral("U-Short"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::Int), QStringLiteral("Int"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::UnsignedInt), QStringLiteral("U-Int"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::HalfFloat), QStringLiteral("Half Float"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::Float), QStringLiteral("Float"));
        QCOMPARE(BufferModel::vertexBaseTypeName(Qt3DRender::QAttribute::Double), QStringLiteral("Double"));
    }

    void testPrimitiveCountCalculation()
    {
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::Points), 12U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::Lines), 6U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::LineLoop), 12U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::LineStrip), 11U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::Triangles), 4U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::TriangleStrip), 10U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::TriangleFan), 10U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::LinesAdjacency), 0U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::TrianglesAdjacency), 0U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::LineStripAdjacency), 0U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::TriangleStripAdjacency), 0U);
        QCOMPARE(MeshInspector::calculatePrimitiveCount(12, QGeometryRenderer::Patches), 0U);
    }

    void testPrimitiveName()
    {
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::Points), QStringLiteral("Points"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::Lines), QStringLiteral("Lines"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::LineLoop), QStringLiteral("LineLoop"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::LineStrip), QStringLiteral("LineStrip"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::Triangles), QStringLiteral("Triangles"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::TriangleStrip), QStringLiteral("TriangleStrip"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::TriangleFan), QStringLiteral("TriangleFan"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::LinesAdjacency), QStringLiteral("Unknown"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::TrianglesAdjacency), QStringLiteral("Unknown"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::LineStripAdjacency), QStringLiteral("Unknown"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::TriangleStripAdjacency), QStringLiteral("Unknown"));
        QCOMPARE(MeshInspector::nameForPrimitiveType(QGeometryRenderer::Patches), QStringLiteral("Unknown"));
    }

    void testAttributeSize()
    {
        // test size for every QAttribute::VertexBaseType
        testAttributeSize(1, QAttribute::Byte);
        testAttributeSize(1, QAttribute::UnsignedByte);
        testAttributeSize(2, QAttribute::Short);
        testAttributeSize(2, QAttribute::UnsignedShort);
        testAttributeSize(4, QAttribute::Int);
        testAttributeSize(4, QAttribute::UnsignedInt);
        testAttributeSize(2, QAttribute::HalfFloat);
        testAttributeSize(4, QAttribute::Float);
        testAttributeSize(8, QAttribute::Double);

        // test total size calculation (10 float vertices of size 3 = 120)
        testAttributeSize(120, QAttribute::Float, 3, 10);
    }

private:
    int findBufferModelRow(BufferModel *bufferModel, const QString &searchString)
    {
        for (int row = 0; row < bufferModel->rowCount(); ++row) {
            if (bufferModel->index(row, BufferModel::BufferNameColumn).data(Qt::EditRole).toString().contains(searchString))
                return row;
        }
        return -1;
    }

    void testAttributeSize(const unsigned int expectedSize, const QAttribute::VertexBaseType baseType, unsigned int dataSize = 1, unsigned int count = 1)
    {
        QAttribute attribute(nullptr, QString(), baseType, dataSize, count);
        QCOMPARE(MeshInspector::attributeSizeInBytes(&attribute), expectedSize);
    }

    QGeometryRenderer *loadMesh(const QString &fileName, const QString &meshName) const
    {
        GLTF2Parser parser(m_scene.data(), m_scene.data());
        parser.parse(fileName);
        auto mesh = m_scene->meshes()->find(meshName);
        mesh->setParent(m_scene.data());
        return mesh;
    }

    QScopedPointer<SceneEntity> m_scene;
    QScopedPointer<MeshInspector> m_inspector;
    QGeometryRenderer *m_boxMesh;
};

QTEST_MAIN(tst_MeshInspector)
#include "tst_meshinspector.moc"
