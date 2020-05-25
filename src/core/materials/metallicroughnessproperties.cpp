/*
    metallicroughnessproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "metallicroughnessproperties.h"
#include "metallicroughnessshaderdata_p.h"

namespace Kuesa {

/*!
    \class Kuesa::MetallicRoughnessProperties
    \inheaderfile Kuesa/MetallicRoughnessProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa::MetallicRoughnessProperties hold the properties of a
    physically based rendering (PBR) material based on the glTF 2.0 material
    description.

    The material properties are defined using a common set of parameter that
    define how light interacts with it. This allows to use a common description
    of the material so they are rendered consistently across platforms and 3D
    software and can be described by an intuitive set of parameters.

    The MetallicRoughnessProperties consists of the following properties:
    \list
    \li baseColorFactor: Base color of the material
    \li baseColorMap: A texture specifying the base color of the material
    \li metallicFactor: The metalness of the material
    \li roughnessFactor: The roughness of the material
    \li metalRoughMap: A texture specifying both metalness and roughness of the
    material
    \li normalMap: Normal texture for normal mapping. This allows to simulate
    very detailed surfaces without too many triangle
    \li normalScale: A scale factor for the normal map
    \li ambientOcclusionMap: An ambient occlusion map. This allows to simulate
    ambient occlusion and shadow zones that otherwise will be too bright
    \li emissiveFactor: The emissive strength of the material
    \li emissiveMap: A texture specifying the emissive property of the material
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \endlist

    For a particular point of an object, the base color is computed as
    baseColorFactor*baseColorMap*vertexColor. The vertexColor is the
    interpolated value of the vertex color property of the mesh to which the
    material is applied.

    Kuesa::MetallicRoughnessProperties also supports alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.

    Kuesa::MetallicRoughnessProperties supports two texture coordinate sets for
    each map. To specify the texture coordinate set a map must use, you can set
    the <map>UsesTexCoord1 property to true or false. If it's false, it will
    use the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials
*/

/*!
    \property MetallicRoughnessProperties::metallicFactor

    Specifies the metallic factor of the material. Set this value between 0.0
    and 1.0 to module how metallic the material is.
 */

/*!
    \property MetallicRoughnessProperties::roughnessFactor

    Specifies the roughness factor of the material. Set the value between 0.0
    and 1.0 to modulate how rough the material is.
 */

/*!
    \property MetallicRoughnessProperties::metalRoughMap

    Specifies a texture to be used as metallic and roughness factor. The
    metallic factor is sampled for the B channel of the texture, while the
    roughness factor is sampled from the G channel. R and A channel are unused.
    This offers a way of combining the ambientOcclusionMap's R channel in the
    same texture as the metalRoughMap. The content of the texture is expected
    to be in linear RGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.

 */

/*!
    \property MetallicRoughnessProperties::metallicRoughnessUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessProperties::normalMap

    Specifies a normal map for the material. This allows to simulate very
    detailed surfaces without a huge number of triangles. The content of the
    texture is expected to be in linear RGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessProperties::normalUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessProperties::normalScale

    Specifies a scale for the normal map calculations. A bigger number is used
    to simulated bigger bumps in the surface
 */

/*!
    \property MetallicRoughnessProperties::ambientOcclusionMap

    Specifies a texture to be used for ambient occlusion. The content of the
    texture is expected to be a single R channel in linear space. This offers a
    way of combining the metalRoughMap's G and B channels in the same texture
    as the ambientOcclusionMap.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessProperties::aoUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessProperties::emissiveFactor

    Specifies an emissive factor to be used for emissive surfaces.
 */

/*!
    \property MetallicRoughnessProperties::emissiveMap

    Specifies a texture to be used for emissive surfaces. The content of the
    texture is expected to be in sRGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessProperties::emissiveUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmltype MetallicRoughnessProperties
    \instantiates Kuesa::MetallicRoughnessProperties
    \inqmlmodule Kuesa
    \inherits Kuesa::GLTF2MaterialProperties
    \since Kuesa 1.1

    \brief Kuesa::MetallicRoughnessProperties hold the properties of a
    physically based rendering (PBR) material based on the glTF 2.0 material
    description.

    The material properties are defined using a common set of parameter that
    define how light interacts with it. This allows to use a common description
    of the material so they are rendered consistently across platforms and 3D
    software and can be described by an intuitive set of parameters.

    The MetallicRoughnessProperties consists of the following properties:
    \list
    \li baseColorFactor: Base color of the material
    \li baseColorMap: A texture specifying the base color of the material
    \li metallicFactor: The metalness of the material
    \li roughnessFactor: The roughness of the material
    \li metalRoughMap: A texture specifying both metalness and roughness of the
    material
    \li normalMap: Normal texture for normal mapping. This allows to simulate
    very detailed surfaces without too many triangle
    \li normalScale: A scale factor for the normal map
    \li ambientOcclusionMap: An ambient occlusion map. This allows to simulate
    ambient occlusion and shadow zones that otherwise will be too bright
    \li emissiveFactor: The emissive strength of the material
    \li emissiveMap: A texture specifying the emissive property of the material
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \endlist

    For a particular point of an object, the base color is computed as
    baseColorFactor*baseColorMap*vertexColor. The vertexColor is the
    interpolated value of the vertex color property of the mesh to which the
    material is applied.

    Kuesa::MetallicRoughnessProperties also supports alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.

    Kuesa::MetallicRoughnessProperties supports two texture coordinate sets for
    each map. To specify the texture coordinate set a map must use, you can set
    the <map>UsesTexCoord1 property to true or false. If it's false, it will
    use the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials
 */

/*!
    \qmlproperty float MetallicRoughnessProperties::metallicFactor

    Specifies the metallic factor of the material. Set the value between 0.0
    and 1.0 to modulate how metallic the material is.
 */

/*!
    \qmlproperty float MetallicRoughnessProperties::roughnessFactor

    Specifies the roughness factor of the material. Set the value between 0.0
    and 1.0 to modulate how rough the material is.
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessProperties::metalRoughMap

    Specifies a texture to be used as metallic and roughness factor. The
    metallic factor is sampled for the B channel of the texture, while the
    roughness factor is sampled from the G channel. R and A channel are unused.
    This offers a way of combining the ambientOcclusionMap's R channel in the
    same texture as the metalRoughMap. The content of the texture is expected
    to be in linear RGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessProperties::metallicRoughnessUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessProperties::normalMap

    Specifies a normal map for the material. This allows to simulate very
    detailed surfaces without a huge number of triangles. The content of the
    texture is expected to be in linear RGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessProperties::normalUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty float MetallicRoughnessProperties::normalScale

    Specifies a scale for the normal map calculations. A bigger number is used
    to simulated bigger bumps in the surface
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessProperties::ambientOcclusionMap

    Specifies a texture to be used for ambient occlusion. The content of the
    texture is expected to be a single R channel in linear space. This offers a
    way of combining the metalRoughMap's G and B channels in the same texture
    as the ambientOcclusionMap.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessProperties::aoUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty color MetallicRoughnessProperties::emissiveFactor

    Specifies an emissive factor to be used for emissive surfaces.
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessProperties::emissiveMap

    Specifies a texture to be used for emissive surfaces. The content of the
    texture is expected to be in sRGB color space.


    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessProperties::emissiveUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

MetallicRoughnessProperties::MetallicRoughnessProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_metallicRoughnessShaderData(new MetallicRoughnessShaderData(this))

{
    QObject::connect(this, &GLTF2MaterialProperties::baseColorMapChanged,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setBaseColorMap);
    QObject::connect(this, &GLTF2MaterialProperties::baseColorFactorChanged,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setBaseColorFactor);
    QObject::connect(this, &GLTF2MaterialProperties::baseColorUsesTexCoord1Changed,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setBaseColorUsesTexCoord1);
    QObject::connect(this, &GLTF2MaterialProperties::alphaCutoffChanged,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setAlphaCutoff);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::metallicRoughnessUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::metallicRoughnessUsesTexCoord1Changed);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::normalUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::normalUsesTexCoord1Changed);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::aoUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::aoUsesTexCoord1Changed);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::emissiveUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::emissiveUsesTexCoord1Changed);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::metallicFactorChanged,
                     this, &MetallicRoughnessProperties::metallicFactorChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::roughnessFactorChanged,
                     this, &MetallicRoughnessProperties::roughnessFactorChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::metalRoughMapChanged,
                     this, &MetallicRoughnessProperties::metalRoughMapChanged);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::normalScaleChanged,
                     this, &MetallicRoughnessProperties::normalScaleChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::normalMapChanged,
                     this, &MetallicRoughnessProperties::normalMapChanged);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::ambientOcclusionMapChanged,
                     this, &MetallicRoughnessProperties::ambientOcclusionMapChanged);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::emissiveFactorChanged,
                     this, &MetallicRoughnessProperties::emissiveFactorChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::emissiveMapChanged,
                     this, &MetallicRoughnessProperties::emissiveMapChanged);
}

MetallicRoughnessProperties::~MetallicRoughnessProperties() = default;

Qt3DRender::QShaderData *MetallicRoughnessProperties::shaderData() const
{
    return m_metallicRoughnessShaderData;
}

bool MetallicRoughnessProperties::isMetallicRoughnessUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isMetallicRoughnessUsingTexCoord1();
}

bool MetallicRoughnessProperties::isNormalUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isNormalUsingTexCoord1();
}

bool MetallicRoughnessProperties::isAOUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isAOUsingTexCoord1();
}

bool MetallicRoughnessProperties::isEmissiveUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isEmissiveUsingTexCoord1();
}

float MetallicRoughnessProperties::metallicFactor() const
{
    return m_metallicRoughnessShaderData->metallicFactor();
}

float MetallicRoughnessProperties::roughnessFactor() const
{
    return m_metallicRoughnessShaderData->roughnessFactor();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::metalRoughMap() const
{
    return m_metallicRoughnessShaderData->metalRoughMap();
}

float MetallicRoughnessProperties::normalScale() const
{
    return m_metallicRoughnessShaderData->normalScale();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::normalMap() const
{
    return m_metallicRoughnessShaderData->normalMap();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::ambientOcclusionMap() const
{
    return m_metallicRoughnessShaderData->ambientOcclusionMap();
}

QColor MetallicRoughnessProperties::emissiveFactor() const
{
    return m_metallicRoughnessShaderData->emissiveFactor();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::emissiveMap() const
{
    return m_metallicRoughnessShaderData->emissiveMap();
}
void MetallicRoughnessProperties::setMetallicRoughnessUsesTexCoord1(bool metallicRoughnessUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setMetallicRoughnessUsesTexCoord1(metallicRoughnessUsesTexCoord1);
}

void MetallicRoughnessProperties::setNormalUsesTexCoord1(bool normalUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setNormalUsesTexCoord1(normalUsesTexCoord1);
}

void MetallicRoughnessProperties::setAOUsesTexCoord1(bool aoUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setAOUsesTexCoord1(aoUsesTexCoord1);
}

void MetallicRoughnessProperties::setEmissiveUsesTexCoord1(bool emissiveUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setEmissiveUsesTexCoord1(emissiveUsesTexCoord1);
}

void MetallicRoughnessProperties::setMetallicFactor(float metallicFactor)
{
    m_metallicRoughnessShaderData->setMetallicFactor(metallicFactor);
}

void MetallicRoughnessProperties::setRoughnessFactor(float roughnessFactor)
{
    m_metallicRoughnessShaderData->setRoughnessFactor(roughnessFactor);
}

void MetallicRoughnessProperties::setMetalRoughMap(Qt3DRender::QAbstractTexture *metalRoughMap)
{
    m_metallicRoughnessShaderData->setMetalRoughMap(metalRoughMap);
}

void MetallicRoughnessProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
{
    m_metallicRoughnessShaderData->setNormalMap(normalMap);
}

void MetallicRoughnessProperties::setNormalScale(float normalScale)
{
    m_metallicRoughnessShaderData->setNormalScale(normalScale);
}

void MetallicRoughnessProperties::setAmbientOcclusionMap(Qt3DRender::QAbstractTexture *ambientOcclusionMap)
{
    m_metallicRoughnessShaderData->setAmbientOcclusionMap(ambientOcclusionMap);
}

void MetallicRoughnessProperties::setEmissiveFactor(const QColor &emissiveFactor)
{
    m_metallicRoughnessShaderData->setEmissiveFactor(emissiveFactor);
}

void MetallicRoughnessProperties::setEmissiveMap(Qt3DRender::QAbstractTexture *emissiveMap)
{
    m_metallicRoughnessShaderData->setEmissiveMap(emissiveMap);
}

} // namespace Kuesa
