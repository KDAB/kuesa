/*
    meshparser.cpp

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

#include "meshparser_p.h"
#include "bufferviewsparser_p.h"
#include "gltf2context_p.h"
#include "kuesa_p.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QElapsedTimer>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <QtGui/qopengl.h>

#if defined(KUESA_DRACO_COMPRESSION)
#include <draco/compression/decode.h>
#endif

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

const QLatin1String KEY_PRIMITIVES = QLatin1Literal("primitives");
const QLatin1String KEY_ATTRIBUTES = QLatin1Literal("attributes");
const QLatin1String KEY_INDICES = QLatin1Literal("indices");
const QLatin1String KEY_MATERIAL = QLatin1Literal("material");
const QLatin1String KEY_MODE = QLatin1Literal("mode");
const QLatin1String KEY_NAME = QLatin1Literal("name");
#if defined(KUESA_DRACO_COMPRESSION)
const QLatin1String KEY_EXTENSIONS = QLatin1String("extensions");
const QLatin1String KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION = QLatin1String("KHR_draco_mesh_compression");
const QLatin1String KEY_BUFFERVIEW = QLatin1String("bufferView");
#endif

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
        return QString();
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
        const AttributeValueIndex val_index = dracoAttribute->mapped_index(i);
        if (!dracoAttribute->ConvertValue<ValueType>(val_index, &values[0]))
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

    const int meshSize = meshArray.size();
    QVector<Mesh> meshes;
    meshes.resize(meshSize);

    QElapsedTimer timer;
    timer.start();

    // Each mesh may contain several primitives, so we are storing each mesh as
    // an entity and several subentities, one for each primitive
    for (int meshId = 0; meshId < meshSize; ++meshId) {
        Mesh &mesh = meshes[meshId];
        const QJsonObject &meshObject = meshArray[meshId].toObject();
        const QJsonArray &primitivesArray = meshObject.value(KEY_PRIMITIVES).toArray();

        const int primitiveCount = primitivesArray.size();

        if (primitiveCount == 0)
            return false;

        mesh.meshPrimitives.resize(primitiveCount);
        mesh.name = meshObject.value(KEY_NAME).toString();
        for (int primitiveId = 0; primitiveId < primitiveCount; ++primitiveId) {
            Primitive &primitive = mesh.meshPrimitives[primitiveId];
            const QJsonObject &primitivesObject = primitivesArray[primitiveId].toObject();

            bool hasColorAttr = false;
            Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();

#if defined(KUESA_DRACO_COMPRESSION)
            const QJsonObject extensions = primitivesObject.value(KEY_EXTENSIONS).toObject();

            // Draco Extensions
            if (extensions.contains(KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION)) {
                if (!geometryDracoFromJSON(geometry, primitivesObject, hasColorAttr) &&
                    geometry->attributes().isEmpty()) {
                    delete geometry;
                    return false;
                }
            } else
#endif
            {
                if (!geometryFromJSON(geometry, primitivesObject, hasColorAttr) &&
                    geometry->attributes().isEmpty()) {
                    delete geometry;
                    return false;
                }
            }

            Qt3DRender::QGeometryRenderer *renderer = new Qt3DRender::QGeometryRenderer;
            renderer->setPrimitiveType(static_cast<Qt3DRender::QGeometryRenderer::PrimitiveType>(primitivesObject.value(KEY_MODE).toInt(GL_TRIANGLES)));
            renderer->setGeometry(geometry);
            primitive.primitiveRenderer = renderer;
            primitive.materialIdx = primitivesObject.value(KEY_MATERIAL).toInt(-1);
            primitive.hasColorAttr = hasColorAttr;
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
        const int accessorIndex = indices.toInt();
        const Accessor &accessor = m_context->accessor(accessorIndex);
        const BufferView &viewData = m_context->bufferView(accessor.bufferViewIndex);

        auto *buffer = m_qbuffers.value(accessor.bufferViewIndex, nullptr);
        if (buffer == nullptr) {
            buffer = new Qt3DRender::QBuffer;
            buffer->setData(viewData.bufferData);
            m_qbuffers.insert(accessor.bufferViewIndex, buffer);
        }

        Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(buffer,
                                                                       accessor.type,
                                                                       accessor.dataSize,
                                                                       accessor.count,
                                                                       accessor.offset,
                                                                       viewData.byteStride);
        attribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        // store some GLTF metadata for asset pipeline editor
        attribute->setProperty("bufferIndex", viewData.bufferIdx);
        attribute->setProperty("bufferViewIndex", accessor.bufferViewIndex);
        attribute->setProperty("bufferViewOffset", viewData.byteOffset);
        attribute->setProperty("bufferName", accessor.name);
        geometry->addAttribute(attribute);
    }
    return true;
}

bool MeshParser::geometryAttributesFromJSON(Qt3DRender::QGeometry *geometry,
                                            const QJsonObject &json,
                                            QStringList existingAttributes,
                                            bool &hasColorAttr)
{
    const QJsonObject &attrs = json.value(KEY_ATTRIBUTES).toObject();

    if (attrs.size() == 0)
        return false;

    for (auto it = attrs.begin(), end = attrs.end(); it != end; ++it) {
        const int accessorIndex = it.value().toInt();
        const Accessor &accessor = m_context->accessor(accessorIndex);

        const QString attrName = it.key();
        QString attributeName = standardAttributeNameFromSemantic(attrName);
        if (attributeName.isEmpty())
            attributeName = attrName;

        if (existingAttributes.contains(attributeName))
            continue;

        if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
            hasColorAttr = true;

        const BufferView &viewData = m_context->bufferView(accessor.bufferViewIndex);

        auto *buffer = m_qbuffers.value(accessor.bufferViewIndex, nullptr);
        if (buffer == nullptr) {
            buffer = new Qt3DRender::QBuffer;
            buffer->setData(viewData.bufferData);
            m_qbuffers.insert(accessor.bufferViewIndex, buffer);
        }

        Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(buffer,
                                                                       attributeName,
                                                                       accessor.type,
                                                                       accessor.dataSize,
                                                                       accessor.count,
                                                                       accessor.offset,
                                                                       viewData.byteStride);
        attribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        // store some GLTF metadata for asset pipeline editor
        attribute->setProperty("bufferIndex", viewData.bufferIdx);
        attribute->setProperty("bufferViewIndex", accessor.bufferViewIndex);
        attribute->setProperty("bufferViewOffset", viewData.byteOffset);
        attribute->setProperty("bufferName", accessor.name);
        geometry->addAttribute(attribute);
    }

    return true;
}

#if defined(KUESA_DRACO_COMPRESSION)
bool MeshParser::geometryDracoFromJSON(Qt3DRender::QGeometry *geometry,
                                       const QJsonObject &json,
                                       bool &hasColorAttr)
{
    const QJsonObject extensions = json.value(KEY_EXTENSIONS).toObject();
    const QJsonObject dracoExtensionObject = extensions.value(KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION).toObject();

    // Get the compressed data
    int bufferViewIndex = dracoExtensionObject.value(KEY_BUFFERVIEW).toInt(-1);
    if (bufferViewIndex == -1)
        return false;

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
        qCWarning(kuesa) << QLatin1Literal("Draco data is not a mesh nor a point cloud");
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
    const QJsonObject &attrs = json.value(KEY_ATTRIBUTES).toObject();
    const QJsonObject &dracoAttrs = dracoExtensionObject.value(KEY_ATTRIBUTES).toObject();

    if (attrs.size() == 0 || dracoAttrs.size() == 0)
        return false;

    existingAttributes.reserve(attrs.size());
    for (auto it = attrs.begin(), end = attrs.end(); it != end; ++it) {
        const int accessorIndex = it.value().toInt();
        const Accessor &accessor = m_context->accessor(accessorIndex);
        const BufferView &viewData = m_context->bufferView(accessor.bufferViewIndex);

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
                qDebug() << "unsupported data type:" << dracoAttribute->data_type();
                break;
            }
        } else {
            return false;
        }

        attribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        // store some GLTF metadata for asset pipeline editor
        attribute->setProperty("bufferIndex", viewData.bufferIdx);
        attribute->setProperty("bufferViewIndex", accessor.bufferViewIndex);
        attribute->setProperty("bufferViewOffset", accessor.offset);
        attribute->setProperty("bufferName", accessor.name);

        geometry->addAttribute(attribute);
    }
    return true;
}
#endif

QT_END_NAMESPACE
