/*
    unlitproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "unlitproperties.h"
#include "unlitshaderdata_p.h"

namespace Kuesa {

/*!
    \class Kuesa::UnlitProperties
    \inheaderfile Kuesa/UnlitProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa::UnlitProperties hold the properties of a simple material
    without shading. It implements the KHR_materials_unlit extension from glTF
    2.0.

    The UnlitProperties consists of the following properties:
    \list
    \li baseColorFactor: Base color of the#include <QtGui/qgenericmatrix.h>
 material
    \li baseColorMap: A texture specifying the base color of the material
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \endlist

    For a particular point of an object, the base color is computed as
    baseColorFactor*baseColorMap*vertexColor. The vertexColor is the
    interpolated value of the vertex color property of the mesh to which the
    material is applied.

    Kuesa::UnlitProperties also supports alpha blending and alpha cutoff. Alpha
    blending is used to simulate transparent materials, like a glass, while
    alpha cutoff is used to discard fragment that have an alpha value below a
    threshold. This is useful to render grass or trees for example.

    Kuesa::UnlitProperties supports two texture coordinate sets for each map.
    To specify the texture coordinate set a map must use, you can set the
    <map>UsesTexCoord1 property to true or false. If it's false, it will use
    the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_unlit
*/

/*!
    \qmltype UnlitProperties
    \instantiates Kuesa::UnlitProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa.UnlitProperties hold the properties of a simple material
    without shading. It implements the KHR_materials_unlit extension from glTF
    2.0.

    The UnlitProperties consists of the following properties:
    \list
    \li baseColorFactor: Base color of the#include <QtGui/qgenericmatrix.h>
 material
    \li baseColorMap: A texture specifying the base color of the material
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \endlist

    For a particular point of an object, the base color is computed as
    baseColorFactor*baseColorMap*vertexColor. The vertexColor is the
    interpolated value of the vertex color property of the mesh to which the
    material is applied.

    Kuesa.UnlitProperties also supports alpha blending and alpha cutoff. Alpha
    blending is used to simulate transparent materials, like a glass, while
    alpha cutoff is used to discard fragment that have an alpha value below a
    threshold. This is useful to render grass or trees for example.

    Kuesa.UnlitProperties supports two texture coordinate sets for each map. To
    specify the texture coordinate set a map must use, you can set the
    <map>UsesTexCoord1 property to true or false. If it's false, it will use
    the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_unlit
 */

UnlitProperties::UnlitProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_unlitShaderData(new UnlitShaderData(this))
{
    QObject::connect(this, &GLTF2MaterialProperties::baseColorMapChanged,
                     m_unlitShaderData, &UnlitShaderData::setBaseColorMap);
    QObject::connect(this, &GLTF2MaterialProperties::baseColorFactorChanged,
                     m_unlitShaderData, &UnlitShaderData::setBaseColorFactor);
    QObject::connect(this, &GLTF2MaterialProperties::baseColorUsesTexCoord1Changed,
                     m_unlitShaderData, &UnlitShaderData::setBaseColorUsesTexCoord1);
    QObject::connect(this, &GLTF2MaterialProperties::alphaCutoffChanged,
                     m_unlitShaderData, &UnlitShaderData::setAlphaCutoff);
}

UnlitProperties::~UnlitProperties() = default;

Qt3DRender::QShaderData *UnlitProperties::shaderData() const
{
    return m_unlitShaderData;
}

} // namespace Kuesa
