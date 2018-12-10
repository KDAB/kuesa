#include "dracocompressor.h"

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <draco/mesh/mesh.h>
#include <draco/attributes/geometry_attribute.h>
#include <draco/attributes/geometry_indices.h>
#include <draco/io/mesh_io.h>
#include <QVector>
#include <QFile>
#include <QDataStream>
#include <fstream>
#include <iterator>
#include <iostream>

namespace {
draco::GeometryAttribute::Type attributeTypeFromName(const QString &attributeName)
{
    auto type = draco::GeometryAttribute::Type::INVALID;

    if (attributeName == Qt3DRender::QAttribute::defaultPositionAttributeName())
        type = draco::GeometryAttribute::Type::POSITION;
    if (attributeName == Qt3DRender::QAttribute::defaultNormalAttributeName())
        type = draco::GeometryAttribute::Type::NORMAL;
    if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinate1AttributeName())
        type = draco::GeometryAttribute::Type::TEX_COORD;
    if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
        type = draco::GeometryAttribute::Type::COLOR;

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

template<typename T>
QByteArray packedDataInBuffer(const QByteArray &inputBuffer,
                              int vertexSize,
                              int offset,
                              int stride,
                              int count)
{
    QByteArray outputBuffer;
    outputBuffer.resize(count * vertexSize * sizeof(T));

    int byteMarkerPosition = offset;
    stride = (stride == 0) ? vertexSize * sizeof(T) : stride;

    for (int i = 0; i < count; ++i) {
        std::memcpy(&(outputBuffer.data()[i * vertexSize * sizeof(T)]), &(inputBuffer.data()[byteMarkerPosition]), vertexSize * sizeof(T));
        byteMarkerPosition += stride;
    }

    T *typedData = reinterpret_cast<T *>(outputBuffer.data());
    for (int i = 0; i < vertexSize * count; ++i) {
        qDebug() << typedData[i];
    }

    return outputBuffer;
}

template<typename T>
void addAttributeToMesh(const Qt3DRender::QAttribute &attribute, draco::Mesh &mesh)
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
                       attribute.vertexSize(),
                       dracoDataType,
                       false,
                       attribute.vertexSize() * sizeof(T),
                       0);
    auto attId = mesh.AddAttribute(meshAttribute, true, attribute.count());
    for (unsigned int i = 0; i < attribute.count(); ++i)
        mesh.attribute(attId)->SetAttributeValue(draco::AttributeValueIndex(i), &packedData.data()[i * attribute.vertexSize() * sizeof(T)]);
}

template<typename T>
QVector<T> createIndicesVectorFromAttribute(const Qt3DRender::QAttribute &attribute)
{
    const auto byteStride = attribute.byteStride();
    const auto byteOffset = attribute.byteOffset();
    const auto data = attribute.buffer()->data().mid(byteOffset);

    QVector<T> indicesVector;
    indicesVector.resize(attribute.count());

    if (byteStride == 0)
        for (int i = 0, nbIndices = indicesVector.size(); i < nbIndices; ++i)
            indicesVector[i] = static_cast<T>(data.data()[i * sizeof(T)]);
    else
        for (int i = 0, nbIndices = indicesVector.size(); i < nbIndices; ++i)
            indicesVector[i] = static_cast<T>(data.data()[i * byteStride]);

    return indicesVector;
}

template<typename T>
void addFacesToMesh(const QVector<T> &indices, draco::Mesh &dracoMesh)
{
    for (int i = 0, nbIndices = indices.size() / 3; i < nbIndices; ++i)
        dracoMesh.AddFace({ static_cast<draco::IndexType<unsigned int, draco::PointIndex_tag_type_>>(indices[3 * i + 0]),
                            static_cast<draco::IndexType<unsigned int, draco::PointIndex_tag_type_>>(indices[3 * i + 1]),
                            static_cast<draco::IndexType<unsigned int, draco::PointIndex_tag_type_>>(indices[3 * i + 2]) });
}
} // namespace

std::unique_ptr<draco::EncoderBuffer> compressMesh(const Qt3DRender::QGeometry &geometry)
{
    std::unique_ptr<draco::EncoderBuffer> compressBuffer = std::unique_ptr<draco::EncoderBuffer>(new draco::EncoderBuffer);
    draco::Mesh dracoMesh;

    std::unordered_map<Qt3DRender::QBuffer *, draco::DataBuffer *> attributeBufferToDataBuffer;
    // Convert Qt3D geometry to draco mesh
    const auto geometryAttributes = geometry.attributes();
    Qt3DRender::QAttribute *indicesAttribute = nullptr;
    for (const auto attribute : geometryAttributes) {
        if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName())
            dracoMesh.set_num_points(attribute->count());
    }

    for (const auto attribute : geometryAttributes) {
        if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
            indicesAttribute = attribute;
            continue;
        }

        switch (attribute->vertexBaseType()) {
        case Qt3DRender::QAttribute::VertexBaseType::Float:
            addAttributeToMesh<float>(*attribute, dracoMesh);
            break;
        default:
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
        }
    }

    auto bb = dracoMesh.ComputeBoundingBox();
    qDebug() << bb.max_point()[0]
             << bb.max_point()[1]
             << bb.max_point()[2];

    qDebug() << bb.min_point()[0]
             << bb.min_point()[1]
             << bb.min_point()[2];

    draco::Encoder encoder;
    encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION, 5);
    encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL, 5);
    encoder.SetTrackEncodedProperties(true);
    const auto status = encoder.EncodeMeshToBuffer(dracoMesh, compressBuffer.get());
    if (!status.ok()) {
        qDebug() << status.code();
        qDebug() << status.error_msg();
    }

    qDebug() << "Mesh properties";
    qDebug() << "Faces" << dracoMesh.num_faces();
    qDebug() << "Attributes" << dracoMesh.num_attributes();
    qDebug() << "Points" << dracoMesh.num_points();

    qDebug() << "Encoded data";
    qDebug() << "Encoded faces" << encoder.num_encoded_faces();
    qDebug() << "Encoded points" << encoder.num_encoded_points();

    std::ofstream stream;
    stream.open("/home/kdab/data.drc");
    draco::WriteMeshIntoStream(&dracoMesh, stream);
    return compressBuffer;
}
