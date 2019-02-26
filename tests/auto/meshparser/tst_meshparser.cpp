/*
    tst_meshparser.cpp

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
#include <Kuesa/private/bufferparser_p.h>
#include <Kuesa/private/gltf2context_p.h>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QAttribute>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_MeshParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("meshesCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "simple_cube.gltf")
                               << true
                               << 1;

        QTest::newRow("Empty") << QStringLiteral(ASSETS "meshparser_empty.gltf")
                               << false
                               << 0;

        QTest::newRow("No_Primitive") << QStringLiteral(ASSETS "meshparser_no_primitive.gltf")
                                      << false
                                      << 0;

        QTest::newRow("Empty_Primitive") << QStringLiteral(ASSETS "meshparser_empty_primitive.gltf")
                                         << false
                                         << 0;

        QTest::newRow("No_Attribute") << QStringLiteral(ASSETS "meshparser_no_attribute.gltf")
                                      << false
                                      << 0;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, meshesCount);

        // GIVEN
        GLTF2Context context;

        Accessor positionAccessor;
        positionAccessor.dataSize = 3;

        Accessor normalAccessor;
        normalAccessor.dataSize = 3;

        Accessor indicesAccessor;
        indicesAccessor.dataSize = 1;
        indicesAccessor.type = Qt3DRender::QAttribute::VertexBaseType::UnsignedShort;

        context.addAccessor(normalAccessor);
        context.addAccessor(positionAccessor);
        context.addAccessor(indicesAccessor);

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
        QCOMPARE(success, succeeded);
        QCOMPARE(context.meshesCount(), meshesCount);
    }

    // The asset pipeline editor needs extra info from GLTF file that's not stored in the Qt3D classes
    void checkForAssetPipelineEditorProperties()
    {
        // GIVEN
        GLTF2Context context;
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
        for (int meshNo = 0; meshNo < context.meshesCount(); ++meshNo) {
            auto parsedMesh = context.mesh(meshNo);
            for (auto primitive : parsedMesh.meshPrimitives) {
                auto qMesh = primitive.primitiveRenderer;
                for (auto attribute : qMesh->geometry()->attributes()) {
                    QVERIFY(attribute->property("bufferIndex").isValid());
                    QVERIFY(attribute->property("bufferViewIndex").isValid());
                    QVERIFY(attribute->property("bufferViewOffset").isValid());
                    QVERIFY(attribute->property("bufferName").isValid());
                }
            }
        }
    }
};

QTEST_GUILESS_MAIN(tst_MeshParser)

#include "tst_meshparser.moc"
