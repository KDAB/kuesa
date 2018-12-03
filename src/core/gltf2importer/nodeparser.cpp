/*
    nodeparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <functional>
#include <memory>

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

const QLatin1String KEY_NAME = QLatin1Literal("name");
const QLatin1String KEY_MATRIX = QLatin1Literal("matrix");
const QLatin1String KEY_TRANSLATION = QLatin1Literal("translation");
const QLatin1String KEY_ROTATION = QLatin1Literal("rotation");
const QLatin1String KEY_SCALE = QLatin1Literal("scale");
const QLatin1String KEY_MESH = QLatin1Literal("mesh");
const QLatin1String KEY_CAMERA = QLatin1Literal("camera");
const QLatin1String KEY_SKIN = QLatin1String("skin");
const QLatin1String KEY_CHILDREN = QLatin1Literal("children");
const QLatin1String KEY_EXTENSIONS = QLatin1String("extensions");
const QLatin1String KEY_KDAB_KUESA_LAYER_EXTENSION = QLatin1String("KDAB_Kuesa_Layers");
const QLatin1String KEY_NODE_KUESA_LAYERS = QLatin1Literal("layers");

QMatrix4x4 matrixFromArray(const QJsonArray &matrixValues)
{
    if (matrixValues.size() != 16) {
        qCWarning(kuesa) << "Matrix arrays should contain 16 elements";
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
        const int childIdx = v.toInt(-1);
        if (childIdx < 0) {
            qCWarning(kuesa, "Node referencing invalid child");
            return QPair<bool, TreeNode>(false, node);
        }
        if (node.childrenIndices.contains(childIdx)) {
            qCWarning(kuesa, "Node referencing same child twice");
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
        const QVector<QPair<QLatin1String, std::function<bool (const QJsonArray &)>>> transformConverters
        {
            { KEY_SCALE, [&node] (const QJsonArray &transformElement) {
                    if (transformElement.size() == 3) {
                        auto scale = QVector3D(transformElement[0].toDouble(),
                                               transformElement[1].toDouble(),
                                               transformElement[2].toDouble());
                        node.transformInfo.scale3D = scale;
                        node.transformInfo.bits |= TreeNode::TransformInfo::ScaleSet;
                        return true;
                    } else {
                        qCWarning(kuesa, "Node Wrong scale size");
                        return false;
                    }
                }
            },
            { KEY_ROTATION, [&node] (const QJsonArray &transformElement) {
                    if (transformElement.size() == 4) {
                        auto rotation = QQuaternion(transformElement[3].toDouble(),
                                                    transformElement[0].toDouble(),
                                                    transformElement[1].toDouble(),
                                                    transformElement[2].toDouble());
                        node.transformInfo.rotation = rotation;
                        node.transformInfo.bits |= TreeNode::TransformInfo::RotationSet;
                        return true;
                    } else {
                        qCWarning(kuesa, "Node Wrong rotation size");
                        return false;
                    }
                }
            },
            { KEY_TRANSLATION, [&node] (const QJsonArray &transformElement) {
                    if (transformElement.size() == 3) {
                        auto translation = QVector3D(transformElement[0].toDouble(),
                                                     transformElement[1].toDouble(),
                                                     transformElement[2].toDouble());
                        node.transformInfo.translation = translation;
                        node.transformInfo.bits |= TreeNode::TransformInfo::TranslationSet;
                        return true;
                    } else {
                        qCWarning(kuesa, "Node Wrong translation size");
                        return false;
                    }
                }
            }
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
                    qCWarning(kuesa, "Node referencing invalid Kuesa Layer");
                    return QPair<bool, TreeNode>(false, node);
                }
                node.layerIndices.push_back(layerId);
            }
        }
    }

    return QPair<bool, TreeNode>(true, node);
}

} // anonymous

NodeParser::NodeParser()
{
}

bool NodeParser::parse(const QJsonArray &nodes, GLTF2Context *context) const
{
    const int nbNodes = nodes.size();
    for (int nodeId = 0; nodeId < nbNodes; ++nodeId) {
        const QJsonObject &nodeObject = nodes.at(nodeId).toObject();
        const QPair<bool, TreeNode> treeNodeCreationResult = treenodeFromJson(nodeObject);
        if (!treeNodeCreationResult.first)
            return false;
        context->addTreeNode(treeNodeCreationResult.second);
    }

    return nbNodes > 0;
}

QT_END_NAMESPACE
