/*
    meshparser_utils.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QAttribute>
#else
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#endif
#include <Qt3DRender/QGeometryRenderer>

#include <array>
#include <cstring>

#include "mikktspace.h"
#include "gltf2context_p.h"
#include "gltf2keys_p.h"
#include "gltf2utils_p.h"
#include "gltf2exporter_p.h"
#include "kuesa_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DGeometry;

namespace {

const QLatin1String ATTR_TANGENT = QLatin1String("TANGENT");

const auto morphTargetAttributeRegExps = []() {
    const QString morphTargetAttributePattern = QStringLiteral(R"(%1(_\d+)?)");
    const QString morphTargetBaseAttributeNames[]{
        QAttribute::defaultPositionAttributeName(),
        QAttribute::defaultNormalAttributeName(),
        QAttribute::defaultTangentAttributeName()
    };

    // Morph Target Attributes
    constexpr int maxEntries = sizeof(morphTargetBaseAttributeNames) / sizeof(morphTargetBaseAttributeNames[0]);
    std::array<QRegularExpression, maxEntries> regExps;

    for (int i = 0; i < maxEntries; ++i) {
        const QString &baseAttributeName = morphTargetBaseAttributeNames[i];
        QRegularExpression &re = regExps[i];
        re.setPattern(morphTargetAttributePattern.arg(baseAttributeName));
        Q_ASSERT(re.isValid());
    }
    return regExps;
}();

QVarLengthArray<QAttribute::VertexBaseType, 3> validVertexBaseTypesForAttribute(const QString &attributeName)
{
    {
        // Morph Target Attributes
        for (const QRegularExpression &re : morphTargetAttributeRegExps) {
            if (re.match(attributeName).hasMatch())
                return { QAttribute::Float };
        }
    }

    // Standard Attributes
    if (attributeName == QAttribute::defaultTextureCoordinateAttributeName())
        return { QAttribute::Float,
                 QAttribute::UnsignedByte,
                 QAttribute::UnsignedShort };
    if (attributeName == QAttribute::defaultTextureCoordinate1AttributeName())
        return { QAttribute::Float,
                 QAttribute::UnsignedByte,
                 QAttribute::UnsignedShort };
    if (attributeName == QAttribute::defaultColorAttributeName())
        return { QAttribute::Float,
                 QAttribute::UnsignedByte,
                 QAttribute::UnsignedShort };
    if (attributeName == QAttribute::defaultJointIndicesAttributeName())
        return { QAttribute::UnsignedByte,
                 QAttribute::UnsignedShort };
    if (attributeName == QAttribute::defaultJointWeightsAttributeName())
        return { QAttribute::Float,
                 QAttribute::UnsignedByte,
                 QAttribute::UnsignedShort };

    return QVarLengthArray<QAttribute::VertexBaseType, 3>();
}

QVarLengthArray<uint, 2> validVertexSizesForAttribute(const QString &attributeName)
{
    // Morph Target Attributes
    {
        // Morph Target Attributes
        for (const QRegularExpression &re : morphTargetAttributeRegExps) {
            if (re.match(attributeName).hasMatch()) {
                // Tangent attribute size is 4, all others are 3
                if (attributeName.startsWith(QAttribute::defaultTangentAttributeName()))
                    return { 3, 4 };
                return { 3 };
            }
        }
    }

    // Standard Attributes
    if (attributeName == QAttribute::defaultPositionAttributeName())
        return { 3 };
    if (attributeName == QAttribute::defaultNormalAttributeName())
        return { 3 };
    if (attributeName == QAttribute::defaultTangentAttributeName())
        return { 3, 4 };
    if (attributeName == QAttribute::defaultTextureCoordinateAttributeName())
        return { 2 };
    if (attributeName == QAttribute::defaultTextureCoordinate1AttributeName())
        return { 2 };
    if (attributeName == QAttribute::defaultColorAttributeName())
        return { 3, 4 };
    if (attributeName == QAttribute::defaultJointIndicesAttributeName())
        return { 4 };
    if (attributeName == QAttribute::defaultJointWeightsAttributeName())
        return { 4 };

    return QVarLengthArray<uint, 2>();
}

uint vertexBaseTypeSize(QAttribute::VertexBaseType vertexBaseType)
{
    switch (vertexBaseType) {
    case QAttribute::UnsignedByte:
        return 1U;
    case QAttribute::UnsignedShort:
        return 2U;
    case QAttribute::UnsignedInt:
    case QAttribute::Float:
        return 4U;
    default:
        Q_UNREACHABLE();
    }
}

QAttribute *attributeFromGeometry(const QString &name,
                                  const QGeometry *geometry)
{
    const auto attributes = geometry->attributes();
    const auto end = std::end(attributes);
    auto it = std::find_if(std::begin(attributes),
                           end,
                           [name](const QAttribute *attr) {
                               return attr->name() == name;
                           });
    if (it == end)
        return nullptr;
    return *it;
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

        if (data.isNull()) {
            verticesForLastFace = { 3 * iFace + 0u, 3 * iFace + 1u, 3 * iFace + 2u };
            return verticesForLastFace;
        }

        const auto *iFaceHead = data.constData() + (indexAttribute.byteOffset + iFace * indexAttribute.byteStride);
        switch (vertexBaseType) {
        case QAttribute::UnsignedByte: {
            const auto *typedIndices = reinterpret_cast<const unsigned char *>(iFaceHead);
            verticesForLastFace = { typedIndices[0], typedIndices[1], typedIndices[2] };
            break;
        }
        case QAttribute::UnsignedShort: {
            const auto *typedIndices = reinterpret_cast<const unsigned short *>(iFaceHead);
            verticesForLastFace = { typedIndices[0], typedIndices[1], typedIndices[2] };
            break;
        }
        case QAttribute::UnsignedInt: {
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
    QAttribute::VertexBaseType vertexBaseType;
    mutable int lastFace;
    mutable std::array<unsigned int, 3> verticesForLastFace;
};

struct MikkTSpaceUserData {

    Attribute indexAttribute;

    Attribute positionAttribute;
    Attribute positionMorphAttribute;

    Attribute normalAttribute;
    Attribute normalMorphAttribute;

    Attribute uvAttribute;

    int nFaces;
    int nVertices;
    QGeometryRenderer::PrimitiveType primitiveType;
    QAttribute::VertexBaseType vertexBaseTypeForUVAttribute;

    FindVertexIndicesInFaceHelper vertexIndicesFinder;
    QByteArray tangentBufferData;
};

MikkTSpaceUserData precomputeMikkTSpaceUserData(QGeometry *geometry,
                                                QGeometryRenderer::PrimitiveType primitiveType)
{
    // Precompute some data
    MikkTSpaceUserData userData;

    const auto &attributes = geometry->attributes();
    for (const auto attr : attributes) {
        if (attr->attributeType() == QAttribute::IndexAttribute) {
            userData.indexAttribute.byteOffset = attr->byteOffset();
            constexpr auto NumberVerticesInFace = 3;
            userData.indexAttribute.byteStride = attr->byteStride() == 0 ? NumberVerticesInFace * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.indexAttribute.bufferData = attr->buffer()->data();
            userData.vertexIndicesFinder.vertexBaseType = attr->vertexBaseType();
            userData.vertexIndicesFinder.indexAttribute = userData.indexAttribute;
            switch (primitiveType) {
            case QGeometryRenderer::Triangles:
                // Triangles has 3N faces, begin N the number of vertices
                userData.nFaces = attr->count() / 3;
                break;
            case QGeometryRenderer::TriangleFan:
            case QGeometryRenderer::TriangleStrip:
                // TriangleFan and TriangleStrip have N+2 faces, begin N the number of vertices
                userData.nFaces = attr->count() - 2;
                break;
            default:
                userData.nFaces = 0;
            }
        }
        if (attr->name() == QAttribute::defaultPositionAttributeName()) {
            userData.positionAttribute.byteOffset = attr->byteOffset();
            userData.positionAttribute.byteStride = attr->byteStride() == 0 ? attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.positionAttribute.bufferData = attr->buffer()->data();
            userData.nVertices = attr->count();
            if (userData.indexAttribute.bufferData.isNull()) {
                userData.nFaces = userData.nVertices / 3;
            }
        }
        if (attr->name() == QAttribute::defaultNormalAttributeName()) {
            userData.normalAttribute.byteOffset = attr->byteOffset();
            userData.normalAttribute.byteStride = attr->byteStride() == 0 ? attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.normalAttribute.bufferData = attr->buffer()->data();
        }
        if (attr->name() == QAttribute::defaultTextureCoordinateAttributeName()) {
            userData.uvAttribute.byteOffset = attr->byteOffset();
            userData.uvAttribute.byteStride = attr->byteStride() == 0 ? attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType()) : attr->byteStride();
            userData.uvAttribute.bufferData = attr->buffer()->data();
            userData.vertexBaseTypeForUVAttribute = attr->vertexBaseType();
        }
    }

    if (userData.positionAttribute.bufferData.isNull() ||
        userData.normalAttribute.bufferData.isNull() ||
        userData.uvAttribute.bufferData.isNull())
        return {};

    // Create the tangent attribute
    userData.tangentBufferData.resize(userData.nVertices * sizeof(std::array<float, 4>));

    return userData;
}

SMikkTSpaceInterface createMikkTSpaceInterface()
{
    ::SMikkTSpaceInterface interface;
    interface.m_getNumFaces = [](const ::SMikkTSpaceContext *pContext) -> int {
        const auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        return userData->nFaces;
    };

    interface.m_getNumVerticesOfFace = [](const ::SMikkTSpaceContext *pContext, const int iFace) -> int {
        Q_UNUSED(pContext);
        Q_UNUSED(iFace);
        return 3;
    };

    interface.m_getPosition = [](const ::SMikkTSpaceContext *pContext,
                                 float fvPosOut[],
                                 const int iFace,
                                 const int iVertex) {
        const auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        const auto &vertexIndices = userData->vertexIndicesFinder(iFace);
        const auto vertexIndex = vertexIndices[iVertex];

        QVector3D position;
        {
            const auto byteOffset = userData->positionAttribute.byteOffset;
            const auto byteStride = userData->positionAttribute.byteStride;
            const auto &vertexBufferData = userData->positionAttribute.bufferData;
            const auto *positionHead = vertexBufferData.constData() + (byteOffset + vertexIndex * byteStride);
            position = *reinterpret_cast<const QVector3D *>(positionHead);
        }

        QVector3D morphPosition = { 0.0f, 0.0f, 0.0f };
        if (!userData->positionMorphAttribute.bufferData.isNull()) {
            const auto byteOffset = userData->positionMorphAttribute.byteOffset;
            const auto byteStride = userData->positionMorphAttribute.byteStride;
            const auto &vertexBufferData = userData->positionMorphAttribute.bufferData;
            const auto *positionHead = vertexBufferData.constData() + (byteOffset + vertexIndex * byteStride);
            morphPosition = *reinterpret_cast<const QVector3D *>(positionHead);
        }

        const auto fv = position + morphPosition;
        fvPosOut[0] = fv[0];
        fvPosOut[1] = fv[1];
        fvPosOut[2] = fv[2];
    };

    interface.m_getNormal = [](const ::SMikkTSpaceContext *pContext,
                               float fvPosOut[],
                               const int iFace,
                               const int iVertex) {
        const auto *userData = reinterpret_cast<MikkTSpaceUserData *>(pContext->m_pUserData);
        const auto &vertexIndices = userData->vertexIndicesFinder(iFace);
        const auto vertexIndex = vertexIndices[iVertex];

        QVector3D normal;
        {
            const auto byteOffset = userData->normalAttribute.byteOffset;
            const auto byteStride = userData->normalAttribute.byteStride;
            const auto &normalBufferData = userData->normalAttribute.bufferData;
            const auto *normalHead = normalBufferData.constData() + (byteOffset + vertexIndex * byteStride);
            normal = *reinterpret_cast<const QVector3D *>(normalHead);
        }

        QVector3D morphNormal = { 0.0f, 0.0f, 0.0f };
        if (!userData->normalMorphAttribute.bufferData.isNull()) {
            const auto byteOffset = userData->normalMorphAttribute.byteOffset;
            const auto byteStride = userData->normalMorphAttribute.byteStride;
            const auto &normalBufferData = userData->normalMorphAttribute.bufferData;
            const auto *normalHead = normalBufferData.constData() + (byteOffset + vertexIndex * byteStride);
            morphNormal = *reinterpret_cast<const QVector3D *>(normalHead);
        }

        const auto fv = normal + morphNormal;
        fvPosOut[0] = fv[0];
        fvPosOut[1] = fv[1];
        fvPosOut[2] = fv[2];
    };

    interface.m_getTexCoord = [](const ::SMikkTSpaceContext *pContext,
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
        case QAttribute::VertexBaseType::Float: {
            const auto *typedUvHead = reinterpret_cast<const float *>(uvHead);
            fvPosOut[0] = typedUvHead[0];
            fvPosOut[1] = typedUvHead[1];
            break;
        }
        case QAttribute::UnsignedByte: {
            const auto *typedUvHead = reinterpret_cast<const unsigned char *>(uvHead);
            const auto div = 1.0f / static_cast<float>(std::numeric_limits<unsigned char>::max());
            fvPosOut[0] = div * static_cast<float>(typedUvHead[0]);
            fvPosOut[1] = div * static_cast<float>(typedUvHead[1]);
            break;
        }
        case QAttribute::UnsignedShort: {
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

    interface.m_setTSpaceBasic = [](const ::SMikkTSpaceContext *pContext,
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
        positionHead[3] = -fSign;
        memcpy(userData->tangentBufferData.data() + vertexIndex * byteStride,
               reinterpret_cast<const char *>(&positionHead[0]),
               sizeof(positionHead));
    };

    interface.m_setTSpace = nullptr;
    return interface;
}

bool vertexBaseTypeForAttributesAreValid(const QVector<QAttribute *> &attributes)
{
    for (QAttribute *attribute : attributes) {
        const auto validVertexBaseTypes = validVertexBaseTypesForAttribute(attribute->name());
        const auto vertexBaseType = attribute->vertexBaseType();
        if (!validVertexBaseTypes.isEmpty() && !validVertexBaseTypes.contains(vertexBaseType))
            return false;
    }
    return true;
}

bool vertexSizesForAttributesAreValid(const QVector<QAttribute *> &attributes)
{
    for (QAttribute *attribute : attributes) {
        const auto validVertexSizes = validVertexSizesForAttribute(attribute->name());
        const auto vertexSize = attribute->vertexSize();
        if (!validVertexSizes.isEmpty() && !validVertexSizes.contains(vertexSize))
            return false;
    }
    return true;
}

bool getMeshGLTFInformation(const QGeometryRenderer *primitiveRenderer,
                            const Kuesa::GLTF2Import::GLTF2Context *context,
                            Kuesa::GLTF2Import::Mesh &mesh, int &primitiveNumber)
{
    for (int i = 0; i < context->meshesCount(); ++i) {
        const Kuesa::GLTF2Import::Mesh gltf2Mesh = context->mesh(i);

        for (int nPriv = 0; nPriv < gltf2Mesh.meshPrimitives.size(); ++nPriv) {
            const QGeometryRenderer *primitive = gltf2Mesh.meshPrimitives.at(nPriv).primitiveRenderer;

            if (primitive != primitiveRenderer)
                continue;

            primitiveNumber = nPriv;
            mesh = gltf2Mesh;
            return true;
        }
    }

    return false;
}

QLatin1String glTFAttributeNameFromQt3DAttribute(const QAttribute *attribute)
{
    const QString attributeName = attribute->name();
    // Standard Attributes
    if (attributeName.startsWith(QAttribute::defaultPositionAttributeName()))
        return QLatin1String("POSITION");
    if (attributeName.startsWith(QAttribute::defaultNormalAttributeName()))
        return QLatin1String("NORMAL");
    if (attributeName.startsWith(QAttribute::defaultTangentAttributeName()))
        return QLatin1String("TANGENT");
    if (attributeName.startsWith(QAttribute::defaultTextureCoordinateAttributeName()))
        return QLatin1String("TEXCOORD_0");
    if (attributeName.startsWith(QAttribute::defaultTextureCoordinate1AttributeName()))
        return QLatin1String("TEXCOORD_1");
    if (attributeName.startsWith(QAttribute::defaultTextureCoordinate2AttributeName()))
        return QLatin1String("TEXCOORD_2");
    if (attributeName.startsWith(QAttribute::defaultColorAttributeName()))
        return QLatin1String("COLOR_0");
    if (attributeName.startsWith(QAttribute::defaultJointIndicesAttributeName()))
        return QLatin1String("JOINTS_0");
    if (attributeName.startsWith(QAttribute::defaultJointWeightsAttributeName()))
        return QLatin1String("WEIGHTS_0");
    return QLatin1String();
}

QLatin1String glTFTypeForAttribute(const QAttribute *attribute)
{
    Q_ASSERT(attribute->vertexBaseType() == QAttribute::Float);
    switch (attribute->vertexSize()) {
    case 1:
        return QLatin1String("SCALAR");
    case 2:
        return QLatin1String("VEC2");
    case 3:
        return QLatin1String("VEC3");
    case 4:
        return QLatin1String("VEC4");
    case 9:
        return QLatin1String("MAT3");
    case 16:
        return QLatin1String("MAT4");
    default:
        Q_UNREACHABLE();
    }
    return QLatin1String();
}

int addJsonBufferView(QJsonObject &rootObject,
                      int bufferIndex,
                      int byteLength,
                      int byteStride = 0)
{
    QJsonObject jsonBufferView;
    jsonBufferView[Kuesa::GLTF2Import::KEY_BUFFER] = bufferIndex;
    jsonBufferView[Kuesa::GLTF2Import::KEY_BYTELENGTH] = byteLength;
    jsonBufferView[Kuesa::GLTF2Import::KEY_BYTEOFFSET] = 0;
    if (byteStride > 0)
        jsonBufferView[Kuesa::GLTF2Import::KEY_BYTESTRIDE] = byteStride;
    jsonBufferView[Kuesa::GLTF2Import::KEY_BUFFERVIEWTARGET] = GL_ARRAY_BUFFER;
    const int bufferViewIndex = Kuesa::addToJsonChildArray(rootObject, Kuesa::GLTF2Import::KEY_BUFFERVIEWS, jsonBufferView);
    return bufferViewIndex;
}

int addJsonAccessor(QJsonObject &rootObject,
                    int bufferViewIndex,
                    const QAttribute *attribute)
{
    // add accessor
    QJsonObject jsonAccessor;
    jsonAccessor[Kuesa::GLTF2Import::KEY_BUFFERVIEW] = bufferViewIndex;
    jsonAccessor[Kuesa::GLTF2Import::KEY_COMPONENTTYPE] = GL_FLOAT;
    jsonAccessor[Kuesa::GLTF2Import::KEY_BYTEOFFSET] = static_cast<int>(attribute->byteOffset());
    jsonAccessor[Kuesa::GLTF2Import::KEY_COUNT] = static_cast<int>(attribute->count());
    jsonAccessor[Kuesa::GLTF2Import::KEY_TYPE] = glTFTypeForAttribute(attribute);
    const int accessorIndex = Kuesa::addToJsonChildArray(rootObject, Kuesa::GLTF2Import::KEY_ACCESSORS, jsonAccessor);
    return accessorIndex;
}

int addJsonBuffer(QJsonObject &rootObject,
                  const QByteArray &data,
                  const QString bufferFileName,
                  const QString pathPrefix)
{
    QFile bufferFile(pathPrefix + bufferFileName);
    if (!bufferFile.open(QFile::WriteOnly)) {
        qCWarning(kuesa) << "Can't open" << bufferFile.fileName() << "for writing tangent buffer";
        return -1;
    }
    bufferFile.write(data);
    QJsonObject jsonBuffer;
    jsonBuffer[Kuesa::GLTF2Import::KEY_BYTELENGTH] = data.size();
    jsonBuffer[Kuesa::GLTF2Import::KEY_URI] = bufferFileName;
    const int bufferIndex = Kuesa::addToJsonChildArray(rootObject, Kuesa::GLTF2Import::KEY_BUFFERS, jsonBuffer);
    return bufferIndex;
}

QAttribute *generateTangentForBaseMesh(MikkTSpaceUserData *mikkTSpaceUserData, SMikkTSpaceInterface *interface)
{
    ::SMikkTSpaceContext mikkContext;
    mikkContext.m_pUserData = mikkTSpaceUserData;

    mikkContext.m_pInterface = interface;
    genTangSpaceDefault(&mikkContext);

    auto *tangentBuffer = new Qt3DGeometry::QBuffer;
    constexpr auto NumberValuesPerTangent = 4;
    tangentBuffer->setData(mikkTSpaceUserData->tangentBufferData);
    auto *tangentAttribute = new QAttribute(tangentBuffer,
                                                        QAttribute::defaultTangentAttributeName(),
                                                        QAttribute::Float,
                                                        NumberValuesPerTangent,
                                                        mikkTSpaceUserData->nVertices);

    return tangentAttribute;
}

QAttribute *generateTangentForMorphTarget(QGeometry *geometry, MikkTSpaceUserData *mikkTSpaceUserData, SMikkTSpaceInterface *interface, int morphTargetId)
{
    // Update userdata to add the morphed attributes
    QString positionMorphName = QAttribute::defaultPositionAttributeName() + QStringLiteral("_") + QString::number(morphTargetId + 1);
    QString normalMorphName = QAttribute::defaultNormalAttributeName() + QStringLiteral("_") + QString::number(morphTargetId + 1);
    QString tangentName = QAttribute::defaultTangentAttributeName();

    auto *positionMorphAttribute = attributeFromGeometry(positionMorphName, geometry);
    auto *normalMorphAttribute = attributeFromGeometry(normalMorphName, geometry);

    auto *tangentAttribute = attributeFromGeometry(tangentName, geometry);
    auto tangentByteOffset = tangentAttribute->byteOffset();
    auto tangentByteStride = tangentAttribute->byteStride() == 0 ? //
            tangentAttribute->vertexSize() * vertexBaseTypeSize(tangentAttribute->vertexBaseType())
                                                                 : tangentAttribute->byteStride();
    auto tangentBufferData = tangentAttribute->buffer()->data();

    const auto nVertices = mikkTSpaceUserData->nVertices;
    if (positionMorphAttribute && normalMorphAttribute) {
        ::SMikkTSpaceContext mikkContext;

        if (normalMorphAttribute) {
            mikkTSpaceUserData->normalMorphAttribute.byteOffset = normalMorphAttribute->byteOffset();
            mikkTSpaceUserData->normalMorphAttribute.byteStride = normalMorphAttribute->byteStride() == 0 ? //
                    normalMorphAttribute->vertexSize() * vertexBaseTypeSize(normalMorphAttribute->vertexBaseType())
                                                                                                          : normalMorphAttribute->byteStride();
            mikkTSpaceUserData->normalMorphAttribute.bufferData = normalMorphAttribute->buffer()->data();
        } else {
            mikkTSpaceUserData->normalMorphAttribute.byteOffset = 0;
            mikkTSpaceUserData->normalMorphAttribute.byteStride = 0;
            mikkTSpaceUserData->normalMorphAttribute.bufferData = {};
        }

        if (positionMorphAttribute) {
            mikkTSpaceUserData->positionMorphAttribute.byteOffset = positionMorphAttribute->byteOffset();
            mikkTSpaceUserData->positionMorphAttribute.byteStride = positionMorphAttribute->byteStride() == 0 ? //
                    positionMorphAttribute->vertexSize() * vertexBaseTypeSize(positionMorphAttribute->vertexBaseType())
                                                                                                              : positionMorphAttribute->byteStride();
            mikkTSpaceUserData->positionMorphAttribute.bufferData = positionMorphAttribute->buffer()->data();
        } else {
            mikkTSpaceUserData->positionMorphAttribute.byteOffset = 0;
            mikkTSpaceUserData->positionMorphAttribute.byteStride = 0;
            mikkTSpaceUserData->positionMorphAttribute.bufferData = {};
        }

        mikkContext.m_pUserData = mikkTSpaceUserData;

        mikkContext.m_pInterface = interface;
        genTangSpaceDefault(&mikkContext);

        // TODO instead of creating a new buffer, try to use a new MikkTSpaceInterface which uses directly the buffer we need
        QByteArray morphTargetTangentMorphData;
        morphTargetTangentMorphData.resize(nVertices * sizeof(QVector3D));

        for (int vertexId = 0; vertexId < nVertices; ++vertexId) {
            const auto *tangentHead = tangentBufferData.constData() + (tangentByteOffset + vertexId * tangentByteStride);
            const auto tangentValue = *reinterpret_cast<const QVector4D *>(tangentHead);

            auto *tangentMorphHead4D = mikkTSpaceUserData->tangentBufferData.data() + (vertexId * sizeof(QVector4D));
            auto *tangentMorphHead3D = morphTargetTangentMorphData.data() + (vertexId * sizeof(QVector3D));

            QVector4D *tangentMorphValue4D = reinterpret_cast<QVector4D *>(tangentMorphHead4D);
            QVector3D *tangentMorphValue3D = reinterpret_cast<QVector3D *>(tangentMorphHead3D);
            *tangentMorphValue3D = QVector3D{ tangentMorphValue4D->x(), tangentMorphValue4D->y(), tangentMorphValue4D->z() } //
                    - QVector3D{ tangentValue.x(), tangentValue.y(), tangentValue.z() };
        }

        auto *tangentBuffer = new Qt3DGeometry::QBuffer;
        constexpr auto NumberValuesPerMorphTangent = 3;
        tangentBuffer->setData(morphTargetTangentMorphData);
        auto *tangentAttribute = new QAttribute(tangentBuffer,
                                                            QAttribute::defaultTangentAttributeName() + QStringLiteral("_") + QString::number(morphTargetId + 1),
                                                            QAttribute::Float,
                                                            NumberValuesPerMorphTangent,
                                                            mikkTSpaceUserData->nVertices);

        return tangentAttribute;
    }
    return nullptr;
}

} // namespace

// TODO This is only needed when the material has normal mapping
// TODO Compute tangents when the primitive is not indexed
namespace Kuesa {
namespace GLTF2Import {
namespace MeshParserUtils {

void createTangentForGeometry(QGeometry *geometry, QGeometryRenderer::PrimitiveType primitiveType)
{
    auto mikkTSpaceUserData = precomputeMikkTSpaceUserData(geometry, primitiveType);

    if (mikkTSpaceUserData.tangentBufferData.isNull())
        return;

    auto mikkTSpaceInterface = createMikkTSpaceInterface();
    auto tangentAttribute = generateTangentForBaseMesh(&mikkTSpaceUserData, &mikkTSpaceInterface);
    geometry->addAttribute(tangentAttribute);

    for (int morphTargetId = 0; morphTargetId < 8; ++morphTargetId) {
        tangentAttribute = generateTangentForMorphTarget(geometry, &mikkTSpaceUserData, &mikkTSpaceInterface, morphTargetId);
        if (tangentAttribute)
            geometry->addAttribute(tangentAttribute);
    }
}

bool needsTangentAttribute(const QGeometry *geometry,
                           QGeometryRenderer::PrimitiveType primitiveType)
{
    // only generate tangents for triangles
    if (primitiveType != QGeometryRenderer::Triangles &&
        primitiveType != QGeometryRenderer::TriangleStrip &&
        primitiveType != QGeometryRenderer::TriangleFan)
        return false;

    if (!geometry)
        return false;

    const auto positionAttr = attributeFromGeometry(QAttribute::defaultPositionAttributeName(), geometry);
    const auto normalAttr = attributeFromGeometry(QAttribute::defaultNormalAttributeName(), geometry);
    const auto uvAttr = attributeFromGeometry(QAttribute::defaultTextureCoordinateAttributeName(), geometry);
    const auto tangentAttr = attributeFromGeometry(QAttribute::defaultTangentAttributeName(), geometry);

    return positionAttr && normalAttr && uvAttr && (tangentAttr == nullptr);
}

bool generatePrecomputedTangentAttribute(QGeometryRenderer *mesh,
                                         GLTF2Context *context)
{
    QGeometry *geometry = mesh->geometry();
    if (!GLTF2Import::MeshParserUtils::needsTangentAttribute(geometry,
                                                             mesh->primitiveType()))
        return false;

    // get information about mesh name and primitive index within the original glTF file
    Kuesa::GLTF2Import::Mesh gltfMesh;
    int primitiveNumber;
    if (!getMeshGLTFInformation(mesh, context, gltfMesh, primitiveNumber))
        return false;
    if (gltfMesh.meshIdx < 0) {
        qCWarning(kuesa) << "Unable to find corresponding glTF Mesh for QGeometryRenderer";
        return false;
    }

    QJsonDocument jsonDocument = context->json();
    QJsonObject rootObject = jsonDocument.object();

    // create new attribute
    createTangentForGeometry(mesh->geometry(), mesh->primitiveType());
    auto *newTangentAttr = attributeFromGeometry(QAttribute::defaultTangentAttributeName(), mesh->geometry());
    if (newTangentAttr == nullptr)
        return false;

    const QByteArray bufferData = newTangentAttr->buffer()->data();
    const QString basePath = context->basePath();
    const QString bufferFileName = generateUniqueFilename(basePath, QStringLiteral("kuesaTangentBuffers.bin"));

    // Add Buffer
    const int bufferIdx = addJsonBuffer(rootObject,
                                        bufferData,
                                        bufferFileName,
                                        basePath + QDir::separator());
    if (bufferIdx < 0) {
        qCWarning(kuesa) << "unable to insert new buffer";
        return false;
    }
    context->addLocalFile(bufferFileName);

    // Add BufferView
    const int bufferViewIndex = addJsonBufferView(rootObject, bufferIdx, bufferData.size(), 0);

    // Add Accessor
    const int accessorIndex = addJsonAccessor(rootObject, bufferViewIndex, newTangentAttr);

    // modify mesh object and add attribute/accessor
    QJsonArray jsonMeshes = rootObject.value(KEY_MESHES).toArray();
    const qint32 meshIdx = gltfMesh.meshIdx;
    Q_ASSERT(meshIdx < jsonMeshes.size());
    // Build updated JsonObject for the glTF2 Mesh that references our GeometryRenderer
    QJsonObject jsonMesh = jsonMeshes.at(meshIdx).toObject();
    // replace matching primitive
    QJsonArray jsonPrimitives = jsonMesh.value(KEY_PRIMITIVES).toArray();
    QJsonObject jsonPrimitive = jsonPrimitives.at(primitiveNumber).toObject();
    QJsonObject jsonPrimAttrs = jsonPrimitive.value(KEY_ATTRIBUTES).toObject();
    jsonPrimAttrs[ATTR_TANGENT] = accessorIndex;
    jsonPrimitive[KEY_ATTRIBUTES] = jsonPrimAttrs;
    jsonPrimitives[primitiveNumber] = jsonPrimitive;
    jsonMesh[KEY_PRIMITIVES] = jsonPrimitives;
    jsonMeshes[meshIdx] = jsonMesh;

    rootObject[KEY_MESHES] = jsonMeshes;

    context->setJson(QJsonDocument(rootObject));

    return true;
}

bool geometryIsGLTF2Valid(QGeometry *geometry)
{
    const auto &attributes = geometry->attributes();
    const bool vertexBaseTypesAreValid = vertexBaseTypeForAttributesAreValid(attributes);
    const bool vertexSizesAreValid = vertexSizesForAttributesAreValid(attributes);
    return vertexSizesAreValid && vertexBaseTypesAreValid;
}

bool needsNormalAttribute(const QGeometry *geometry,
                          QGeometryRenderer::PrimitiveType primitiveType)
{
    // only generate tangents for triangles
    if (primitiveType != QGeometryRenderer::Triangles &&
        primitiveType != QGeometryRenderer::TriangleStrip &&
        primitiveType != QGeometryRenderer::TriangleFan)
        return false;

    if (!geometry)
        return false;

    return attributeFromGeometry(QAttribute::defaultNormalAttributeName(),
                                 geometry) == nullptr &&
            attributeFromGeometry(QAttribute::defaultPositionAttributeName(),
                                  geometry) != nullptr;
}

namespace {

quint32 indexValueAt(QAttribute::VertexBaseType type, const char *rawValue)
{
    switch (type) {
    case QAttribute::UnsignedInt:
        return *reinterpret_cast<const quint32*>(rawValue);
    case QAttribute::Int:
        return *reinterpret_cast<const qint32*>(rawValue);
    case QAttribute::UnsignedShort:
        return *reinterpret_cast<const quint16*>(rawValue);
    case QAttribute::Short:
        return *reinterpret_cast<const qint16*>(rawValue);
    case QAttribute::UnsignedByte:
        return *reinterpret_cast<const quint8*>(rawValue);
    case QAttribute::Byte:
        return *reinterpret_cast<const qint8*>(rawValue);
    default:
        Q_UNREACHABLE();
    }
}

template<QGeometryRenderer::PrimitiveType>
void convertIndexedGeometryToTriangleBasedGeometry(QGeometry *)
{
    Q_UNREACHABLE();
}

template<QGeometryRenderer::PrimitiveType>
void convertNonIndexedGeometryToTriangleBasedGeometry(QGeometry *)
{
    Q_UNREACHABLE();
}

struct NormalGenerationContext {
    int vertexCount = 0;
    int vertexByteSize = 0;
    int indexByteStride = 0;
    int newVertexCount = 0;

    struct VertexAttrInfo {
        QAttribute *attr;
        uint originalByteOffset;
        uint originalByteStride;
        uint actualByteSize;
        const char *rawData;
    };
    QAttribute *indexAttribute = nullptr;
    QVector<VertexAttrInfo> vertexAttributes;
    QGeometry *geometry = nullptr;

    void prepareFromIndexed(QGeometry *g)
    {
        geometry = g;
        const auto &attributes = geometry->attributes();
        vertexAttributes.reserve(attributes.size() - 1);

        // Compute Vertex Stride out of all Vertex Attributes
        // And retrive count of vertices
        for (QAttribute *attr : attributes) {
            switch (attr->attributeType()) {
            case QAttribute::VertexAttribute: {
                const uint rawSize = attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType());
                vertexByteSize += rawSize;
                vertexAttributes.push_back({ attr,
                                             attr->byteOffset(),
                                             std::max(attr->byteStride(), rawSize),
                                             rawSize,
                                             attr->buffer()->data().constData() });
                break;
            }
            case QAttribute::IndexAttribute: {
                vertexCount = attr->count();
                indexByteStride = std::max(attr->byteStride(), vertexBaseTypeSize(attr->vertexBaseType()));
                indexAttribute = attr;
            }
            default:
                break;
            }
        }
    }

    void prepareFromNonIndexed(QGeometry *g)
    {
        geometry = g;
        const auto &attributes = geometry->attributes();
        vertexAttributes.reserve(attributes.size() - 1);

        // Compute Vertex Stride out of all Vertex Attributes
        // And retrive count of vertices
        for (QAttribute *attr : attributes) {
            switch (attr->attributeType()) {
            case QAttribute::VertexAttribute: {
                const uint rawSize = attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType());
                vertexByteSize += rawSize;
                vertexAttributes.push_back({ attr,
                                             attr->byteOffset(),
                                             std::max(attr->byteStride(), rawSize),
                                             rawSize,
                                             attr->buffer()->data().constData() });
                if (attr->name() == QAttribute::defaultPositionAttributeName())
                    vertexCount = attr->count();
                break;
            }
            default:
                break;
            }
        }

        Q_ASSERT(vertexCount > 0);
    }

    void updateAttributes(const QByteArray &newVertexData)
    {
        Q_ASSERT(newVertexCount > 0);
        // Generate a new QBuffer to hold the data and set it on the attributes
        Qt3DGeometry::QBuffer *buffer = new Qt3DGeometry::QBuffer();
        buffer->setData(newVertexData);

        const auto &attributes = geometry->attributes();

        int byteOffset = 0;
        for (QAttribute *attr : attributes) {
            switch (attr->attributeType()) {
            case QAttribute::VertexAttribute: {
                // Update attributes to map properly against new buffer
                attr->setBuffer(buffer);
                attr->setByteStride(vertexByteSize);
                attr->setByteOffset(byteOffset);
                attr->setCount(newVertexCount);
                byteOffset += attr->vertexSize() * vertexBaseTypeSize(attr->vertexBaseType());
                break;
            }
            case QAttribute::IndexAttribute: {
                // Remove Index Attribute
                geometry->removeAttribute(attr);
            }
            default:
                break;
            }
        }
    }
};

template<>
void convertIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::Triangles>(QGeometry *geometry)
{
    NormalGenerationContext ctx;
    ctx.prepareFromIndexed(geometry);

    // Since we are already dealing with Triangles, vertex count stays the same
    ctx.newVertexCount = ctx.vertexCount;

    // Create raw buffer large enough
    QByteArray newVertexData;
    newVertexData.resize(ctx.newVertexCount * ctx.vertexByteSize);
    char *rawData = newVertexData.data();

    // Copy all vertex attributes data into new buffer
    QAttribute *indexAttribute = ctx.indexAttribute;
    const QByteArray &indexBufferData = indexAttribute->buffer()->data();
    const char *rawIndexData = indexBufferData.constData();
    for (uint i = 0, m = indexAttribute->count(); i < m; ++i) {
        const uint rawIdx = indexAttribute->byteOffset() + i * ctx.indexByteStride;
        const quint32 idx = indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawIdx);
        for (const NormalGenerationContext::VertexAttrInfo &attr : qAsConst(ctx.vertexAttributes)) {
            const char *attrData = attr.rawData + attr.originalByteOffset + idx * attr.originalByteStride;
            std::memcpy(rawData, attrData, attr.actualByteSize);
            rawData += attr.actualByteSize;
        }
    }

    // Update attributes/buffer based on new data
    ctx.updateAttributes(newVertexData);
}

template<>
void convertIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleStrip>(QGeometry *geometry)
{
    NormalGenerationContext ctx;
    ctx.prepareFromIndexed(geometry);

    // 3 First vertices form the first triangle, then each new vertex forms a
    // new triangle with the last two vertices.
    ctx.newVertexCount = 3 + (ctx.vertexCount - 3) * 3;

    // Create raw buffer large enough
    QByteArray newVertexData;
    newVertexData.resize(ctx.newVertexCount * ctx.vertexByteSize);
    char *rawData = newVertexData.data();

    // Copy all vertex attributes data into new buffer
    QAttribute *indexAttribute = ctx.indexAttribute;
    const QByteArray &indexBufferData = indexAttribute->buffer()->data();
    const char *rawIndexData = indexBufferData.constData();

    for (uint i = 0, m = indexAttribute->count(); i < m; ++i) {
        QVarLengthArray<quint32, 3> indices;
        if (i >= 3) {
            const uint rawIdxPrev = indexAttribute->byteOffset() + (i - 1) * ctx.indexByteStride;
            indices.push_back(indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawIdxPrev));
            const uint rawIdxPrevPrev = indexAttribute->byteOffset() + (i - 2) * ctx.indexByteStride;
            indices.push_back(indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawIdxPrevPrev));
        }
        const uint rawIdx = indexAttribute->byteOffset() + i * ctx.indexByteStride;
        indices.push_back(indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawIdx));

        for (const quint32 idx : indices) {
            for (const NormalGenerationContext::VertexAttrInfo &attr : qAsConst(ctx.vertexAttributes)) {
                const char *attrData = attr.rawData + attr.originalByteOffset + idx * attr.originalByteStride;
                std::memcpy(rawData, attrData, attr.actualByteSize);
                rawData += attr.actualByteSize;
            }
        }
    }

    // Update attributes/buffer based on new data
    ctx.updateAttributes(newVertexData);
}

template<>
void convertIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleFan>(QGeometry *geometry)
{
    NormalGenerationContext ctx;
    ctx.prepareFromIndexed(geometry);

    // 3 First vertices form the first triangle, then each new vertex forms a
    // new triangle with using the first vertex and the last vertex
    ctx.newVertexCount = 3 + (ctx.vertexCount - 3) * 3;

    // Create raw buffer large enough
    QByteArray newVertexData;
    newVertexData.resize(ctx.newVertexCount * ctx.vertexByteSize);
    char *rawData = newVertexData.data();

    // Copy all vertex attributes data into new buffer
    QAttribute *indexAttribute = ctx.indexAttribute;
    const QByteArray &indexBufferData = indexAttribute->buffer()->data();
    const char *rawIndexData = indexBufferData.constData();

    for (uint i = 0, m = indexAttribute->count(); i < m; ++i) {
        QVarLengthArray<quint32, 3> indices;
        const uint rawIdx = indexAttribute->byteOffset() + i * ctx.indexByteStride;
        if (i >= 3) {
            const uint rawFirstIdx = indexAttribute->byteOffset();
            indices.push_back(indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawFirstIdx));

            indices.push_back(indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawIdx));

            const uint rawIdxPrev = indexAttribute->byteOffset() + (i - 1) * ctx.indexByteStride;
            indices.push_back(indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawIdxPrev));
        } else {
            indices.push_back(indexValueAt(indexAttribute->vertexBaseType(), rawIndexData + rawIdx));
        }

        for (const quint32 idx : indices) {
            for (const NormalGenerationContext::VertexAttrInfo &attr : qAsConst(ctx.vertexAttributes)) {
                const char *attrData = attr.rawData + attr.originalByteOffset + idx * attr.originalByteStride;
                std::memcpy(rawData, attrData, attr.actualByteSize);
                rawData += attr.actualByteSize;
            }
        }
    }

    // Update attributes/buffer based on new data
    ctx.updateAttributes(newVertexData);
}

template<>
void convertNonIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleStrip>(QGeometry *geometry)
{
    NormalGenerationContext ctx;
    ctx.prepareFromNonIndexed(geometry);

    // 3 First vertices form the first triangle, then each new vertex forms a
    // new triangle with the last two vertices.
    ctx.newVertexCount = 3 + (ctx.vertexCount - 3) * 3;

    // Create raw buffer large enough
    QByteArray newVertexData;
    newVertexData.resize(ctx.newVertexCount * ctx.vertexByteSize);
    char *rawData = newVertexData.data();

    // Copy all vertex attributes data into new buffer
    for (uint i = 0, m = ctx.vertexCount; i < m; ++i) {
        QVarLengthArray<quint32, 3> indices;
        if (i >= 3) {
            indices.push_back(i - 1);
            indices.push_back(i - 2);
        }
        indices.push_back(i);
        for (const quint32 idx : indices) {
            for (const NormalGenerationContext::VertexAttrInfo &attr : qAsConst(ctx.vertexAttributes)) {
                const char *attrData = attr.rawData + attr.originalByteOffset + idx * attr.originalByteStride;
                std::memcpy(rawData, attrData, attr.actualByteSize);
                rawData += attr.actualByteSize;
            }
        }
    }

    // Update attributes/buffer based on new data
    ctx.updateAttributes(newVertexData);
}

template<>
void convertNonIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleFan>(QGeometry *geometry)
{
    NormalGenerationContext ctx;
    ctx.prepareFromNonIndexed(geometry);

    // 3 First vertices form the first triangle, then each new vertex forms a
    // new triangle with using the first vertex and the last vertex
    ctx.newVertexCount = 3 + (ctx.vertexCount - 3) * 3;

    // Create raw buffer large enough
    QByteArray newVertexData;
    newVertexData.resize(ctx.newVertexCount * ctx.vertexByteSize);
    char *rawData = newVertexData.data();

    // Copy all vertex attributes data into new buffer
    for (uint i = 0, m = ctx.vertexCount; i < m; ++i) {
        QVarLengthArray<quint32, 3> indices;
        if (i >= 3) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i - 1);
        } else {
            indices.push_back(i);
        }
        for (const quint32 idx : indices) {
            for (const NormalGenerationContext::VertexAttrInfo &attr : qAsConst(ctx.vertexAttributes)) {
                const char *attrData = attr.rawData + attr.originalByteOffset + idx * attr.originalByteStride;
                std::memcpy(rawData, attrData, attr.actualByteSize);
                rawData += attr.actualByteSize;
            }
        }
    }

    // Update attributes/buffer based on new data
    ctx.updateAttributes(newVertexData);
}

bool geometryHasIndexAttribute(QGeometry *geometry)
{
    const auto &attributes = geometry->attributes();

    // Check if we are sharing vertices between faces. This is the case if we
    // have an IndexAttribute or if the primitive type is a TriangleStrip or
    // TriangleFan
    const bool hasIndexAttribute = std::find_if(std::begin(attributes), std::end(attributes),
                                                [](const QAttribute *attr) {
                                                    return attr->attributeType() == QAttribute::IndexAttribute;
                                                }) != std::end(attributes);
    return hasIndexAttribute;
}

bool geometryHasSharedVertices(QGeometry *geometry,
                               QGeometryRenderer::PrimitiveType primitiveType)
{
    // Check if we are sharing vertices between faces. This is the case if we
    // have an IndexAttribute or if the primitive type is a TriangleStrip or
    // TriangleFan
    const bool hasIndexAttribute = geometryHasIndexAttribute(geometry);
    return (primitiveType != QGeometryRenderer::Triangles) || hasIndexAttribute;
}

void convertGeometryToTriangleBasedGeometry(QGeometry *geometry,
                                            QGeometryRenderer::PrimitiveType primitive)
{
    const bool hasIndexAttribute = geometryHasIndexAttribute(geometry);
    if (hasIndexAttribute) {
        switch (primitive) {
        case QGeometryRenderer::Triangles:
            convertIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::Triangles>(geometry);
            break;
        case QGeometryRenderer::TriangleStrip:
            convertIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleStrip>(geometry);
            break;
        case QGeometryRenderer::TriangleFan:
            convertIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleFan>(geometry);
            break;
        default:
            Q_UNREACHABLE();
        }
    } else {
        switch (primitive) {
        case QGeometryRenderer::TriangleStrip:
            convertNonIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleStrip>(geometry);
            break;
        case QGeometryRenderer::TriangleFan:
            convertNonIndexedGeometryToTriangleBasedGeometry<QGeometryRenderer::TriangleFan>(geometry);
            break;
        default:
            Q_UNREACHABLE();
        }
    }
}

QAttribute *generateNormalsForBaseMesh(const QAttribute *positionAttribute)
{
    if (positionAttribute->vertexBaseType() != QAttribute::Float ||
        positionAttribute->vertexSize() != 3) {
        qCWarning(kuesa) << "Currently only vec3 positions are supported, unable to generate normals.";
        return nullptr;
    }

    QByteArray rawNormals;
    rawNormals.resize(positionAttribute->count() * sizeof(QVector3D));
    QVector3D *normals = reinterpret_cast<QVector3D *>(rawNormals.data());

    QByteArray positionBuffer = positionAttribute->buffer()->data();
    const char *positionData = positionBuffer.constData();
    const uint positionByteOffset = positionAttribute->byteOffset();
    const uint positionByteStride = std::max(positionAttribute->byteStride(), uint(sizeof(QVector3D)));

    // Iterate over position attribute and compute normals
    // (Only handle vec3 types for now)
    Q_ASSERT(positionAttribute->count() % 3 == 0);
    for (int i = 0; i < positionAttribute->count(); i += 3) {
        QVarLengthArray<QVector3D, 3> positions;
        for (int j = 0; j < 3; ++j) {
            const char *rawPos = positionData + positionByteOffset + (i + j) * positionByteStride;
            const QVector3D p = *reinterpret_cast<const QVector3D *>(rawPos);
            positions.push_back(p);
        }

        // Compute normals from positions
        const QVector3D ab = positions[1] - positions[0];
        const QVector3D ac = positions[2] - positions[0];
        const QVector3D normal = QVector3D::crossProduct(ab, ac).normalized();

        // Insert computed normals (its a per face normal, the 3 triangle
        // vertices will have the same normal)
        for (int j = 0; j < 3; ++j) {
            *normals = normal;
            ++normals;
        }
    }

    // TO DO: use same buffer for all attributes
    // We use a separate buffer for normals:
    // An improvement would be to preallocate some space for normals
    // directly when converting from indexed of triangle stip/fan
    // However in the case the mesh was already provided as triangles this would
    // be a bit prolematic, this would have to be handled as well
    Qt3DGeometry::QBuffer *buffer = new Qt3DGeometry::QBuffer();
    buffer->setData(rawNormals);

    QAttribute *normalsAttribute = new QAttribute();
    normalsAttribute->setName(QAttribute::defaultNormalAttributeName());
    normalsAttribute->setCount(positionAttribute->count());
    normalsAttribute->setByteOffset(0);
    normalsAttribute->setByteStride(3 * sizeof(float));
    normalsAttribute->setVertexBaseType(QAttribute::Float);
    normalsAttribute->setVertexSize(3);
    normalsAttribute->setBuffer(buffer);
    normalsAttribute->setAttributeType(QAttribute::VertexAttribute);

    return normalsAttribute;
}

QAttribute *generateNormalsForMorphTarget(const QAttribute *positionAttribute, const QAttribute *normalsAttribute,
                                          const QAttribute *positionMorphAttribute, int morphTargetId)
{
    QByteArray rawMorphNormals;
    rawMorphNormals.resize(positionAttribute->count() * sizeof(QVector3D));
    QVector3D *morphNormals = reinterpret_cast<QVector3D *>(rawMorphNormals.data());

    QByteArray positionBuffer = positionAttribute->buffer()->data();
    const char *positionData = positionBuffer.constData();
    const uint positionByteOffset = positionAttribute->byteOffset();
    const uint positionByteStride = std::max(positionAttribute->byteStride(), uint(sizeof(QVector3D)));

    QByteArray positionMorphBuffer = positionMorphAttribute->buffer()->data();
    const char *positionMorphData = positionMorphBuffer.constData();
    const uint positionMorphByteOffset = positionMorphAttribute->byteOffset();
    const uint positionMorphByteStride = std::max(positionMorphAttribute->byteStride(), uint(sizeof(QVector3D)));

    QByteArray normalsBuffer = normalsAttribute->buffer()->data();
    const char *normalsData = normalsBuffer.constData();
    const uint normalsByteOffset = normalsAttribute->byteOffset();
    const uint normalsByteStride = std::max(normalsAttribute->byteStride(), uint(sizeof(QVector3D)));

    // Iterate over position attribute and compute normals
    // We need to update the position according to the morph target
    // And we need to subtract the original normal to the deformed normal to have the morphtarget
    // (Only handle vec3 types for now)
    Q_ASSERT(positionAttribute->count() % 3 == 0);
    for (int i = 0; i < positionAttribute->count(); i += 3) {
        QVarLengthArray<QVector3D, 3> positions;
        QVarLengthArray<QVector3D, 3> normals;
        for (int j = 0; j < 3; ++j) {
            const char *rawPos = positionData + positionByteOffset + (i + j) * positionByteStride;
            const QVector3D p = *reinterpret_cast<const QVector3D *>(rawPos);

            const char *rawMorphPos = positionMorphData + positionMorphByteOffset + (i + j) * positionMorphByteStride;
            const QVector3D morphP = *reinterpret_cast<const QVector3D *>(rawMorphPos);
            positions.push_back(p + morphP);

            const char *rawNormal = normalsData + normalsByteOffset + (i + j) * normalsByteStride;
            const QVector3D n = *reinterpret_cast<const QVector3D *>(rawNormal);
            normals.push_back(n);
        }

        // Compute normals from positions
        const QVector3D ab = positions[1] - positions[0];
        const QVector3D ac = positions[2] - positions[0];
        const QVector3D normal = QVector3D::crossProduct(ab, ac).normalized();

        // Insert computed normals (its a per face normal, the 3 triangle
        // vertices will have the same normal)
        for (int j = 0; j < 3; ++j) {
            *morphNormals = normal - normals[j];
            ++morphNormals;
        }
    }

    // TO DO: use same buffer for all attributes
    // We use a separate buffer for normals:
    // An improvement would be to preallocate some space for normals
    // directly when converting from indexed of triangle stip/fan
    // However in the case the mesh was already provided as triangles this would
    // be a bit prolematic, this would have to be handled as well
    Qt3DGeometry::QBuffer *buffer = new Qt3DGeometry::QBuffer();
    buffer->setData(rawMorphNormals);

    const QString attributeName = QStringLiteral("%1_%2")
                                          .arg(QAttribute::defaultNormalAttributeName())
                                          .arg(morphTargetId + 1);

    QAttribute *normalsMorphAttribute = new QAttribute();
    normalsMorphAttribute->setName(attributeName);
    normalsMorphAttribute->setCount(positionAttribute->count());
    normalsMorphAttribute->setByteOffset(0);
    normalsMorphAttribute->setByteStride(3 * sizeof(float));
    normalsMorphAttribute->setVertexBaseType(QAttribute::Float);
    normalsMorphAttribute->setVertexSize(3);
    normalsMorphAttribute->setBuffer(buffer);
    normalsMorphAttribute->setAttributeType(QAttribute::VertexAttribute);

    return normalsMorphAttribute;
}

void generateNormals(QGeometry *geometry)
{
    const QAttribute *positionAttribute = attributeFromGeometry(QAttribute::defaultPositionAttributeName(),
                                                                geometry);

    if (positionAttribute == nullptr) {
        qCWarning(kuesa) << "No position attribute found on geometry, unable to generate normals.";
        Q_UNREACHABLE();
    }

    auto normalsAttribute = generateNormalsForBaseMesh(positionAttribute);
    if (normalsAttribute == nullptr)
        return;

    geometry->addAttribute(normalsAttribute);

    // Compute normals for morph targets
    for (int morphTargetId = 0; morphTargetId < 8; ++morphTargetId) {
        const QString attributeName = QStringLiteral("%1_%2")
                                              .arg(QAttribute::defaultPositionAttributeName())
                                              .arg(morphTargetId + 1);
        const QAttribute *positionMorphAttribute = attributeFromGeometry(attributeName,
                                                                                     geometry);
        if (positionMorphAttribute != nullptr) {
            auto normalMorphAttribute = generateNormalsForMorphTarget(positionAttribute, normalsAttribute, positionMorphAttribute, morphTargetId);
            geometry->addAttribute(normalMorphAttribute);
        }
    }
}

} // namespace

void createNormalsForGeometry(QGeometry *geometry,
                              QGeometryRenderer::PrimitiveType primitiveType)
{
    const bool hasSharedVertices = geometryHasSharedVertices(geometry, primitiveType);
    if (hasSharedVertices) {
        // This step gets rid of the IndexAttribute if we had one which has no use
        // anymore since every entry will be unique
        // The new geometry content we will generate will be Triangle based
        convertGeometryToTriangleBasedGeometry(geometry, primitiveType);
    }

    // Generate normals on triangle based geometry
    generateNormals(geometry);
}

bool generatePrecomputedNormalAttribute(QGeometryRenderer *mesh, GLTF2Context *context)
{
    QGeometry *geometry = mesh->geometry();
    if (!GLTF2Import::MeshParserUtils::needsNormalAttribute(geometry,
                                                            mesh->primitiveType()))
        return false;

    // get information about mesh name and primitive index within the original glTF file
    Kuesa::GLTF2Import::Mesh gltfMesh;
    int primitiveNumber;
    if (!getMeshGLTFInformation(mesh, context, gltfMesh, primitiveNumber))
        return false;
    if (gltfMesh.meshIdx < 0) {
        qCWarning(kuesa) << "Unable to find corresponding glTF Mesh for QGeometryRenderer";
        return false;
    }

    QJsonDocument jsonDocument = context->json();
    QJsonObject rootObject = jsonDocument.object();
    QJsonArray jsonMeshes = rootObject.value(KEY_MESHES).toArray();
    const qint32 meshIdx = gltfMesh.meshIdx;
    Q_ASSERT(meshIdx < jsonMeshes.size());
    QJsonObject jsonMesh = jsonMeshes.at(meshIdx).toObject();
    QJsonArray jsonPrimitives = jsonMesh.value(KEY_PRIMITIVES).toArray();
    QJsonObject jsonPrimitive = jsonPrimitives.at(primitiveNumber).toObject();
    QJsonObject jsonPrimAttrs = jsonPrimitive.value(KEY_ATTRIBUTES).toObject();

    const bool hasSharedVertices = geometryHasSharedVertices(geometry,
                                                             mesh->primitiveType());
    // Generate normals
    createNormalsForGeometry(geometry, mesh->primitiveType());

    if (hasSharedVertices) {
        // To create normals in the case some vertices were shared, we have to
        // regenerate all other attributes and combine them in a single buffer,
        // also changing the primitive type to Triangles
        // For these reasons, we need to rewrite completely the buffer views,
        // accessors and meshes
        const auto attributes = geometry->attributes();
        int bufferIdx = -1;
        int bufferSize = -1;

        // Clear previous attributes for mesh in JSON
        jsonPrimAttrs = {};
        // Remove index buffer reference in jsonPrimitive
        jsonPrimitive.remove(KEY_INDICES);

        for (const QAttribute *attr : attributes) {
            // All attributes are backed by the same buffer except the normal
            // attribute which uses its own buffer
            if (attr->name() != QAttribute::defaultNormalAttributeName()) {
                // Write buffer if not done already
                if (bufferIdx < 0) {
                    const QByteArray bufferData = attr->buffer()->data();
                    const QString basePath = context->basePath();
                    const QString bufferFileName = generateUniqueFilename(basePath, QLatin1String("kuesaMeshNoSharedVerticesBuffers.bin"));
                    bufferSize = bufferData.size();

                    // Add Buffer
                    bufferIdx = addJsonBuffer(rootObject,
                                              bufferData,
                                              bufferFileName,
                                              basePath + QDir::separator());
                    if (bufferIdx < 0) {
                        qCWarning(kuesa) << "unable to insert new buffer";
                        return false;
                    }
                    context->addLocalFile(bufferFileName);
                }
                // Add BufferView
                const int bufferViewIndex = addJsonBufferView(rootObject, bufferIdx, bufferSize, 0);
                // Add Accessor
                const int accessorIndex = addJsonAccessor(rootObject, bufferViewIndex, attr);
                // Add updated Mesh Primitive
                const QLatin1String gltfAttributeName = glTFAttributeNameFromQt3DAttribute(attr);

                // replace matching primitive
                jsonPrimAttrs[gltfAttributeName] = accessorIndex;
            }
        }
    }

    // Write Normals Attribute
    {
        // We can get away by writing only the normals attribute
        QAttribute *normalAttribute = attributeFromGeometry(QAttribute::defaultNormalAttributeName(),
                                                                        geometry);
        if (!normalAttribute)
            return false;

        const QByteArray bufferData = normalAttribute->buffer()->data();
        const QString basePath = context->basePath();
        const QString bufferFileName = generateUniqueFilename(basePath, QLatin1String("kuesaMeshNormalsBuffers.bin"));

        // Add Buffer
        const int bufferIdx = addJsonBuffer(rootObject,
                                            bufferData,
                                            bufferFileName,
                                            basePath + QDir::separator());
        if (bufferIdx < 0) {
            qCWarning(kuesa) << "unable to insert new buffer";
            return false;
        }
        context->addLocalFile(bufferFileName);

        // Add BufferView
        const int bufferViewIndex = addJsonBufferView(rootObject, bufferIdx, bufferData.size(), 0);
        // Add Accessor
        const int accessorIndex = addJsonAccessor(rootObject, bufferViewIndex, normalAttribute);
        // Add updated Mesh Primitive
        const QLatin1String gltfAttributeName = glTFAttributeNameFromQt3DAttribute(normalAttribute);

        // replace matching primitive
        jsonPrimAttrs[gltfAttributeName] = accessorIndex;
    }

    jsonPrimitive[KEY_ATTRIBUTES] = jsonPrimAttrs;
    jsonPrimitives[primitiveNumber] = jsonPrimitive;
    jsonMesh[KEY_PRIMITIVES] = jsonPrimitives;
    jsonMeshes[meshIdx] = jsonMesh;
    rootObject[KEY_MESHES] = jsonMeshes;
    context->setJson(QJsonDocument(rootObject));

    return true;
}

} // namespace MeshParserUtils
} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE
