/*
    bufferaccessorparser.cpp

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

#include "bufferaccessorparser_p.h"

#include "kuesa_p.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

#include <QOpenGLContext>

#include "gltf2context_p.h"
#include "gltf2keys_p.h"
#include "gltf2utils_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

QVector<float> jsonArrayToVectorOfFloats(const QJsonArray &values)
{
    const int nbComponents = values.size();
    QVector<float> d(nbComponents);
    std::transform(values.begin(), values.end(), d.begin(),
                   [](const QJsonValue &value) -> float {
                       return static_cast<float>(value.toDouble(0));
                   });
    return d;
}

quint8 accessorDataSizeFromJson(const QString &type)
{
    const QString typeName = type.toUpper();
    if (typeName == QLatin1String("SCALAR"))
        return 1;
    if (typeName == QLatin1String("VEC2"))
        return 2;
    if (typeName == QLatin1String("VEC3"))
        return 3;
    if (typeName == QLatin1String("VEC4"))
        return 4;
    if (typeName == QLatin1String("MAT2"))
        return 4;
    if (typeName == QLatin1String("MAT3"))
        return 9;
    if (typeName == QLatin1String("MAT4"))
        return 16;
    qCWarning(kuesa) << "Unrecognized data type";
    return 0;
}

Qt3DRender::QAttribute::VertexBaseType accessorTypeFromJSON(int componentType)
{
    switch (componentType) {
    case GL_BYTE:
        return Qt3DRender::QAttribute::Byte;
    case GL_UNSIGNED_BYTE:
        return Qt3DRender::QAttribute::UnsignedByte;
    case GL_SHORT:
        return Qt3DRender::QAttribute::Short;
    case GL_UNSIGNED_SHORT:
        return Qt3DRender::QAttribute::UnsignedShort;
    case GL_UNSIGNED_INT:
        return Qt3DRender::QAttribute::UnsignedInt;
    case GL_FLOAT:
        return Qt3DRender::QAttribute::Float;
    default:
        qCWarning(kuesa) << "Unrecognized accessor component type";
        return Qt3DRender::QAttribute::Float;
    }
}

template<class IndexType>
bool copySparseValues(int count, const QByteArray &indicesBuffer, const QByteArray &valuesBuffer,
                      QByteArray &targetBuffer, int attributeOffset, int stride,
                      int valueSize)
{
    if (stride == 0)
        stride = valueSize;
    const IndexType *indices = reinterpret_cast<const IndexType *>(indicesBuffer.data());
    for (int i = 0; i < count; ++i) {
        const IndexType ndx = indices[i];
        char *target = targetBuffer.data() + stride * ndx + attributeOffset;
        const char *source = valuesBuffer.constData() + i * valueSize;
        memcpy(target, source, static_cast<size_t>(valueSize));
    }
    return true;
}

template<class IndexType>
bool copySparseValues(int bufferCount, int sparseCount, const QByteArray &indicesBuffer, const QByteArray &valuesBuffer,
                      QByteArray &targetBuffer, int attributeOffset, int stride,
                      Qt3DRender::QAttribute::VertexBaseType componentType, int numComponents)
{
    const int valueSize = accessorDataTypeToBytes(componentType) * numComponents;
    Q_ASSERT(valueSize != 0);
    if (targetBuffer.isEmpty())
        targetBuffer = QByteArray(bufferCount * valueSize, 0);

    return copySparseValues<IndexType>(sparseCount, indicesBuffer, valuesBuffer,
                                       targetBuffer, attributeOffset, stride,
                                       valueSize);
}

} // namespace

BufferAccessorParser::BufferAccessorParser()
{
}

BufferAccessorParser::~BufferAccessorParser()
{
}

bool Kuesa::GLTF2Import::BufferAccessorParser::parse(const QJsonArray &bufferAccessors, Kuesa::GLTF2Import::GLTF2Context *context)
{
    for (const QJsonValue &bufferAccessor : bufferAccessors) {
        const QJsonObject json = bufferAccessor.toObject();

        // Check it has the required properties
        if (!json.contains(KEY_COMPONENTTYPE) || !json.contains(KEY_COUNT) || !json.contains(KEY_TYPE))
            return false;

        Accessor accessor;
        accessor.bufferViewIndex = json.value(KEY_BUFFERVIEW).toInt(-1);
        if (accessor.bufferViewIndex >= 0)
            accessor.bufferData = context->bufferView(accessor.bufferViewIndex).bufferData;
        accessor.type = accessorTypeFromJSON(json.value(KEY_COMPONENTTYPE).toInt(GL_FLOAT));
        accessor.dataSize = accessorDataSizeFromJson(json.value(KEY_TYPE).toString());
        accessor.count = json.value(KEY_COUNT).toInt();
        accessor.offset = json.value(KEY_BYTEOFFSET).toInt(0);
        accessor.normalized = json.value(KEY_NORMALIZED).toBool(false);
        accessor.max = jsonArrayToVectorOfFloats(json.value(KEY_MAX).toArray());
        accessor.min = jsonArrayToVectorOfFloats(json.value(KEY_MIN).toArray());
        accessor.name = json.value(KEY_NAME).toString();

        if (json.contains(KEY_SPARSE)) {
            const QJsonObject sparse = json.value(KEY_SPARSE).toObject();
            if (!sparse.contains(KEY_COUNT) || !sparse.contains(KEY_SPARSE_INDICES) || !sparse.contains(KEY_SPARSE_VALUES)) {
                qCWarning(kuesa, "Missing sparse data in accessor");
                return false;
            }

            accessor.sparseCount = sparse.value(KEY_COUNT).toInt(0);

            const QJsonObject sparseIndices = sparse.value(KEY_SPARSE_INDICES).toObject();
            accessor.sparseIndices.type = accessorTypeFromJSON(sparseIndices.value(KEY_COMPONENTTYPE).toInt(GL_UNSIGNED_INT));
            accessor.sparseIndices.bufferViewIndex = sparseIndices.value(KEY_BUFFERVIEW).toInt(-1);
            accessor.sparseIndices.offset = sparseIndices.value(KEY_BYTEOFFSET).toInt(0);

            const QJsonObject sparseValues = sparse.value(KEY_SPARSE_VALUES).toObject();
            accessor.sparseValues.bufferViewIndex = sparseValues.value(KEY_BUFFERVIEW).toInt(-1);
            accessor.sparseValues.offset = sparseValues.value(KEY_BYTEOFFSET).toInt(0);

            if (accessor.sparseIndices.bufferViewIndex == -1 || accessor.sparseValues.bufferViewIndex == -1) {
                qCWarning(kuesa, "Missing buffers for sparse indices or values in accessor");
                return false;
            }

            const QByteArray sparseIndicesData = context->bufferView(accessor.sparseIndices.bufferViewIndex).bufferData;
            const QByteArray sparseValuesData = context->bufferView(accessor.sparseValues.bufferViewIndex).bufferData;
            if (sparseIndicesData.isEmpty() || sparseValuesData.isEmpty()) {
                qCWarning(kuesa, "Empty buffers for sparse indices or values in accessor");
                return false;
            }

            int stride = 0;
            if (!accessor.bufferData.isEmpty()) {
                accessor.bufferData.detach();
                const auto &bufferView = context->bufferView(accessor.bufferViewIndex);
                stride = bufferView.byteStride;
            }

            switch (accessor.sparseIndices.type) {
            case Qt3DRender::QAttribute::UnsignedByte:
                if (!copySparseValues<quint8>(accessor.count, accessor.sparseCount, sparseIndicesData, sparseValuesData,
                                              accessor.bufferData, accessor.offset, stride, accessor.type, accessor.dataSize)) {
                    qCWarning(kuesa, "Failed to parse sparse accessor data");
                    return false;
                }
                break;
            case Qt3DRender::QAttribute::Byte:
                if (!copySparseValues<qint8>(accessor.count, accessor.sparseCount, sparseIndicesData, sparseValuesData,
                                             accessor.bufferData, accessor.offset, stride, accessor.type, accessor.dataSize)) {
                    qCWarning(kuesa, "Failed to parse sparse accessor data");
                    return false;
                }
                break;
            case Qt3DRender::QAttribute::UnsignedShort:
                if (!copySparseValues<quint16>(accessor.count, accessor.sparseCount, sparseIndicesData, sparseValuesData,
                                               accessor.bufferData, accessor.offset, stride, accessor.type, accessor.dataSize)) {
                    qCWarning(kuesa, "Failed to parse sparse accessor data");
                    return false;
                }
                break;
            case Qt3DRender::QAttribute::Short:
                if (!copySparseValues<qint16>(accessor.count, accessor.sparseCount, sparseIndicesData, sparseValuesData,
                                              accessor.bufferData, accessor.offset, stride, accessor.type, accessor.dataSize)) {
                    qCWarning(kuesa, "Failed to parse sparse accessor data");
                    return false;
                }
                break;
            case Qt3DRender::QAttribute::UnsignedInt:
                if (!copySparseValues<uint>(accessor.count, accessor.sparseCount, sparseIndicesData, sparseValuesData,
                                            accessor.bufferData, accessor.offset, stride, accessor.type, accessor.dataSize)) {
                    qCWarning(kuesa, "Failed to parse sparse accessor data");
                    return false;
                }
                break;
            case Qt3DRender::QAttribute::Int:
                if (!copySparseValues<int>(accessor.count, accessor.sparseCount, sparseIndicesData, sparseValuesData,
                                           accessor.bufferData, accessor.offset, stride, accessor.type, accessor.dataSize)) {
                    qCWarning(kuesa, "Failed to parse sparse accessor data");
                    return false;
                }
                break;
            default:
                qCWarning(kuesa, "Unhandled sparse accessor index type");
                return false;
            }
        } else {
            if (!json.contains(KEY_BUFFERVIEW)
#if defined(KUESA_DRACO_COMPRESSION)
                && !context->requiredExtensions().contains(KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION)
#endif
            ) {
                qCWarning(kuesa, "Missing bufferView index in accessor");
                return false;
            }
        }

        context->addAccessor(accessor);
    }

    return true;
}

QT_END_NAMESPACE
