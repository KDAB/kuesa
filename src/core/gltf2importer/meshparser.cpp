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

#if defined(KUESA_DRACO_COMPRESSION)
#include <draco/compression/decode.h>
#endif

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

#include "mikktspace/mikktspace.h"

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

std::vector<Qt3DRender::QAttribute::VertexBaseType> validVertexBaseTypesForAttribute(const QString &attributeName)
{
    // Standard Attributes
    if (attributeName == Qt3DRender::QAttribute::defaultPositionAttributeName())
        return { Qt3DRender::QAttribute::Float };
    if (attributeName == Qt3DRender::QAttribute::defaultNormalAttributeName())
        return { Qt3DRender::QAttribute::Float };
    if (attributeName == Qt3DRender::QAttribute::defaultTangentAttributeName())
        return { Qt3DRender::QAttribute::Float };
    if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName())
        return { Qt3DRender::QAttribute::Float,
                 Qt3DRender::QAttribute::UnsignedByte,
                 Qt3DRender::QAttribute::UnsignedShort };
    if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinate1AttributeName())
        return { Qt3DRender::QAttribute::Float,
                 Qt3DRender::QAttribute::UnsignedByte,
                 Qt3DRender::QAttribute::UnsignedShort };
    if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
        return { Qt3DRender::QAttribute::Float,
                 Qt3DRender::QAttribute::UnsignedByte,
                 Qt3DRender::QAttribute::UnsignedShort };
    if (attributeName == Qt3DRender::QAttribute::defaultJointIndicesAttributeName())
        return { Qt3DRender::QAttribute::UnsignedByte,
                 Qt3DRender::QAttribute::UnsignedShort };
    if (attributeName == Qt3DRender::QAttribute::defaultJointWeightsAttributeName())
        return { Qt3DRender::QAttribute::Float,
                 Qt3DRender::QAttribute::UnsignedByte,
                 Qt3DRender::QAttribute::UnsignedShort };

    return { Qt3DRender::QAttribute::UnsignedByte,
             Qt3DRender::QAttribute::UnsignedShort,
             Qt3DRender::QAttribute::UnsignedInt };
}

std::vector<uint> validVertexSizesForAttribute(const QString &attributeName)
{
    // Standard Attributes
    if (attributeName == Qt3DRender::QAttribute::defaultPositionAttributeName())
        return { 3 };
    if (attributeName == Qt3DRender::QAttribute::defaultNormalAttributeName())
        return { 3 };
    if (attributeName == Qt3DRender::QAttribute::defaultTangentAttributeName())
        return { 4 };
    if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName())
        return { 2 };
    if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinate1AttributeName())
        return { 2 };
    if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
        return { 3, 4 };
    if (attributeName == Qt3DRender::QAttribute::defaultJointIndicesAttributeName())
        return { 4 };
    if (attributeName == Qt3DRender::QAttribute::defaultJointWeightsAttributeName())
        return { 4 };

    return { 1 };
}

bool geometryIsGLTF2Valid(Qt3DRender::QGeometry *geometry)
{
    const auto &attributes = geometry->attributes();
    const bool vertexBaseTypesAreValid = std::find_if(std::begin(attributes), std::end(attributes),
                                                      [](Qt3DRender::QAttribute *attribute) {
                                                          const auto validVertexBaseTypes = validVertexBaseTypesForAttribute(attribute->name());
                                                          const auto vertexBaseType = attribute->vertexBaseType();
                                                          return std::find_if(std::begin(validVertexBaseTypes),
                                                                              std::end(validVertexBaseTypes),
                                                                              [vertexBaseType](Qt3DRender::QAttribute::VertexBaseType validVertexBaseType) {
                                                                                  return vertexBaseType == validVertexBaseType;
                                                                              }) == std::end(validVertexBaseTypes);
                                                      }) == std::end(attributes);

    const bool vertexSizesAreValid = std::find_if(std::begin(attributes), std::end(attributes),
                                                  [](Qt3DRender::QAttribute *attribute) {
                                                      const auto validVertexSizes = validVertexSizesForAttribute(attribute->name());
                                                      const auto vertexSize = attribute->vertexSize();
                                                      return std::find_if(std::begin(validVertexSizes),
                                                                          std::end(validVertexSizes),
                                                                          [vertexSize](uint validVertexSize) {
                                                                              return vertexSize == validVertexSize;
                                                                          }) == std::end(validVertexSizes);
                                                  }) == std::end(attributes);

    return vertexSizesAreValid && vertexBaseTypesAreValid;
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

int vertexBaseTypeSize(Qt3DRender::QAttribute::VertexBaseType vertexBaseType)
{
    switch (vertexBaseType) {
    case Qt3DRender::QAttribute::UnsignedByte:
        return 1;
    case Qt3DRender::QAttribute::UnsignedShort:
        return 2;
    case Qt3DRender::QAttribute::UnsignedInt:
        return 4;
    case Qt3DRender::QAttribute::Float:
        return 4;
    default:
        Q_UNREACHABLE();
    }
}

struct Attribute {
    QByteArray bufferData;
    unsigned int byteOffset;
    unsigned int byteStride;
};

// Chances to visit the same face again are high, so cache the state of the last search
struct FindVertexIndicesInFaceHelper {

    FindVertexIndicesInFaceHelper()
        : lastFace(-1)
    {
    }

    const std::array<unsigned int, 3> &operator()(int iFace) const
    {
        if (iFace == lastFace)
            return verticesForLastFace;

        lastFace = iFace;
        const auto data = indexAttribute.bufferData;
        const auto iFaceHead = data.mid(indexAttribute.byteOffset + iFace * indexAttribute.byteStride);
        switch (vertexBaseType) {
        case Qt3DRender::QAttribute::UnsignedByte: {
            const auto *typedIndices = reinterpret_cast<const unsigned char *>(iFaceHead.data());
            verticesForLastFace = { typedIndices[0], typedIndices[1], typedIndices[2] };
            break;
        }
        case Qt3DRender::QAttribute::UnsignedShort: {
            const auto *typedIndices = reinterpret_cast<const unsigned short *>(iFaceHead.data());
            verticesForLastFace = { typedIndices[0], typedIndices[1], typedIndices[2] };
            break;
        }
        case Qt3DRender::QAttribute::UnsignedInt: {
            const auto *typedIndices = reinterpret_cast<const unsigned int *>(iFaceHead.data());
            verticesForLastFace = { typedIndices[0], typedIndices[1], typedIndices[2] };
            break;
        }
        default:
            Q_UNREACHABLE();
        }
        return verticesForLastFace;
    }

    Attribute indexAttribute;
    Qt3DRender::QAttribute::VertexBaseType vertexBaseType;
    mutable int lastFace;
    mutable std::array<unsigned int, 3> verticesForLastFace;
};

struct MikkTSpaceUserData {
    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;

    Attribute indexAttribute;
    Attribute positionAttribute;
    Attribute uvAttribute;
    Attribute normalAttribute;

    int nFaces;
    int nVertices;
    Qt3DRender::QAttribute::VertexBaseType vertexBaseTypeForUVAttribute;

    FindVertexIndicesInFaceHelper vertexIndicesFinder;
    QByteArray tangentBufferData;
};

MikkTSpaceUserData precomputeMikkTSpaceUserData(Qt3DRender::QGeometry *geometry, Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType)
{
    // Precompute some data
    MikkTSpaceUserData userData;

    const auto &attributes = geometry->attributes();
    for (const auto attr : attributes) {
        if (attr->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
            userData.indexAttribute.byteOffset = attr->byteOffset();
            constexpr auto NumberVerticesInFace = 3;
            userData.indexAttribute.byteStride = attr->byteStride() == 0 ? NumberVerticesInFace * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.indexAttribute.bufferData = attr->buffer()->data();
            userData.vertexIndicesFinder.vertexBaseType = attr->vertexBaseType();
            userData.vertexIndicesFinder.indexAttribute = userData.indexAttribute;
            switch (primitiveType) {
            case Qt3DRender::QGeometryRenderer::Triangles:
                // Triangles has 3N faces, begin N the number of vertices
                userData.nFaces = attr->count() / 3;
                break;
            case Qt3DRender::QGeometryRenderer::TriangleFan:
            case Qt3DRender::QGeometryRenderer::TriangleStrip:
                // TriangleFan and TriangleStrip have N+2 faces, begin N the number of vertices
                userData.nFaces = attr->count() - 2;
                break;
            default:
                userData.nFaces = 0;
            }
        }
        if (attr->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            userData.positionAttribute.byteOffset = attr->byteOffset();
            userData.positionAttribute.byteStride = attr->byteStride() == 0 ? attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.positionAttribute.bufferData = attr->buffer()->data();
            userData.nVertices = attr->count();
        }
        if (attr->name() == Qt3DRender::QAttribute::defaultNormalAttributeName()) {
            userData.normalAttribute.byteOffset = attr->byteOffset();
            userData.normalAttribute.byteStride = attr->byteStride() == 0 ? attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.normalAttribute.bufferData = attr->buffer()->data();
        }
        if (attr->name() == Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName()) {
            userData.uvAttribute.byteOffset = attr->byteOffset();
            userData.uvAttribute.byteStride = attr->byteStride() == 0 ? attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.uvAttribute.bufferData = attr->buffer()->data();
            userData.vertexBaseTypeForUVAttribute = attr->vertexBaseType();
        }
    }

    if (userData.indexAttribute.bufferData.isNull() ||
        userData.positionAttribute.bufferData.isNull() ||
        userData.normalAttribute.bufferData.isNull() ||
        userData.uvAttribute.bufferData.isNull())
        return {};

    // Create the tangent attribute
    userData.tangentBufferData.resize(userData.nVertices * sizeof(QVector4D));

    return userData;
} // namespace

SMikkTSpaceInterface createMikkTSpaceInterface()
{
    SMikkTSpaceInterface interface;
    interface.m_getNumFaces = [](const SMikkTSpaceContext *pContext) -> int {
        const auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        return userData->nFaces;
    };

    interface.m_getNumVerticesOfFace = [](const SMikkTSpaceContext *pContext, const int iFace) -> int {
        Q_UNUSED(pContext);
        Q_UNUSED(iFace);
        return 3;
    };

    interface.m_getPosition = [](const SMikkTSpaceContext *pContext,
                                 float fvPosOut[],
                                 const int iFace,
                                 const int iVertex) {
        const auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        const auto &vertexIndices = userData->vertexIndicesFinder(iFace);
        const auto vertexIndex = vertexIndices[iVertex];
        const auto byteOffset = userData->positionAttribute.byteOffset;
        const auto byteStride = userData->positionAttribute.byteStride;
        const auto &vertexBufferData = userData->positionAttribute.bufferData;
        const auto positionHead = vertexBufferData.mid(byteOffset + vertexIndex * byteStride);
        const auto *typedPositionHead = reinterpret_cast<const float *>(positionHead.data());
        fvPosOut[0] = typedPositionHead[0];
        fvPosOut[1] = typedPositionHead[1];
        fvPosOut[2] = typedPositionHead[2];
    };

    interface.m_getNormal = [](const SMikkTSpaceContext *pContext,
                               float fvPosOut[],
                               const int iFace,
                               const int iVertex) {
        const auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        const auto &vertexIndices = userData->vertexIndicesFinder(iFace);
        const auto vertexIndex = vertexIndices[iVertex];
        const auto byteOffset = userData->normalAttribute.byteOffset;
        const auto byteStride = userData->normalAttribute.byteStride;
        const auto &normalBufferData = userData->normalAttribute.bufferData;
        const auto normalHead = normalBufferData.mid(byteOffset + vertexIndex * byteStride);
        const auto *typedNormalHead = reinterpret_cast<const float *>(normalHead.data());
        fvPosOut[0] = typedNormalHead[0];
        fvPosOut[1] = typedNormalHead[1];
        fvPosOut[2] = typedNormalHead[2];
    };

    interface.m_getTexCoord = [](const SMikkTSpaceContext *pContext,
                                 float fvPosOut[],
                                 const int iFace,
                                 const int iVertex) {
        const auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        const auto &vertexIndices = userData->vertexIndicesFinder(iFace);
        const auto vertexIndex = vertexIndices[iVertex];
        const auto byteOffset = userData->uvAttribute.byteOffset;
        const auto byteStride = userData->uvAttribute.byteStride;
        const auto &uvBufferData = userData->uvAttribute.bufferData;
        const auto uvHead = uvBufferData.mid(byteOffset + vertexIndex * byteStride);

        // data type can be different from float in UV attribute
        switch (userData->vertexBaseTypeForUVAttribute) {
        case Qt3DRender::QAttribute::VertexBaseType::Float: {
            const auto *typedUvHead = reinterpret_cast<const float *>(uvHead.data());
            fvPosOut[0] = typedUvHead[0];
            fvPosOut[1] = typedUvHead[1];
            break;
        }
        case Qt3DRender::QAttribute::UnsignedByte: {
            const auto *typedUvHead = reinterpret_cast<const unsigned char *>(uvHead.data());
            const auto div = 1.0f / static_cast<float>(std::numeric_limits<unsigned char>::max());
            fvPosOut[0] = div * static_cast<float>(typedUvHead[0]);
            fvPosOut[1] = div * static_cast<float>(typedUvHead[1]);
            break;
        }
        case Qt3DRender::QAttribute::UnsignedShort: {
            const auto *typedUvHead = reinterpret_cast<const unsigned short *>(uvHead.data());
            const auto div = 1.0f / static_cast<float>(std::numeric_limits<unsigned short>::max());
            fvPosOut[0] = div * static_cast<float>(typedUvHead[0]);
            fvPosOut[1] = div * static_cast<float>(typedUvHead[1]);
            break;
        }
        default:
            Q_UNREACHABLE();
        }
    };

    interface.m_setTSpaceBasic = [](const SMikkTSpaceContext *pContext,
                                    const float fvTangent[],
                                    const float fSign,
                                    const int iFace,
                                    const int iVertex) {
        auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        const auto &vertexIndices = userData->vertexIndicesFinder(iFace);
        const auto vertexIndex = vertexIndices[iVertex];
        const auto byteStride = sizeof(QVector4D);
        QVector4D positionHead;
        positionHead[0] = fvTangent[0];
        positionHead[1] = fvTangent[1];
        positionHead[2] = fvTangent[2];
        positionHead[3] = fSign;
        userData->tangentBufferData.insert(vertexIndex * byteStride,
                                           reinterpret_cast<const char *>(&positionHead[0]),
                                           sizeof(positionHead));
    };

    interface.m_setTSpace = nullptr;
    return interface;
}

// TODO This is only needed when the material has normal mapping
// TODO Compute tangents when the primitive is not indexed
Qt3DRender::QAttribute *createTangentAttribute(Qt3DRender::QGeometry *geometry, Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType)
{
    //Create callbacks for MikkTSpace
    SMikkTSpaceContext mikkContext;
    auto userData = precomputeMikkTSpaceUserData(geometry, primitiveType);
    if (userData.tangentBufferData.isNull())
        return nullptr;
    mikkContext.m_pUserData = &userData;

    SMikkTSpaceInterface interface = createMikkTSpaceInterface();
    mikkContext.m_pInterface = &interface;
    genTangSpaceDefault(&mikkContext);

    auto *tangentBuffer = new Qt3DRender::QBuffer;
    constexpr auto NumberValuesPerTangent = 4;
    tangentBuffer->setData(userData.tangentBufferData);
    auto *tangentAttribute = new Qt3DRender::QAttribute(tangentBuffer,
                                                        Qt3DRender::QAttribute::Float,
                                                        NumberValuesPerTangent,
                                                        userData.nVertices);

    return tangentAttribute;
}

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

            if (!geometryIsGLTF2Valid(geometry)) {
                qCWarning(kuesa) << QLatin1Literal("Geometry doesn't meet glTF 2.0 requirements");
                delete geometry;
                return false;
            }

            const auto primitiveType = static_cast<Qt3DRender::QGeometryRenderer::PrimitiveType>(primitivesObject.value(KEY_MODE).toInt(GL_TRIANGLES));
            if (primitiveType == Qt3DRender::QGeometryRenderer::Triangles ||
                primitiveType == Qt3DRender::QGeometryRenderer::TriangleStrip ||
                primitiveType == Qt3DRender::QGeometryRenderer::TriangleFan) {
                const auto &attributes = geometry->attributes();
                const bool hasTangent = std::find_if(std::begin(attributes),
                                                     std::end(attributes),
                                                     [](const Qt3DRender::QAttribute *attr) {
                                                         return attr->name() == Qt3DRender::QAttribute::defaultTangentAttributeName();
                                                     }) != std::end(attributes);

                if (!hasTangent) {
                    auto tangentAttr = createTangentAttribute(geometry, primitiveType);
                    if (tangentAttr) {
                        tangentAttr->setName(Qt3DRender::QAttribute::defaultTangentAttributeName());
                        geometry->addAttribute(tangentAttr);
                    }
                }
            }

            Qt3DRender::QGeometryRenderer *renderer = new Qt3DRender::QGeometryRenderer;
            renderer->setPrimitiveType(primitiveType);
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
        attribute->setProperty("semanticName", attrName);
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
        attribute->setProperty("semanticName", attrName);

        geometry->addAttribute(attribute);
    }
    return true;
}
#endif

QT_END_NAMESPACE
