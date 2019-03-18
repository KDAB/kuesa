/*
    meshparser_utils.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "meshparser_utils_p.h"

#include <QVarLengthArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometryRenderer>

#include <array>

#include "mikktspace.h"
#include "gltf2context_p.h"
#include "gltf2keys_p.h"
#include "gltf2utils_p.h"
#include "gltf2exporter_p.h"

namespace {

const QLatin1String KEY_BUFFERVIEWTARGET = QLatin1String("target");
const QLatin1String KEY_COMPONENTTYPE = QLatin1Literal("componentType");
const QLatin1String KEY_COUNT = QLatin1Literal("count");
const QLatin1String KEY_TYPE = QLatin1Literal("type");
const QLatin1String KEY_MAX = QLatin1Literal("max");
const QLatin1String KEY_MIN = QLatin1Literal("min");
const QLatin1String ATTR_TANGENT = QLatin1Literal("TANGENT");

QVarLengthArray<Qt3DRender::QAttribute::VertexBaseType, 3> validVertexBaseTypesForAttribute(const QString &attributeName)
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

    return QVarLengthArray<Qt3DRender::QAttribute::VertexBaseType, 3>();
}

QVarLengthArray<uint, 2> validVertexSizesForAttribute(const QString &attributeName)
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

    return QVarLengthArray<uint, 2>();
}

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
        const auto *iFaceHead = data.constData() + (indexAttribute.byteOffset + iFace * indexAttribute.byteStride);
        switch (vertexBaseType) {
        case Qt3DRender::QAttribute::UnsignedByte: {
            const auto *typedIndices = reinterpret_cast<const unsigned char *>(iFaceHead);
            verticesForLastFace = { typedIndices[0], typedIndices[1], typedIndices[2] };
            break;
        }
        case Qt3DRender::QAttribute::UnsignedShort: {
            const auto *typedIndices = reinterpret_cast<const unsigned short *>(iFaceHead);
            verticesForLastFace = { typedIndices[0], typedIndices[1], typedIndices[2] };
            break;
        }
        case Qt3DRender::QAttribute::UnsignedInt: {
            const auto *typedIndices = reinterpret_cast<const unsigned int *>(iFaceHead);
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

    Attribute indexAttribute;
    Attribute positionAttribute;
    Attribute uvAttribute;
    Attribute normalAttribute;

    int nFaces;
    int nVertices;
    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;
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
    userData.tangentBufferData.resize(userData.nVertices * sizeof(std::array<float, 4>));

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
        const auto *positionHead = vertexBufferData.constData() + (byteOffset + vertexIndex * byteStride);
        const auto *typedPositionHead = reinterpret_cast<const float *>(positionHead);
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
        const auto *normalHead = normalBufferData.constData() + (byteOffset + vertexIndex * byteStride);
        const auto *typedNormalHead = reinterpret_cast<const float *>(normalHead);
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
        const auto *uvHead = uvBufferData.constData() + (byteOffset + vertexIndex * byteStride);

        // data type can be different from float in UV attribute
        switch (userData->vertexBaseTypeForUVAttribute) {
        case Qt3DRender::QAttribute::VertexBaseType::Float: {
            const auto *typedUvHead = reinterpret_cast<const float *>(uvHead);
            fvPosOut[0] = typedUvHead[0];
            fvPosOut[1] = typedUvHead[1];
            break;
        }
        case Qt3DRender::QAttribute::UnsignedByte: {
            const auto *typedUvHead = reinterpret_cast<const unsigned char *>(uvHead);
            const auto div = 1.0f / static_cast<float>(std::numeric_limits<unsigned char>::max());
            fvPosOut[0] = div * static_cast<float>(typedUvHead[0]);
            fvPosOut[1] = div * static_cast<float>(typedUvHead[1]);
            break;
        }
        case Qt3DRender::QAttribute::UnsignedShort: {
            const auto *typedUvHead = reinterpret_cast<const unsigned short *>(uvHead);
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
        std::array<float, 4> positionHead;
        const auto byteStride = sizeof(decltype(positionHead));
        positionHead[0] = fvTangent[0];
        positionHead[1] = fvTangent[1];
        positionHead[2] = fvTangent[2];
        positionHead[3] = fSign;
        memcpy(userData->tangentBufferData.data() + vertexIndex * byteStride,
               reinterpret_cast<const char *>(&positionHead[0]),
               sizeof(positionHead));
    };

    interface.m_setTSpace = nullptr;
    return interface;
}

bool vertexBaseTypeForAttributesAreValid(const QVector<Qt3DRender::QAttribute *> &attributes)
{
    for (Qt3DRender::QAttribute *attribute : attributes) {
        const auto validVertexBaseTypes = validVertexBaseTypesForAttribute(attribute->name());
        const auto vertexBaseType = attribute->vertexBaseType();
        if (!validVertexBaseTypes.isEmpty() && !validVertexBaseTypes.contains(vertexBaseType))
            return false;
    }
    return true;
}

bool vertexSizesForAttributesAreValid(const QVector<Qt3DRender::QAttribute *> &attributes)
{
    for (Qt3DRender::QAttribute *attribute : attributes) {
        const auto validVertexSizes = validVertexSizesForAttribute(attribute->name());
        const auto vertexSize = attribute->vertexSize();
        if (!validVertexSizes.isEmpty() && !validVertexSizes.contains(vertexSize))
            return false;
    }
    return true;
}

bool getMeshGLTFInformation(const Qt3DRender::QGeometryRenderer *mesh, const Kuesa::GLTF2Import::GLTF2Context *context, QString &meshName, int &primitiveNumber)
{
    for (int i = 0; i < context->meshesCount(); ++i) {
        const Kuesa::GLTF2Import::Mesh gltf2Mesh = context->mesh(i);

        for (int nPriv = 0; nPriv < gltf2Mesh.meshPrimitives.size(); ++nPriv) {
            Qt3DRender::QGeometryRenderer *primitive = gltf2Mesh.meshPrimitives[nPriv].primitiveRenderer;

            if (primitive != mesh)
                continue;

            meshName = gltf2Mesh.name;
            primitiveNumber = nPriv;
            return true;
        }
    }

    return false;
}

Qt3DRender::QAttribute *createTangentAttribute(Qt3DRender::QGeometryRenderer *mesh)
{
    Qt3DRender::QGeometry *geometry = mesh->geometry();
    if (!geometry)
        return nullptr;

    //Create callbacks for MikkTSpace
    ::SMikkTSpaceContext mikkContext;
    auto userData = precomputeMikkTSpaceUserData(geometry, mesh->primitiveType());
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

    tangentAttribute->setName(Qt3DRender::QAttribute::defaultTangentAttributeName());
    geometry->addAttribute(tangentAttribute);

    return tangentAttribute;
}

} // namespace

// TODO This is only needed when the material has normal mapping
// TODO Compute tangents when the primitive is not indexed
namespace Kuesa {
namespace GLTF2Import {
namespace MeshParserUtils {

bool needsTangentAttribute(const Qt3DRender::QGeometryRenderer *mesh)
{
    const auto primitiveType = mesh->primitiveType();

    // only generate tangents for triangles
    if (primitiveType != Qt3DRender::QGeometryRenderer::Triangles &&
        primitiveType != Qt3DRender::QGeometryRenderer::TriangleStrip &&
        primitiveType != Qt3DRender::QGeometryRenderer::TriangleFan)
        return false;

    Qt3DRender::QGeometry *geometry = mesh->geometry();
    if (!geometry)
        return false;

    const auto &attributes = geometry->attributes();
    const bool hasTangent = std::find_if(std::begin(attributes),
                                         std::end(attributes),
                                         [](const Qt3DRender::QAttribute *attr) {
                                             return attr->name() == Qt3DRender::QAttribute::defaultTangentAttributeName();
                                         }) != std::end(attributes);

    return !hasTangent;
}

bool generateTangentAttribute(Qt3DRender::QGeometryRenderer *mesh, GLTF2Context *context)
{
    if (!GLTF2Import::MeshParserUtils::needsTangentAttribute(mesh))
        return false;

    // get information about mesh name and primitive index within the original glTF file
    QString meshName;
    int primitiveNumber;
    if (!getMeshGLTFInformation(mesh, context, meshName, primitiveNumber))
        return false;

    QJsonDocument jsonDocument = context->json();
    QJsonObject rootObject = jsonDocument.object();

    // create new attribute
    Qt3DRender::QAttribute *newTangentAttr = createTangentAttribute(mesh);
    if (!newTangentAttr)
        return false;

    const QByteArray bufferData = newTangentAttr->buffer()->data();
    const int bufferIndex = rootObject.value(KEY_BUFFERS).toArray().size();

    // glTF: add bufferView
    QJsonObject jsonBufferView;
    jsonBufferView[KEY_BUFFER] = bufferIndex;
    jsonBufferView[KEY_BYTELENGTH] = bufferData.size();
    jsonBufferView[KEY_BYTEOFFSET] = 0;
    jsonBufferView[KEY_BUFFERVIEWTARGET] = GL_ARRAY_BUFFER;
    const int bufferViewIndex = addToJsonChildArray(rootObject, KEY_BUFFERVIEWS, jsonBufferView);

    // add accessor
    QJsonObject jsonAccessor;
    jsonAccessor[KEY_BUFFERVIEW] = bufferViewIndex;
    jsonAccessor[KEY_COMPONENTTYPE] = GL_FLOAT;
    jsonAccessor[KEY_COUNT] = static_cast<int>(newTangentAttr->count());
    jsonAccessor[KEY_TYPE] = QLatin1Literal("VEC4");
    jsonAccessor[KEY_MIN] = QJsonArray{ 0.0, 0.0, 0.0, 0.0 };
    jsonAccessor[KEY_MAX] = QJsonArray{ 0.0, 0.0, 0.0, 0.0 };
    const int accessorIndex = addToJsonChildArray(rootObject, KEY_ACCESSORS, jsonAccessor);

    // modify mesh object and add attribute/accessor
    QJsonArray jsonMeshes = rootObject.value(KEY_MESHES).toArray();
    for (auto it = jsonMeshes.begin(); it != jsonMeshes.end(); ++it) {
        // search for the righ mesh!
        QJsonObject jsonMesh = it->toObject();
        if (jsonMesh.value(KEY_NAME) != meshName)
            continue;

        // replace matching primitive
        QJsonArray jsonPrimitives = jsonMesh.value(KEY_PRIMITIVES).toArray();
        QJsonObject jsonPrimitive = jsonPrimitives.at(primitiveNumber).toObject();
        QJsonObject jsonPrimAttrs = jsonPrimitive.value(KEY_ATTRIBUTES).toObject();
        jsonPrimAttrs[ATTR_TANGENT] = accessorIndex;
        jsonPrimitive[KEY_ATTRIBUTES] = jsonPrimAttrs;
        jsonPrimitives[primitiveNumber] = jsonPrimitive;
        jsonMesh[KEY_PRIMITIVES] = jsonPrimitives;

        *it = jsonMesh;
    }
    rootObject[KEY_MESHES] = jsonMeshes;

    // create tangent buffer file
    const QString basePath = context->basePath();
    const QString bufferFileName = generateUniqueFilename(basePath, "kuesaTangentBuffers.bin");
    QFile bufferFile(basePath + QDir::separator() + bufferFileName);

    if (!bufferFile.open(QFile::WriteOnly)) {
        qWarning() << "Can't open" << bufferFile.fileName() << "for writing tangent buffer";
    } else {
        // add buffer to JSON
        bufferFile.write(bufferData);
        QJsonObject jsonBuffer;
        jsonBuffer[KEY_BYTELENGTH] = bufferData.size();
        jsonBuffer[KEY_URI] = bufferFileName;
        addToJsonChildArray(rootObject, KEY_BUFFERS, jsonBuffer);

        context->addLocalFile(bufferFileName);
    }

    context->setJson(QJsonDocument(rootObject));

    return true;
}

bool geometryIsGLTF2Valid(Qt3DRender::QGeometry *geometry)
{
    const auto &attributes = geometry->attributes();
    const bool vertexBaseTypesAreValid = vertexBaseTypeForAttributesAreValid(attributes);
    const bool vertexSizesAreValid = vertexSizesForAttributesAreValid(attributes);
    return vertexSizesAreValid && vertexBaseTypesAreValid;
}
} // namespace MeshParserUtils
} // namespace GLTF2Import
} // namespace Kuesa
