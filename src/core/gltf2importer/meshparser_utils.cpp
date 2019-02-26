/*
    meshparser_utils.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometryRenderer>

#include <array>

#include "mikktspace.h"

namespace {

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

    return { Qt3DRender::QAttribute::UnsignedByte,
             Qt3DRender::QAttribute::UnsignedShort,
             Qt3DRender::QAttribute::UnsignedInt };
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

    return { 1 };
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
} // namespace

// TODO This is only needed when the material has normal mapping
// TODO Compute tangents when the primitive is not indexed
namespace Kuesa {
namespace GLTF2Import {
namespace MeshParserUtils {

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
} // namespace MeshParserUtils
} // namespace GLTF2Import
} // namespace Kuesa
