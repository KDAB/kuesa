/*
    meshparser.cpp

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

#include <qtkuesa-config.h>
#if defined(KUESA_DRACO_COMPRESSION)
#include <Kuesa/private/draco_prefix_p.h>
#include <draco/compression/decode.h>
#endif

#include "meshparser_p.h"
#include "bufferviewsparser_p.h"
#include "gltf2context_p.h"
#include "kuesa_p.h"

#include "meshparser_utils_p.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QElapsedTimer>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <QtGui/qopengl.h>

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

const QLatin1String KEY_PRIMITIVES = QLatin1String("primitives");
const QLatin1String KEY_ATTRIBUTES = QLatin1String("attributes");
const QLatin1String KEY_INDICES = QLatin1String("indices");
const QLatin1String KEY_MATERIAL = QLatin1String("material");
const QLatin1String KEY_MODE = QLatin1String("mode");
const QLatin1String KEY_NAME = QLatin1String("name");
const QLatin1String KEY_TARGETS = QLatin1String("targets");
const QLatin1String KEY_WEIGHTS = QLatin1String("weights");
#if defined(KUESA_DRACO_COMPRESSION)
const QLatin1String KEY_EXTENSIONS = QLatin1String("extensions");
const QLatin1String KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION = QLatin1String("KHR_draco_mesh_compression");
const QLatin1String KEY_BUFFERVIEW = QLatin1String("bufferView");
#endif

const QString morphTargetAttributeNames[]{ QStringLiteral("POSITION"),
                                           QStringLiteral("NORMAL"),
                                           QStringLiteral("TANGENT") };

QString standardAttributeNameFromSemantic(const QString &semantic)
{
    // Standard Attributes
    if (semantic.startsWith(QLatin1String("POSITION")))
        return Qt3DRender::QAttribute::defaultPositionAttributeName();
    if (semantic.startsWith(QLatin1String("NORMAL")))
        return Qt3DRender::QAttribute::defaultNormalAttributeName();
    if (semantic.startsWith(QLatin1String("TANGENT")))
        return Qt3DRender::QAttribute::defaultTangentAttributeName();
    if (semantic.startsWith(QLatin1String("TEXCOORD_0")))
        return Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName();
    if (semantic.startsWith(QLatin1String("TEXCOORD_1")))
        return Qt3DRender::QAttribute::defaultTextureCoordinate1AttributeName();
    if (semantic.startsWith(QLatin1String("TEXCOORD_2")))
        return Qt3DRender::QAttribute::defaultTextureCoordinate2AttributeName();
    if (semantic.startsWith(QLatin1String("COLOR_0")))
        return Qt3DRender::QAttribute::defaultColorAttributeName();
    if (semantic.startsWith(QLatin1String("JOINTS_0")))
        return Qt3DRender::QAttribute::defaultJointIndicesAttributeName();
    if (semantic.startsWith(QLatin1String("WEIGHTS_0")))
        return Qt3DRender::QAttribute::defaultJointWeightsAttributeName();

    return QString();
}

#if defined(KUESA_DRACO_COMPRESSION)
template<typename ValueType, Qt3DRender::QAttribute::VertexBaseType ComponentType>
Qt3DRender::QAttribute *decodeAttribute(const draco::PointCloud *pointCould,
                                        const draco::PointAttribute *dracoAttribute,
                                        QString attributeName)
{
    using namespace draco;

    const size_t components = static_cast<size_t>(dracoAttribute->num_components());
    const PointIndex::ValueType num_points = pointCould->num_points();
    const PointIndex::ValueType num_entries = num_points * static_cast<PointIndex::ValueType>(components);
    std::vector<ValueType> values(components);
    size_t entry_id = 0;

    QByteArray qbuffer;
    qbuffer.resize(static_cast<int>(sizeof(ValueType) * num_entries));
    ValueType *bufferData = reinterpret_cast<ValueType *>(qbuffer.data());
    for (PointIndex i(0); i < num_points; ++i) {
        if (!dracoAttribute->ConvertValue<ValueType>(dracoAttribute->mapped_index(i), &values[0]))
            return nullptr;
        for (size_t j = 0; j < components; ++j)
            bufferData[entry_id++] = values[j];
    }

    Qt3DRender::QBuffer *buffer = new Qt3DRender::QBuffer();
    buffer->setData(qbuffer);
    Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(buffer,
                                                                   attributeName,
                                                                   ComponentType,
                                                                   static_cast<uint>(components),
                                                                   num_points);

    return attribute;
}
#endif

} // namespace

MeshParser::MeshParser()
    : m_context(nullptr)
{
}

bool MeshParser::parse(const QJsonArray &meshArray, GLTF2Context *context)
{
    m_context = context;

    const qint32 meshSize = meshArray.size();
    QVector<Mesh> meshes;
    meshes.resize(meshSize);

    QElapsedTimer timer;
    timer.start();

    // Each mesh may contain several primitives, so we are storing each mesh as
    // an entity and several subentities, one for each primitive
    for (qint32 meshId = 0; meshId < meshSize; ++meshId) {
        Mesh &mesh = meshes[meshId];
        mesh.meshIdx = meshId;

        const QJsonObject &meshObject = meshArray[meshId].toObject();
        const QJsonArray &primitivesArray = meshObject.value(KEY_PRIMITIVES).toArray();

        const qint32 primitiveCount = primitivesArray.size();
        bool hasMorphTargets = false;

        if (primitiveCount == 0) {
            qWarning(kuesa) << "Mesh doesn't define any primitive";
            return false;
        }

        mesh.meshPrimitives.resize(primitiveCount);
        mesh.name = meshObject.value(KEY_NAME).toString();

        for (qint32 primitiveId = 0; primitiveId < primitiveCount; ++primitiveId) {
            Primitive &primitive = mesh.meshPrimitives[primitiveId];
            const QJsonObject &primitivesObject = primitivesArray[primitiveId].toObject();

            bool hasColorAttr = false;
            auto geometry = std::unique_ptr<Qt3DRender::QGeometry>(new Qt3DRender::QGeometry);

#if defined(KUESA_DRACO_COMPRESSION)
            const QJsonObject extensions = primitivesObject.value(KEY_EXTENSIONS).toObject();

            // Draco Extensions
            if (extensions.contains(KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION)) {
                if (!geometryDracoFromJSON(geometry.get(), primitivesObject, hasColorAttr) &&
                    geometry->attributes().isEmpty()) {
                    qCWarning(kuesa) << "Failed to parse draco compressed mesh primitive";
                    return false;
                }
            } else
#endif
            {
                if (!geometryFromJSON(geometry.get(), primitivesObject, hasColorAttr) &&
                    geometry->attributes().isEmpty()) {
                    qCWarning(kuesa) << "Failed to parse mesh primitive";
                    return false;
                }

                if (primitivesObject.contains(KEY_TARGETS)) {
                    hasMorphTargets = true;

                    bool morphTargetsAreValid = false;
                    QVector<MorphTarget> morphTargets;
                    std::tie(morphTargetsAreValid, morphTargets) =
                            geometryMorphTargetsFromJSON(geometry.get(),
                                                         primitivesObject);
                    if (!morphTargetsAreValid) {
                        qCWarning(kuesa) << "Failed to parse Mesh Primitive morph targets";
                        return false;
                    }

                    primitive.morphTargets = std::move(morphTargets);
                }
            }

            if (!Kuesa::GLTF2Import::MeshParserUtils::geometryIsGLTF2Valid(geometry.get())) {
                qCWarning(kuesa) << QLatin1String("Geometry doesn't meet glTF 2.0 requirements");
                return false;
            }

            auto primitiveType = static_cast<Qt3DRender::QGeometryRenderer::PrimitiveType>(primitivesObject.value(KEY_MODE).toInt(GL_TRIANGLES));
            if (m_context->options()->generateNormals()) {
                if (MeshParserUtils::needsNormalAttribute(geometry.get(), primitiveType)) {
                    Kuesa::GLTF2Import::MeshParserUtils::createNormalsForGeometry(geometry.get(), primitiveType);
                    // The generation of normal forces the primitive type to be Triangles
                    primitiveType = Qt3DRender::QGeometryRenderer::Triangles;
                }
            }
            if (m_context->options()->generateTangents()) {
                if (MeshParserUtils::needsTangentAttribute(geometry.get(), primitiveType))
                    Kuesa::GLTF2Import::MeshParserUtils::createTangentForGeometry(geometry.get(), primitiveType);
            }

            Qt3DRender::QGeometryRenderer *renderer = new Qt3DRender::QGeometryRenderer;
            renderer->setPrimitiveType(primitiveType);
            renderer->setGeometry(geometry.release());
            primitive.primitiveRenderer = renderer;
            primitive.materialIdx = primitivesObject.value(KEY_MATERIAL).toInt(-1);
            primitive.hasColorAttr = hasColorAttr;
        }

        // All primitives in a mesh are required to declare the same number of
        // morph targets and they should have the same layout (so that we can
        // use the same shader for all primitives in a mesh)
        if (hasMorphTargets) {
            // Initialize count of morph targets
            const Primitive &firstPrimitive = mesh.meshPrimitives.first();
            // All primitives are supposed to have the same number of morph targets
            mesh.morphTargetCount = quint8(firstPrimitive.morphTargets.size());

            for (int i = 1, m = mesh.meshPrimitives.size(); i < m; ++i) {
                const Primitive &previousPrimitive = mesh.meshPrimitives.at(i - 1);
                const Primitive &currentPrimitive = mesh.meshPrimitives.at(i);
                const QVector<MorphTarget> previousPrimitiveMorphTargets = previousPrimitive.morphTargets;
                const QVector<MorphTarget> currentPrimitiveMorphTargets = currentPrimitive.morphTargets;

                if (previousPrimitiveMorphTargets.size() != currentPrimitiveMorphTargets.size()) {
                    qWarning(kuesa) << "Mesh primitives should all define the same number of morph targets";
                    return false;
                }

                // Check that the first MorphTargets of each primitive define the same attributes
                // Note: we already check that within a Primitive, all MorphTargets define are compatible

                const MorphTarget firstPreviousMorphTarget = previousPrimitiveMorphTargets.first();
                const MorphTarget firstCurrentMorphTarget = currentPrimitiveMorphTargets.first();

                if (firstPreviousMorphTarget.attributes.size() != firstCurrentMorphTarget.attributes.size()) {
                    qWarning(kuesa) << "Morph target attribute counts mismatch between different primitives of the same mesh";
                    return false;
                }

                // Check we define attributes with the same names between MorphTargets of different primitives
                for (const MorphTargetAttribute &currentMorphTargetAttr : firstCurrentMorphTarget.attributes) {
                    bool matchingAttributeNameFound = false;
                    for (const MorphTargetAttribute &previousMorphTargetAttr : firstPreviousMorphTarget.attributes) {
                        if (currentMorphTargetAttr.name == previousMorphTargetAttr.name) {
                            matchingAttributeNameFound = true;
                            break;
                        }
                    }
                    if (!matchingAttributeNameFound) {
                        qWarning(kuesa) << "Morph target attribute name mismatch between different primitives of the same mesh";
                        return false;
                    }
                }
            }

            const QJsonArray weightsArray = meshObject.value(KEY_WEIGHTS).toArray();

            if (weightsArray.isEmpty()) {
                // Fill default weights with 0.0 value
                mesh.morphTargetWeights.resize(mesh.morphTargetCount);
                std::fill(mesh.morphTargetWeights.begin(), mesh.morphTargetWeights.end(), 0.0f);
            } else {
                if (weightsArray.size() != mesh.morphTargetCount) {
                    qWarning(kuesa) << "Mesh has defined"
                                    << mesh.morphTargetCount
                                    << "morph targets but"
                                    << weightsArray.size()
                                    << "default weights were specified";
                    return false;
                }
                mesh.morphTargetWeights.reserve(mesh.morphTargetCount);
                for (const QJsonValue &v : weightsArray)
                    mesh.morphTargetWeights.push_back(float(v.toDouble(0.0)));
            }
        }

        context->addMesh(mesh);
    }

    qCDebug(kuesa) << "Loading mesh took" << timer.elapsed() << "milliseconds";

    return meshSize > 0;
}

bool MeshParser::geometryFromJSON(Qt3DRender::QGeometry *geometry,
                                  const QJsonObject &json,
                                  bool &hasColorAttr)
{
    // Parse vertex attributes
    if (!geometryAttributesFromJSON(geometry, json, {}, hasColorAttr))
        return false;

    // Index attribute
    const QJsonValue &indices = json.value(KEY_INDICES);
    if (!indices.isUndefined()) {
        const qint32 accessorIndex = indices.toInt();
        const Accessor &accessor = m_context->accessor(accessorIndex);
        const BufferView &viewData = m_context->bufferView(accessor.bufferViewIndex);
        const int byteStride = (!viewData.bufferData.isEmpty() && viewData.byteStride > 0 ? viewData.byteStride : 0);

        Qt3DRender::QBuffer *buffer = nullptr;
        if (accessor.sparseCount) {
            buffer = m_qAccessorBuffers.value(accessorIndex, nullptr);
            if (buffer == nullptr) {
                buffer = new Qt3DRender::QBuffer;
                buffer->setData(accessor.bufferData);
                m_qAccessorBuffers.insert(accessorIndex, buffer);
            }
        } else {
            buffer = m_qViewBuffers.value(accessor.bufferViewIndex, nullptr);
            if (buffer == nullptr) {
                buffer = new Qt3DRender::QBuffer;
                buffer->setData(accessor.bufferData);
                m_qViewBuffers.insert(accessor.bufferViewIndex, buffer);
            }
        }

        Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(buffer,
                                                                       accessor.type,
                                                                       accessor.dataSize,
                                                                       accessor.count,
                                                                       accessor.offset,
                                                                       static_cast<uint>(byteStride));
        attribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        // store some GLTF metadata for glTF editor
        attribute->setProperty("bufferIndex", viewData.bufferIdx);
        attribute->setProperty("bufferViewIndex", accessor.bufferViewIndex);
        attribute->setProperty("bufferViewOffset", viewData.byteOffset);
        attribute->setProperty("bufferName", accessor.name);
        geometry->addAttribute(attribute);
    }
    return true;
}

std::tuple<bool, QVector<MorphTarget>>
MeshParser::geometryMorphTargetsFromJSON(Qt3DRender::QGeometry *geometry,
                                         const QJsonObject &json)
{
    const QJsonArray &morphTargetsJsonArray = json.value(KEY_TARGETS).toArray();

    const auto morphAttribNamesCbeg = std::begin(morphTargetAttributeNames);
    const auto morphAttribNamesCend = std::end(morphTargetAttributeNames);
    const QVector<Qt3DRender::QAttribute *> primitiveAttributes =
            geometry->attributes();

    QVector<MorphTarget> morphTargets;
    morphTargets.reserve(morphTargetsJsonArray.size());

    for (const QJsonValue &morphTargetJsonValue : morphTargetsJsonArray) {
        const QJsonObject morphTargetJsonObj = morphTargetJsonValue.toObject();

        MorphTarget morphTarget;
        morphTarget.attributes.reserve(morphTargetJsonObj.size());

        // Technically GLTF has no limitation on the maximum number of
        // morpth targets to support. It states that a conformant implementation
        // allows to have 8 morph targets with a single attribute, 4 morph target
        // with 2 attributes, 2 morph targets that have 3 attributes ...

        auto it = morphTargetJsonObj.constBegin();
        const auto end = morphTargetJsonObj.constEnd();
        while (it != end) {
            const qint32 morphTargetAttributeAccessorIdx = it.value().toInt(-1);
            const QString morphTargetAttributeName = it.key();

            if (morphTargetAttributeAccessorIdx < 0) {
                qWarning(kuesa) << "Morph target attribute"
                                << morphTargetAttributeName
                                << "references an invalid accessor";
                return std::make_tuple(false, morphTargets);
            }

            // Currently we will only handle POSITION/TANGENT/NORMAL
            // technically custom attributes starting with _ATTRIBUTE_NAME
            // are allowed but wouldn't make sense in our use case
            if (std::find(morphAttribNamesCbeg, morphAttribNamesCend,
                          morphTargetAttributeName) == morphAttribNamesCend) {
                qWarning(kuesa) << morphTargetAttributeName
                                << "isn't a valid morph target attribute";
                return std::make_tuple(false, morphTargets);
            }

            // Check that we have an attribute Name that exists in the geometry
            const QString standardAttributeName =
                    standardAttributeNameFromSemantic(morphTargetAttributeName);

            Qt3DRender::QAttribute *referenceAttributeInPrimitive = nullptr;
            for (Qt3DRender::QAttribute *attribute : primitiveAttributes) {
                if (attribute->name() == standardAttributeName) {
                    referenceAttributeInPrimitive = attribute;
                    break;
                }
            }

            if (referenceAttributeInPrimitive == nullptr) {
                qWarning(kuesa) << "Morph target attribute" << morphTargetAttributeName
                                << "isn't an attribute referenced in the primitive";
                return std::make_tuple(false, morphTargets);
            }

            // The accessor count must match that of the primitive's attribute
            // accessor count
            const Accessor &accessor =
                    m_context->accessor(morphTargetAttributeAccessorIdx);
            if (referenceAttributeInPrimitive->count() != accessor.count) {
                qWarning(kuesa) << "Morph target attribute" << morphTargetAttributeName
                                << " count doesn't match count for the reference "
                                   "attribute in the primitive";
                return std::make_tuple(false, morphTargets);
            }

            // Record Morph Target Attribute
            MorphTargetAttribute morphTargetAttribute;
            morphTargetAttribute.name = morphTargetAttributeName;
            morphTargetAttribute.accessorIdx = morphTargetAttributeAccessorIdx;
            morphTarget.attributes.push_back(morphTargetAttribute);

            ++it;
        }

        if (!morphTargets.empty()) {
            // Compare the morphTarget we are about to insert against the last
            // inserted to make sure they have a similar layout

            const MorphTarget &lastInsertedMorphTarget = morphTargets.last();

            if (lastInsertedMorphTarget.attributes.size() !=
                morphTarget.attributes.size()) {
                qWarning(kuesa) << "Morph targets attribute count mismatch, all "
                                   "targets should define the same attributes";
                return std::make_tuple(false, morphTargets);
            }

            for (const MorphTargetAttribute &currentMorphTargetAttr : lastInsertedMorphTarget.attributes) {
                bool matchingAttributeNameFound = false;
                for (const MorphTargetAttribute &previousMorphTargetAttr : morphTarget.attributes) {
                    if (currentMorphTargetAttr.name == previousMorphTargetAttr.name) {
                        matchingAttributeNameFound = true;
                        break;
                    }
                }
                if (!matchingAttributeNameFound) {
                    qWarning(kuesa) << "Morph target attribute name, all "
                                       "targets should define the same "
                                       "attributes names";
                    return std::make_tuple(false, morphTargets);
                }
            }
        }

        // Record Morph Target
        morphTargets.push_back(morphTarget);
    }

    // Add the additional morph target attributes to the geometry
    for (int i = 0, m = morphTargets.size(); i < m; ++i) {
        const MorphTarget &morphTarget = morphTargets.at(i);
        for (const MorphTargetAttribute &morphTargetAttribute : morphTarget.attributes) {
            const QString semanticName = morphTargetAttribute.name;
            const QString attributeName = QStringLiteral("%1_%2")
                                                  .arg(standardAttributeNameFromSemantic(semanticName))
                                                  .arg(i + 1);
            Qt3DRender::QAttribute *attribute = createAttribute(morphTargetAttribute.accessorIdx,
                                                                attributeName,
                                                                semanticName);
            geometry->addAttribute(attribute);
        }
    }
    return std::make_tuple(true, morphTargets);
}

bool MeshParser::geometryAttributesFromJSON(Qt3DRender::QGeometry *geometry,
                                            const QJsonObject &json,
                                            QStringList existingAttributes,
                                            bool &hasColorAttr)
{
    const QJsonObject &attrs = json.value(KEY_ATTRIBUTES).toObject();

    if (attrs.size() == 0) {
        qCWarning(kuesa) << "Mesh primitive doesn't define any attribute";
        return false;
    }

    for (auto it = attrs.begin(), end = attrs.end(); it != end; ++it) {
        const QString attrName = it.key();
        QString attributeName = standardAttributeNameFromSemantic(attrName);
        if (attributeName.isEmpty())
            attributeName = attrName;

        if (existingAttributes.contains(attributeName))
            continue;

        if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
            hasColorAttr = true;

        const qint32 accessorIdx = it.value().toInt(-1);
        Qt3DRender::QAttribute *attribute = createAttribute(accessorIdx,
                                                            attributeName,
                                                            attrName);
        geometry->addAttribute(attribute);
    }

    return true;
}

Qt3DRender::QAttribute *MeshParser::createAttribute(qint32 accessorIndex,
                                                    const QString &attributeName,
                                                    const QString &semanticName)
{
    const Accessor &accessor = m_context->accessor(accessorIndex);

    quint32 byteStride = 0;
    quint32 byteOffset = 0;
    qint32 bufferIdx = -1;

    if (accessor.bufferViewIndex >= 0) {
        const BufferView &viewData = m_context->bufferView(accessor.bufferViewIndex);
        byteStride = static_cast<quint32>(!viewData.bufferData.isEmpty() && viewData.byteStride > 0 ? viewData.byteStride : 0);
        byteOffset = static_cast<quint32>(viewData.byteOffset);
        bufferIdx = viewData.bufferIdx;
    }

    Qt3DRender::QBuffer *buffer = nullptr;
    if (accessor.sparseCount) {
        buffer = m_qAccessorBuffers.value(accessorIndex, nullptr);
        if (buffer == nullptr) {
            buffer = new Qt3DRender::QBuffer;
            buffer->setData(accessor.bufferData);
            m_qAccessorBuffers.insert(accessor.bufferViewIndex, buffer);
        }
    } else {
        buffer = m_qViewBuffers.value(accessor.bufferViewIndex, nullptr);
        if (buffer == nullptr) {
            buffer = new Qt3DRender::QBuffer;
            buffer->setData(accessor.bufferData);
            m_qViewBuffers.insert(accessor.bufferViewIndex, buffer);
        }
    }

    Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(buffer,
                                                                   attributeName,
                                                                   accessor.type,
                                                                   accessor.dataSize,
                                                                   quint32(accessor.count),
                                                                   quint32(accessor.offset),
                                                                   byteStride);
    attribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    // store some GLTF metadata for glTF editor
    attribute->setProperty("bufferIndex", bufferIdx);
    attribute->setProperty("bufferViewIndex", accessor.bufferViewIndex);
    attribute->setProperty("bufferViewOffset", byteOffset);
    attribute->setProperty("bufferName", accessor.name);
    attribute->setProperty("semanticName", semanticName);

    return attribute;
}

#if defined(KUESA_DRACO_COMPRESSION)
bool MeshParser::geometryDracoFromJSON(Qt3DRender::QGeometry *geometry,
                                       const QJsonObject &json,
                                       bool &hasColorAttr)
{
    const QJsonObject extensions = json.value(KEY_EXTENSIONS).toObject();
    const QJsonObject dracoExtensionObject = extensions.value(KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION).toObject();

    // Get the compressed data
    qint32 bufferViewIndex = dracoExtensionObject.value(KEY_BUFFERVIEW).toInt(-1);
    if (bufferViewIndex == -1) {
        qCWarning(kuesa) << "Draco extension referencing invalid buffer view";
        return false;
    }

    const BufferView &viewData = m_context->bufferView(bufferViewIndex);
    draco::DecoderBuffer dBuffer;
    dBuffer.Init(viewData.bufferData.constData(), static_cast<size_t>(viewData.bufferData.size()));

    // Check data
    const draco::StatusOr<draco::EncodedGeometryType> geom_type = draco::Decoder::GetEncodedGeometryType(&dBuffer);
    if (!geom_type.ok()) {
        qCWarning(kuesa) << geom_type.status().error_msg();
        return false;
    }

    if (geom_type.value() != draco::TRIANGULAR_MESH && geom_type.value() != draco::POINT_CLOUD) {
        qCWarning(kuesa) << QLatin1String("Draco data is not a mesh nor a point cloud");
        return false;
    }

    // Decompress
    draco::Decoder decoder;
    std::unique_ptr<draco::PointCloud> geometryData;

    // Draco supports triangular meshes or point clouds
    if (geom_type.value() == draco::TRIANGULAR_MESH)
        geometryData = decoder.DecodeMeshFromBuffer(&dBuffer).value();
    else if (geom_type.value() == draco::POINT_CLOUD)
        geometryData = decoder.DecodePointCloudFromBuffer(&dBuffer).value();

    if (!geometryData) {
        qCWarning(kuesa) << "Failed to decode Draco geometry";
        return false;
    }

    QStringList existingAttributes;

    // Parse draco vertex attributes
    if (!geometryAttributesDracoFromJSON(geometry,
                                         json,
                                         geometryData.get(),
                                         existingAttributes,
                                         hasColorAttr))
        return false;

    // Create Index attribute if we are dealing with a triangular mesh
    if (geom_type.value() == draco::TRIANGULAR_MESH) {
        draco::Mesh *mesh = static_cast<draco::Mesh *>(geometryData.get());
        QByteArray qbuffer;
        qbuffer.resize(static_cast<int>(3 * sizeof(GLuint) * mesh->num_faces()));
        GLuint *bufferData = reinterpret_cast<GLuint *>(qbuffer.data());
        for (uint32_t i = 0; i < mesh->num_faces(); ++i) {
            const auto &face = mesh->face(draco::FaceIndex(i));
            bufferData[i * 3 + 0] = face[0].value();
            bufferData[i * 3 + 1] = face[1].value();
            bufferData[i * 3 + 2] = face[2].value();
        }

        Qt3DRender::QBuffer *buffer = new Qt3DRender::QBuffer();
        buffer->setData(qbuffer);
        Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(buffer,
                                                                       Qt3DRender::QAttribute::UnsignedInt,
                                                                       1,
                                                                       mesh->num_faces() * 3,
                                                                       0,
                                                                       0);
        attribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        geometry->addAttribute(attribute);
    }

    // Parse any additional non draco vertex attributes that may be present
    const QJsonObject &attrs = json.value(KEY_ATTRIBUTES).toObject();
    if (attrs.size() != 0)
        return geometryAttributesFromJSON(geometry, json, existingAttributes, hasColorAttr);

    return true;
}

bool MeshParser::geometryAttributesDracoFromJSON(Qt3DRender::QGeometry *geometry,
                                                 const QJsonObject &json,
                                                 const draco::PointCloud *pointCloud,
                                                 QStringList &existingAttributes,
                                                 bool &hasColorAttr)
{
    const QJsonObject extensions = json.value(KEY_EXTENSIONS).toObject();
    const QJsonObject dracoExtensionObject = extensions.value(KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION).toObject();
    const QJsonObject attrs = json.value(KEY_ATTRIBUTES).toObject();
    const QJsonObject dracoAttrs = dracoExtensionObject.value(KEY_ATTRIBUTES).toObject();

    if (attrs.size() == 0 || dracoAttrs.size() == 0) {
        qCWarning(kuesa) << "Draco primitive not referencing any attributes";
        return false;
    }

    existingAttributes.reserve(attrs.size());
    for (auto it = attrs.begin(), end = attrs.end(); it != end; ++it) {

        const QString attrName = it.key();
        QString attributeName = standardAttributeNameFromSemantic(attrName);
        if (attributeName.isEmpty())
            attributeName = attrName;

        if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
            hasColorAttr = true;

        existingAttributes << attributeName;

        // Get Draco attribute
        Qt3DRender::QAttribute *attribute = nullptr;
        const int attributeId = dracoAttrs.value(attrName).toInt(-1);
        const draco::PointAttribute *dracoAttribute = attributeId < 0 ? nullptr : pointCloud->GetAttributeByUniqueId(static_cast<uint32_t>(attributeId));
        if (dracoAttribute) {
            switch (dracoAttribute->data_type()) {
            case draco::DT_INT8:
                attribute = decodeAttribute<qint8, Qt3DRender::QAttribute::Byte>(pointCloud, dracoAttribute, attributeName);
                break;
            case draco::DT_UINT8:
                attribute = decodeAttribute<quint8, Qt3DRender::QAttribute::UnsignedByte>(pointCloud, dracoAttribute, attributeName);
                break;
            case draco::DT_INT16:
                attribute = decodeAttribute<qint16, Qt3DRender::QAttribute::Short>(pointCloud, dracoAttribute, attributeName);
                break;
            case draco::DT_UINT16:
                attribute = decodeAttribute<quint16, Qt3DRender::QAttribute::UnsignedShort>(pointCloud, dracoAttribute, attributeName);
                break;
            case draco::DT_INT32:
                attribute = decodeAttribute<qint32, Qt3DRender::QAttribute::Int>(pointCloud, dracoAttribute, attributeName);
                break;
            case draco::DT_UINT32:
                attribute = decodeAttribute<quint32, Qt3DRender::QAttribute::UnsignedInt>(pointCloud, dracoAttribute, attributeName);
                break;
            case draco::DT_FLOAT32:
                attribute = decodeAttribute<float, Qt3DRender::QAttribute::Float>(pointCloud, dracoAttribute, attributeName);
                break;
            case draco::DT_FLOAT64:
                attribute = decodeAttribute<double, Qt3DRender::QAttribute::Double>(pointCloud, dracoAttribute, attributeName);
                break;
            default:
                qCWarning(kuesa) << "unsupported data type:" << dracoAttribute->data_type();
                break;
            }
        } else {
            qCWarning(kuesa) << "Failed to parse draco attribute";
            return false;
        }

        const int accessorIndex = it.value().toInt();
        const Accessor accessor = m_context->accessor(accessorIndex);

        attribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        // store some GLTF metadata for glTF editor
        attribute->setProperty("bufferIndex", -1);
        attribute->setProperty("bufferViewIndex", accessor.bufferViewIndex);
        attribute->setProperty("bufferViewOffset", accessor.offset);
        attribute->setProperty("bufferName", accessor.name);
        attribute->setProperty("semanticName", attrName);

        geometry->addAttribute(attribute);
    }
    return true;
}
#endif

QT_END_NAMESPACE
