/*
    tst_nodeparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/private/nodeparser_p.h>
#include <Kuesa/private/gltf2context_p.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_NodeParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkNodeTree()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "nodes_tree.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(5));

        const QVector<QString> names = {
            QLatin1String("Car"),
            QLatin1String("wheel_1"),
            QLatin1String("wheel_2"),
            QLatin1String("wheel_3"),
            QLatin1String("wheel_4"),
        };
        for (int i = 0; i < names.size(); ++i) {
            const auto &node = context.treeNode(i);
            QVERIFY(!node.entity);
            QCOMPARE(node.name, names.at(i));
            QCOMPARE(node.hasPlaceholder, false);

            for (auto &joint : node.joints)
                QVERIFY(!joint);
        }
    }

    void checkNodeTransformWithSTR()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "nodes_str.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(1));

        const TreeNode::TransformInfo transform = context.treeNode(0).transformInfo;

        QCOMPARE(transform.bits, TreeNode::TransformInfo::TranslationSet | TreeNode::TransformInfo::RotationSet | TreeNode::TransformInfo::ScaleSet);
        QCOMPARE(transform.translation, QVector3D(-17.7082, -11.4156, 2.0922));
        QCOMPARE(transform.rotation, QQuaternion(1, 0, 0, 0));
        QCOMPARE(transform.scale3D, QVector3D(1, 2, 3));
    }

    void checkNodeTransformWithMatrix()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "nodes_matrix.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(1));

        const TreeNode::TransformInfo transform = context.treeNode(0).transformInfo;
        QVERIFY(transform.bits == TreeNode::TransformInfo::MatrixSet);

        const QMatrix4x4 &matrix = transform.matrix;
        QMatrix4x4 expectedMatrix;
        expectedMatrix(0, 0) = 2;
        expectedMatrix(1, 1) = 3;
        expectedMatrix(2, 2) = 1;
        expectedMatrix(0, 3) = 1;
        expectedMatrix(1, 3) = 3;
        expectedMatrix(2, 3) = 4;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                QCOMPARE(matrix(i, j), expectedMatrix(i, j));
    }

    void checkEmptyNodesArray()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "nodes_empty.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isObject());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, false);
    }

    void checkNodesSTRWithWrongSize()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "nodes_str_wrong_size.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, false);
    }

    void checkCameraCorrectionRename()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "camera_correction.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonValue nodes = json.object().value(QLatin1String("nodes"));
        QVERIFY(!json.isNull() && nodes.isArray());

        // WHEN
        const bool success = parser.parse(nodes.toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(1));

        const TreeNode n = context.treeNode(0);
        QVERIFY(n.cameraIdx != -1);
        QCOMPARE(n.name, QStringLiteral("Camera"));
    }

    void checkMorphTargetWeights()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "morphtarget_weights.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonValue nodes = json.object().value(QLatin1String("nodes"));
        QVERIFY(!json.isNull() && nodes.isArray());

        // WHEN
        const bool success = parser.parse(nodes.toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(1));

        const TreeNode n = context.treeNode(0);
        QCOMPARE(n.morphTargetWeights.size(), 2);
        QCOMPARE(n.morphTargetWeights.at(0), 0.5f);
        QCOMPARE(n.morphTargetWeights.at(1), 0.8f);
        QCOMPARE(n.name, QStringLiteral("Morphed"));
    }

    void checkDeprecatedMultipleExtensionsOnNode()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "node_multi_extensions_deprecated.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonValue nodes = json.object().value(QLatin1String("nodes"));
        QVERIFY(!json.isNull() && nodes.isArray());

        // WHEN
        const bool success = parser.parse(nodes.toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(1));

        const TreeNode n = context.treeNode(0);
        QCOMPARE(n.reflectionPlaneEquation, QVector4D(1.0f, 0.0f, 0.0f, 883.0f));
        QCOMPARE(n.layerIndices, QVector<int>({ 0, 1, 2 }));
        QCOMPARE(n.hasPlaceholder, true);
        QCOMPARE(n.placeHolder.cameraNode, 1);
    }

    void checkMultipleExtensionsOnNode()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "node_multi_extensions.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonValue nodes = json.object().value(QLatin1String("nodes"));
        QVERIFY(!json.isNull() && nodes.isArray());

        // WHEN
        const bool success = parser.parse(nodes.toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(1));

        const TreeNode n = context.treeNode(0);
        QCOMPARE(n.reflectionPlaneEquation, QVector4D(1.0f, 0.0f, 0.0f, 883.0f));
        QCOMPARE(n.layerIndices, QVector<int>({ 0, 1, 2 }));
        QCOMPARE(n.hasPlaceholder, true);
        QCOMPARE(n.placeHolder.cameraNode, 1);
    }

    void checkParsesExtras()
    {
        // GIVEN
        GLTF2Context context;
        NodeParser parser;
        QFile file(QStringLiteral(ASSETS "node_extras.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonValue nodes = json.object().value(QLatin1String("nodes"));
        QVERIFY(!json.isNull() && nodes.isArray());

        // WHEN
        const bool success = parser.parse(nodes.toArray(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.treeNodeCount(), size_t(1));

        const TreeNode n = context.treeNode(0);
        QCOMPARE(n.extras.size(), 3);
        const std::pair<QString, QVariant> expectedValues[] = {
            { QStringLiteral("myBoolProp"), QVariant(true) },
            { QStringLiteral("myDoubleProp"), QVariant(1584.883) },
            { QStringLiteral("myStringProp"), QVariant(QStringLiteral("Kuesa_String")) },
        };
        QCOMPARE(n.extras[0], expectedValues[0]);
        QCOMPARE(n.extras[1], expectedValues[1]);
        QCOMPARE(n.extras[2], expectedValues[2]);
    }
};

QTEST_APPLESS_MAIN(tst_NodeParser)

#include "tst_nodeparser.moc"
