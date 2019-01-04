/*
    bufferaccessorparser.cpp

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

#include "bufferaccessorparser_p.h"

#include "kuesa_p.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

#include <QOpenGLContext>

#include "gltf2context_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

const QLatin1String KEY_BUFFERVIEW = QLatin1Literal("bufferView");
const QLatin1String KEY_COMPONENTTYPE = QLatin1Literal("componentType");
const QLatin1String KEY_COUNT = QLatin1Literal("count");
const QLatin1String KEY_TYPE = QLatin1Literal("type");
const QLatin1String KEY_NORMALIZED = QLatin1Literal("normalized");
const QLatin1String KEY_MAX = QLatin1Literal("max");
const QLatin1String KEY_MIN = QLatin1Literal("min");
const QLatin1String KEY_BYTEOFFSET = QLatin1Literal("byteOffset");
const QLatin1String KEY_NAME = QLatin1Literal("name");

QVector<float> jsonArrayToVectorOfFloats(const QJsonArray &values)
{
    const int nbComponents = values.size();
    QVector<float> d(nbComponents);
    std::transform(values.begin(), values.end(), d.begin(),
                   [](const QJsonValue &value) -> float {
                       return value.toDouble(0);
                   });
    return d;
}

quint32 accessorDataSizeFromJson(const QString &type)
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
} // namespace

BufferAccessorParser::BufferAccessorParser()
{
}

BufferAccessorParser::~BufferAccessorParser()
{
}

bool Kuesa::GLTF2Import::BufferAccessorParser::parse(const QJsonArray &bufferAccessors, Kuesa::GLTF2Import::GLTF2ContextPrivate *context)
{
    for (const QJsonValue &bufferAccessor : bufferAccessors) {
        QJsonObject json = bufferAccessor.toObject();

        // Check it has the required properties
        if (!json.contains(KEY_COMPONENTTYPE) || !json.contains(KEY_COUNT) || !json.contains(KEY_TYPE))
            return false;

        Accessor accessor;
        accessor.bufferViewIndex = json.value(KEY_BUFFERVIEW).toInt(0);
        accessor.type = accessorTypeFromJSON(json.value(KEY_COMPONENTTYPE).toInt(GL_FLOAT));
        accessor.dataSize = accessorDataSizeFromJson(json.value(KEY_TYPE).toString());
        accessor.count = json.value(KEY_COUNT).toInt();
        accessor.offset = json.value(KEY_BYTEOFFSET).toInt(0);
        accessor.normalized = json.value(KEY_NORMALIZED).toBool(false);
        accessor.max = jsonArrayToVectorOfFloats(json.value(KEY_MAX).toArray());
        accessor.min = jsonArrayToVectorOfFloats(json.value(KEY_MIN).toArray());
        accessor.name = json.value(KEY_NAME).toString();
        context->addAccessor(accessor);
    }

    return true;
}

QT_END_NAMESPACE
