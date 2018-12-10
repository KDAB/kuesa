#include "dracocompressor.h"

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <draco/mesh/mesh.h>
#include <draco/attributes/geometry_attribute.h>
#include <draco/attributes/geometry_indices.h>
#include <QVector>

namespace
{
draco::GeometryAttribute::Type attributeTypeFromName(const QString &attributeName)
{
    auto type = draco::GeometryAttribute::Type::INVALID;

    if (attributeName == Qt3DRender::QAttribute::defaultPositionAttributeName())
        type = draco::GeometryAttribute::Type::POSITION;
    if (attributeName == Qt3DRender::QAttribute::defaultNormalAttributeName())
        type = draco::GeometryAttribute::Type::NORMAL;
    if (attributeName == Qt3DRender::QAttribute::defaultTextureCoordinate1AttributeName())
        type =  draco::GeometryAttribute::Type::TEX_COORD;
    if (attributeName == Qt3DRender::QAttribute::defaultColorAttributeName())
        type = draco::GeometryAttribute::Type::COLOR;

    return type;
}

draco::DataType dataTypeFromVertexBaseType(Qt3DRender::QAttribute::VertexBaseType vertexBaseType)
{
    auto dataType = draco::DataType::DT_INVALID;
    switch (vertexBaseType) {
    case Qt3DRender::QAttribute::VertexBaseType::Byte:
        dataType = draco::DataType::DT_INT8;
        break;
    case Qt3DRender::QAttribute::VertexBaseType::UnsignedByte:
        dataType = draco::DataType::DT_UINT8;
        break;
    case Qt3DRender::QAttribute::VertexBaseType::Short:
        dataType = draco::DataType::DT_INT16;
        break;
    case Qt3DRender::QAttribute::VertexBaseType::UnsignedShort:
        dataType = draco::DataType::DT_UINT16;
        break;
    case Qt3DRender::QAttribute::VertexBaseType::Int:
        dataType = draco::DataType::DT_INT32;
        break;
    case Qt3DRender::QAttribute::VertexBaseType::UnsignedInt:
        dataType = draco::DataType::DT_UINT32;
        break;
    case Qt3DRender::QAttribute::VertexBaseType::Float:
        dataType = draco::DataType::DT_FLOAT32;
        break;
    case Qt3DRender::QAttribute::VertexBaseType::Double:
        dataType = draco::DataType::DT_FLOAT64;
        break;
    }

    return dataType;
}

draco::DataBuffer *createDracoBufferFromQBuffer(const Qt3DRender::QBuffer &qbuffer)
{
    auto dracoDataBuffer = new draco::DataBuffer;
    dracoDataBuffer->Update(qbuffer.data().data(), qbuffer.data().size());
    return dracoDataBuffer;
}

template <typename T>
QVector<T> createIndicesVectorFromAttribute(const Qt3DRender::QAttribute &attribute)
{
    const auto byteStride = attribute.byteStride();
    const auto byteOffset = attribute.byteOffset();
    const auto data = attribute.buffer()->data().mid(byteOffset);

    QVector<T> indicesVector;
    indicesVector.resize(attribute.count());

    if (byteStride == 0)
        for (int i = 0, nbIndices = indicesVector.size(); i < nbIndices; ++i)
            indicesVector[i] = static_cast<T>(data.data()[i*sizeof(T)]);
    else
        for (int i = 0, nbIndices = indicesVector.size(); i < nbIndices; ++i)
            indicesVector[i] = static_cast<T>(data.data()[i*byteStride]);

    return indicesVector;
}

template <typename T>
void addFacesToMesh(const QVector<T> &indices, draco::Mesh &dracoMesh)
{
    for (int i, nbIndices = indices.size()/3; i < nbIndices; ++i)
        dracoMesh.AddFace({static_cast<draco::IndexType<unsigned int, draco::PointIndex_tag_type_>>(indices[3*i+0]),
                           static_cast<draco::IndexType<unsigned int, draco::PointIndex_tag_type_>>(indices[3*i+1]),
                           static_cast<draco::IndexType<unsigned int, draco::PointIndex_tag_type_>>(indices[3*i+2])});
}
}

std::unique_ptr<draco::EncoderBuffer> compressMesh(const Qt3DRender::QGeometry &geometry)
{
    std::unique_ptr<draco::EncoderBuffer> compressBuffer = std::make_unique<draco::EncoderBuffer>();
    draco::Mesh dracoMesh;

    std::unordered_map<Qt3DRender::QBuffer *, draco::DataBuffer *> attributeBufferToDataBuffer;
    // Convert Qt3D geometry to draco mesh
    const auto geometryAttributes = geometry.attributes();
    Qt3DRender::QAttribute *indicesAttribute = nullptr;
    for (const auto attribute : geometryAttributes) {

        const auto dracoAttributeType = attributeTypeFromName(attribute->name());

        if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute)
            indicesAttribute = attribute;

        const auto dracoDataType = dataTypeFromVertexBaseType(attribute->vertexBaseType());
        if (dracoAttributeType == draco::GeometryAttribute::Type::INVALID ||
                        dracoDataType == draco::DataType::DT_INVALID)
            continue;

        const auto attributeBuffer = attribute->buffer();
        if (attributeBufferToDataBuffer.find(attributeBuffer) == std::end(attributeBufferToDataBuffer))
            attributeBufferToDataBuffer[attributeBuffer] = createDracoBufferFromQBuffer(*attributeBuffer);

        const auto dracoBuffer = attributeBufferToDataBuffer[attributeBuffer];

        auto meshAttribute = std::make_unique<draco::GeometryAttribute>();
        meshAttribute->Init(dracoAttributeType,
                            dracoBuffer,
                            attribute->vertexSize(),
                            dracoDataType,
                            false,
                            attribute->byteStride(),
                            attribute->byteOffset());
        dracoMesh.AddAttribute(*meshAttribute.get(), true, attribute->count());
    }

    if (indicesAttribute != nullptr)
    {
        dracoMesh.set_num_points(indicesAttribute->count()/3);
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

    draco::Encoder encoder;
    encoder.EncodeMeshToBuffer(dracoMesh, compressBuffer.get());

    return compressBuffer;
}
