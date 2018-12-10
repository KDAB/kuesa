/*
    dracocompressor_p.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Jose Casafranca <juan.casafranca@kdab.com>

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

#include <draco/mesh/mesh.h>
#include <draco/attributes/geometry_attribute.h>
#include <draco/attributes/geometry_indices.h>
#include <draco/io/mesh_io.h>
#include "dracocompressor_p.h"
#include "gltf2exporter_p.h"
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <QVector>
#include <QFile>
#include <QLoggingCategory>
#include <QDataStream>
#include <fstream>
#include <iterator>
#include <iostream>

QT_BEGIN_NAMESPACE
Q_LOGGING_CATEGORY(dracocompressor, "DracoCompressor")
namespace Kuesa {
namespace DracoCompression {
namespace {
draco::GeometryAttribute::Type attributeTypeFromName(const QString &attributeName)
{
    auto type = draco::GeometryAttribute::Type::INVALID;

    if (attributeName == Qt3DRender::QAttribute::defaultPositionAttributeName())
        type = draco::GeometryAttribute::Type::POSITION;
    else if (attributeName == Qt3DRender::QAttribute::defaultNormalAttributeName())
        type = draco::GeometryAttribute::Type::NORMAL;
    else if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
        type = draco::GeometryAttribute::Type::COLOR;
    else if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName())
        type = draco::GeometryAttribute::Type::TEX_COORD;
    else if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinate1AttributeName())
        type = draco::GeometryAttribute::Type::TEX_COORD;
    else if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinate2AttributeName())
        type = draco::GeometryAttribute::Type::TEX_COORD;
    else if (attributeName == Qt3DRender::QAttribute::defaultTangentAttributeName())
        type = draco::GeometryAttribute::Type::GENERIC;
    else if (attributeName == Qt3DRender::QAttribute::defaultJointWeightsAttributeName())
        type = draco::GeometryAttribute::Type::GENERIC;
    else if (attributeName == Qt3DRender::QAttribute::defaultJointIndicesAttributeName())
        type = draco::GeometryAttribute::Type::GENERIC;

    return type;
}

template<typename T>
struct attribute_type_trait {
    static constexpr draco::DataType type = draco::DataType::DT_INVALID;
};

template<>
struct attribute_type_trait<float> {
    static constexpr draco::DataType type = draco::DataType::DT_FLOAT32;
};

template<>
struct attribute_type_trait<char> {
    static constexpr draco::DataType type = draco::DataType::DT_INT8;
};

template<>
struct attribute_type_trait<unsigned char> {
    static constexpr draco::DataType type = draco::DataType::DT_UINT8;
};

template<>
struct attribute_type_trait<unsigned short> {
    static constexpr draco::DataType type = draco::DataType::DT_UINT16;
};

template<>
struct attribute_type_trait<short> {
    static constexpr draco::DataType type = draco::DataType::DT_INT16;
};

template<>
struct attribute_type_trait<int> {
    static constexpr draco::DataType type = draco::DataType::DT_INT32;
};

template<>
struct attribute_type_trait<unsigned int> {
    static constexpr draco::DataType type = draco::DataType::DT_UINT32;
};

template<>
struct attribute_type_trait<double> {
    static constexpr draco::DataType type = draco::DataType::DT_FLOAT64;
};

template<typename T, typename Sz>
static constexpr Sz actualStride(Sz glStride, Sz vertexSize)
{
    return glStride == 0 ? vertexSize * Sz(sizeof(T)) : glStride;
}

template<typename T>
QByteArray packedDataInBuffer(const QByteArray &inputBuffer,
                              std::size_t vertexSize,
                              std::size_t offset,
                              std::size_t stride,
                              std::size_t count)
{
    const auto elementSize = vertexSize * sizeof(T);

    Q_ASSERT_X(count < std::numeric_limits<int>::max() / elementSize, "Kuesa::packedDataInBuffer", "Too many elements for a QByteArray");

    QByteArray outputBuffer(int(count * elementSize), Qt::Uninitialized);

    auto byteMarkerPosition = offset;
    stride = actualStride<T>(stride, vertexSize);

    for (std::size_t i = 0; i < count; ++i) {
        std::memcpy(&(outputBuffer.data()[i * elementSize]), &(inputBuffer.data()[byteMarkerPosition]), elementSize);
        byteMarkerPosition += stride;
    }

    return outputBuffer;
}

template<typename T>
std::pair<QString, int> addAttributeToMesh(const Qt3DRender::QAttribute &attribute, draco::Mesh &mesh)
{
    const auto dracoAttributeType = attributeTypeFromName(attribute.name());
    const auto dracoDataType = attribute_type_trait<T>::type;

    if (dracoAttributeType == draco::GeometryAttribute::Type::POSITION)
        mesh.set_num_points(attribute.count());

    const auto attributeBuffer = attribute.buffer();

    const QByteArray packedData = packedDataInBuffer<T>(attributeBuffer->data(),
                                                        attribute.vertexSize(),
                                                        attribute.byteOffset(),
                                                        attribute.byteStride(),
                                                        attribute.count());

    draco::PointAttribute meshAttribute;
    meshAttribute.Init(attributeTypeFromName(attribute.name()),
                       nullptr,
                       static_cast<int8_t>(attribute.vertexSize()),
                       dracoDataType,
                       false,
                       actualStride<T>(attribute.byteStride(), attribute.vertexSize()),
                       0);

    auto attId = mesh.AddAttribute(meshAttribute, true, attribute.count());
    if (attId == -1)
        return { {}, -1 };

    mesh.attribute(attId)->buffer()->Write(0, packedData.data(), static_cast<std::size_t>(packedData.size()));

    return { attribute.property("semanticName").toString(), attId };
}

template<typename T>
QVector<draco::PointIndex> createIndicesVectorFromAttribute(const Qt3DRender::QAttribute &attribute)
{
    const auto byteOffset = attribute.byteOffset();
    const auto data = attribute.buffer()->data().mid(int(byteOffset));
    const auto attrCount = attribute.count();
    const int stride = actualStride<T>(int(attribute.byteStride()), 1);

    Q_ASSERT_X(attrCount < std::numeric_limits<int>::max(), "Kuesa::createIndicesVectorFromAttribute", "Too many elements for a QVector");

    QVector<draco::PointIndex> indicesVector(static_cast<int>(attrCount));

    for (int i = 0, n = int(attrCount); i < n; ++i)
        indicesVector[i] = *reinterpret_cast<const T *>(data.data() + i * stride);

    return indicesVector;
}

void addFacesToMesh(const QVector<draco::PointIndex> &indices, draco::Mesh &dracoMesh)
{
    for (int i = 0, nbIndices = indices.size() / 3; i < nbIndices; ++i)
        dracoMesh.AddFace({ indices[3 * i + 0],
                            indices[3 * i + 1],
                            indices[3 * i + 2] });
}

template<typename T>
bool compressAttribute(
        const Qt3DRender::QAttribute &attribute,
        draco::Mesh &dracoMesh,
        std::vector<std::pair<QString, int>>& attributes)
{
    auto compressedAttr = addAttributeToMesh<T>(attribute, dracoMesh);
    if (compressedAttr.second == -1) {
        qCWarning(dracocompressor) << "Could not add attribute to mesh: " << attribute.name();
        return false;
    }
    attributes.push_back(compressedAttr);
    return true;
}

Q_DECL_RELAXED_CONSTEXPR
draco::GeometryAttribute::Type dracoAttribute(GLTF2ExportConfiguration::MeshAttribute m)
{
    switch (m) {
    case GLTF2ExportConfiguration::Position:
        return draco::GeometryAttribute::POSITION;
    case GLTF2ExportConfiguration::Normal:
        return draco::GeometryAttribute::NORMAL;
    case GLTF2ExportConfiguration::TextureCoordinate:
        return draco::GeometryAttribute::TEX_COORD;
    case GLTF2ExportConfiguration::Color:
        return draco::GeometryAttribute::COLOR;
    case GLTF2ExportConfiguration::Generic:
        return draco::GeometryAttribute::GENERIC;
    }
    return draco::GeometryAttribute::INVALID;
}
} // namespace

CompressedMesh compressMesh(
        const Qt3DRender::QGeometry &geometry,
        const GLTF2ExportConfiguration &conf)
{
    std::unique_ptr<draco::EncoderBuffer> compressBuffer(new draco::EncoderBuffer);
    std::vector<std::pair<QString, int>> attributes;
    draco::Mesh dracoMesh;

    // Convert Qt3D geometry to draco mesh
    const auto geometryAttributes = geometry.attributes();
    const Qt3DRender::QAttribute *indicesAttribute = nullptr;

    for (const auto *attribute : geometryAttributes) {
        if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            dracoMesh.set_num_points(attribute->count());
            break;
        }
    }

    for (const auto *attribute : geometryAttributes) {
        if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
            indicesAttribute = attribute;
            continue;
        }

        switch (attribute->vertexBaseType()) {
        case Qt3DRender::QAttribute::VertexBaseType::Float: {
            if(!compressAttribute<float>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::Byte: {
            static_assert(CHAR_MIN < 0, "This code only works on platforms with signed char");
            if(!compressAttribute<char>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::UnsignedByte: {
            if(!compressAttribute<unsigned char>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::Short: {
            if(!compressAttribute<short>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::UnsignedShort: {
            if(!compressAttribute<unsigned short>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::Int: {
            if(!compressAttribute<int>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::UnsignedInt: {
            if(!compressAttribute<unsigned int>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::Double: {
            if(!compressAttribute<double>(*attribute, dracoMesh, attributes))
                return {};
            break;
        }
        case Qt3DRender::QAttribute::VertexBaseType::HalfFloat:
        default:
            qCWarning(dracocompressor) << "Warning: skipping attribute" << attribute->name() << "of unhandled type" << attribute->vertexBaseType();
            break;
        }
    }

    if (indicesAttribute != nullptr) {
        switch (indicesAttribute->vertexBaseType()) {
        case Qt3DRender::QAttribute::VertexBaseType::Byte:
            addFacesToMesh(createIndicesVectorFromAttribute<char>(*indicesAttribute), dracoMesh);
            break;
        case Qt3DRender::QAttribute::VertexBaseType::UnsignedByte:
            addFacesToMesh(createIndicesVectorFromAttribute<unsigned char>(*indicesAttribute), dracoMesh);
            break;
        case Qt3DRender::QAttribute::VertexBaseType::Short:
            addFacesToMesh(createIndicesVectorFromAttribute<short>(*indicesAttribute), dracoMesh);
            break;
        case Qt3DRender::QAttribute::VertexBaseType::UnsignedShort:
            addFacesToMesh(createIndicesVectorFromAttribute<unsigned short>(*indicesAttribute), dracoMesh);
            break;
        case Qt3DRender::QAttribute::VertexBaseType::Int:
            addFacesToMesh(createIndicesVectorFromAttribute<int>(*indicesAttribute), dracoMesh);
            break;
        case Qt3DRender::QAttribute::VertexBaseType::UnsignedInt:
            addFacesToMesh(createIndicesVectorFromAttribute<unsigned int>(*indicesAttribute), dracoMesh);
            break;
        default:
            break;
        }
    }

    draco::Encoder encoder;

    auto setQuantization = [&](GLTF2ExportConfiguration::MeshAttribute attr) {
        int q = conf.attributeQuantizationLevel(attr);
        if (q > 0)
            encoder.SetAttributeQuantization(dracoAttribute(attr), q);
    };
    setQuantization(GLTF2ExportConfiguration::Position);
    setQuantization(GLTF2ExportConfiguration::Normal);
    setQuantization(GLTF2ExportConfiguration::Color);
    setQuantization(GLTF2ExportConfiguration::TextureCoordinate);
    setQuantization(GLTF2ExportConfiguration::Generic);

    encoder.SetSpeedOptions(conf.meshEncodingSpeed(), conf.meshDecodingSpeed());

    encoder.SetTrackEncodedProperties(true);
    const auto status = encoder.EncodeMeshToBuffer(dracoMesh, compressBuffer.get());
    if (!status.ok()) {
        qCWarning(dracocompressor) << status.code() << status.error_msg();
        return {};
    }

    return { std::move(compressBuffer), attributes };
}

} // namespace DracoCompression
} // namespace Kuesa
QT_END_NAMESPACE
