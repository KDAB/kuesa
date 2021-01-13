/*
    nodeparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "nodeparser_p.h"
#include "kuesa_p.h"
#include "gltf2context_p.h"

#include <QMatrix4x4>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>
#include <QPair>
#include <Qt3DCore/private/qmath3d_p.h>
#include <gltf2keys_p.h>
#include <functional>
#include <memory>

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

QMatrix4x4 matrixFromArray(const QJsonArray &matrixValues)
{
    if (matrixValues.size() != 16) {
        qCWarning(Kuesa::kuesa) << "Matrix arrays should contain 16 elements";
        return {};
    }

    float m[16];
    std::transform(std::begin(matrixValues), std::end(matrixValues),
                   std::begin(m), [](const QJsonValue &v) { return v.toDouble(); });

    QMatrix4x4 matrix(m[0], m[4], m[8], m[12],
                      m[1], m[5], m[9], m[13],
                      m[2], m[6], m[10], m[14],
                      m[3], m[7], m[11], m[15]);
    return matrix;
}

QPair<bool, TreeNode> treenodeFromJson(const QJsonObject &nodeObj)
{
    TreeNode node;
    node.name = nodeObj.value(KEY_NAME).toString();
    node.meshIdx = nodeObj.value(KEY_MESH).toInt(-1);
    node.skinIdx = nodeObj.value(KEY_SKIN).toInt(-1);
    node.cameraIdx = nodeObj.value(KEY_CAMERA).toInt(-1);

    // Handle cases were the Khronos GLTF exporter has renamed a camera
    // Correction_CameraName
    if (node.cameraIdx != -1 && node.name.startsWith(QLatin1String("Correction_")))
        node.name = node.name.mid(11);

    const QJsonArray &childrenArray = nodeObj.value(KEY_CHILDREN).toArray();
    node.childrenIndices.reserve(childrenArray.size());

    for (const QJsonValue &v : childrenArray) {
        const qint32 childIdx = v.toInt(-1);
        if (childIdx < 0) {
            qCWarning(Kuesa::kuesa, "Node referencing invalid child");
            return QPair<bool, TreeNode>(false, node);
        }
        if (node.childrenIndices.contains(childIdx)) {
            qCWarning(Kuesa::kuesa, "Node referencing same child twice");
            return QPair<bool, TreeNode>(false, node);
        }
        node.childrenIndices.push_back(childIdx);
    }

    if (nodeObj.contains(KEY_MATRIX)) {
        const QJsonArray matrixData = nodeObj[KEY_MATRIX].toArray();
        const QMatrix4x4 matrix = matrixFromArray(matrixData);
        node.transformInfo.matrix = matrix;
        node.transformInfo.bits |= TreeNode::TransformInfo::MatrixSet;
    } else {
        const QVector<QPair<QLatin1String, std::function<bool(const QJsonArray &)>>> transformConverters{
            { KEY_SCALE, [&node](const QJsonArray &transformElement) {
                 if (transformElement.size() == 3) {
                     auto scale = QVector3D(transformElement[0].toDouble(),
                                            transformElement[1].toDouble(),
                                            transformElement[2].toDouble());
                     node.transformInfo.scale3D = scale;
                     node.transformInfo.bits |= TreeNode::TransformInfo::ScaleSet;
                     return true;
                 } else {
                     qCWarning(Kuesa::kuesa, "Node Wrong scale size");
                     return false;
                 }
             } },
            { KEY_ROTATION, [&node](const QJsonArray &transformElement) {
                 if (transformElement.size() == 4) {
                     auto rotation = QQuaternion(transformElement[3].toDouble(),
                                                 transformElement[0].toDouble(),
                                                 transformElement[1].toDouble(),
                                                 transformElement[2].toDouble());
                     node.transformInfo.rotation = rotation;
                     node.transformInfo.bits |= TreeNode::TransformInfo::RotationSet;
                     return true;
                 } else {
                     qCWarning(Kuesa::kuesa, "Node Wrong rotation size");
                     return false;
                 }
             } },
            { KEY_TRANSLATION, [&node](const QJsonArray &transformElement) {
                 if (transformElement.size() == 3) {
                     auto translation = QVector3D(transformElement[0].toDouble(),
                                                  transformElement[1].toDouble(),
                                                  transformElement[2].toDouble());
                     node.transformInfo.translation = translation;
                     node.transformInfo.bits |= TreeNode::TransformInfo::TranslationSet;
                     return true;
                 } else {
                     qCWarning(Kuesa::kuesa, "Node Wrong translation size");
                     return false;
                 }
             } }
        };

        for (const auto &transformConverter : transformConverters) {
            const QJsonValue &transformElementValue = nodeObj[transformConverter.first];
            if (!transformElementValue.isUndefined()) {
                if (!transformConverter.second(transformElementValue.toArray())) {
                    return QPair<bool, TreeNode>(false, node);
                }
            }
        }
    }

    const QJsonObject nodeExtensions = nodeObj.value(KEY_EXTENSIONS).toObject();

    // Layer Extensions
    if (nodeExtensions.contains(KEY_KDAB_KUESA_LAYER_EXTENSION)) {
        const QJsonObject kdabLayerExtension = nodeExtensions.value(KEY_KDAB_KUESA_LAYER_EXTENSION).toObject();
        // If the node references layers, add them
        if (kdabLayerExtension.contains(KEY_NODE_KUESA_LAYERS)) {
            const QJsonArray layerIds = kdabLayerExtension.value(KEY_NODE_KUESA_LAYERS).toArray();
            for (const QJsonValue &layerValue : layerIds) {
                const int layerId = layerValue.toInt(-1);
                if (layerId < 0) {
                    qCWarning(Kuesa::kuesa, "Node referencing invalid Kuesa Layer");
                    return QPair<bool, TreeNode>(false, node);
                }
                node.layerIndices.push_back(layerId);
            }
        }
    }
    if (nodeExtensions.contains(KEY_KHR_LIGHTS_PUNCTUAL_EXTENSION)) {
        const QJsonObject lightObject = nodeExtensions.value(KEY_KHR_LIGHTS_PUNCTUAL_EXTENSION).toObject();
        node.lightIdx = lightObject.value(KEY_KHR_PUNCTUAL_LIGHT).toInt(-1);
    }
    if (nodeExtensions.contains(KEY_KDAB_REFLECTION_PLANES_EXTENSION)) {
        const QJsonObject reflectionPlaneObject = nodeExtensions.value(KEY_KDAB_REFLECTION_PLANES_EXTENSION).toObject();
        const QJsonArray planeEquationValue = reflectionPlaneObject.value(KEY_PLANE).toArray();
        QVector4D planeEquation;
        if (planeEquationValue.size() == 4) {
            planeEquation = QVector4D(planeEquationValue[0].toDouble(),
                    planeEquationValue[1].toDouble(),
                    planeEquationValue[2].toDouble(),
                    planeEquationValue[3].toDouble());
        }
        if (planeEquation.isNull())
            qCWarning(Kuesa::kuesa, "Invalid Plane Equation for Reflection Plane");
        node.reflectionPlaneEquation = planeEquation;
    }

    if (nodeExtensions.contains(KEY_KDAB_PLACEHOLDER)) {
        const QJsonObject placeHolderObject = nodeExtensions.value(KEY_KDAB_PLACEHOLDER).toObject();
        const auto cameraNode = placeHolderObject.value(KEY_KDAB_PLACEHOLDER_CAMERANODE).toInt();
        node.placeHolder = TreeNode::KDAB_2d_placeholder{static_cast<qint32>(cameraNode)};
        node.hasPlaceholder = true;
    }

    const QJsonArray morphTargetWeights = nodeObj.value(KEY_WEIGHTS).toArray();
    node.morphTargetWeights.reserve(morphTargetWeights.size());
    for (const QJsonValue &weight : morphTargetWeights)
        node.morphTargetWeights.push_back(weight.toDouble(0.0));

    return QPair<bool, TreeNode>(true, node);
}

} // namespace

NodeParser::NodeParser()
{
}

bool NodeParser::parse(const QJsonArray &nodes, GLTF2Context *context) const
{
    const qint32 nbNodes = nodes.size();
    for (qint32 nodeId = 0; nodeId < nbNodes; ++nodeId) {
        const QJsonObject &nodeObject = nodes.at(nodeId).toObject();
        const QPair<bool, TreeNode> treeNodeCreationResult = treenodeFromJson(nodeObject);
        if (!treeNodeCreationResult.first)
            return false;
        context->addTreeNode(treeNodeCreationResult.second);
    }

    return nbNodes > 0;
}

QT_END_NAMESPACE
