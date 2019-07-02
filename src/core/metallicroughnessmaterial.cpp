/*
    metallicroughnessmaterial.cpp

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

#include "metallicroughnessmaterial.h"
#include "kuesa_p.h"

#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qparameter.h>

#include <type_traits>

#include "metallicroughnesseffect.h"
#include "morphcontroller.h"

#include "sceneentity.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

template<class OutputType>
using SignalType = void (MetallicRoughnessMaterial::*)(OutputType);

template<class OutputType>
using ValueTypeMapper = typename std::remove_const<typename std::remove_reference<OutputType>::type>::type;

template<class OutputType>
struct WrappedSignal {
    MetallicRoughnessMaterial *self;
    SignalType<OutputType> sig;
    void operator()(const QVariant &value) const
    {
        std::mem_fn(sig)(self, value.value<ValueTypeMapper<OutputType>>());
    }
};
template<class OutputType>
WrappedSignal<OutputType> wrapParameterSignal(MetallicRoughnessMaterial *self, SignalType<OutputType> sig)
{
    return WrappedSignal<OutputType>{ self, sig };
}

/*!
    \class Kuesa::MetallicRoughnessMaterial
    \inheaderfile Kuesa/MetallicRoughnessMaterial
    \inmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa::MetallicRoughnessMaterial is a ready to use physically based
    rendering (PBR) material based on the glTF 2.0 material description.

    The material is defined using a common set of parameter that define how
    light interacts with it. This allows to use a common description of the
    material so they are rendered consistently across platforms and 3D software
    and can be described by an intuitive set of parameters.

    The material consists on the following properties:
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

    Kuesa::MetallicRoughnessMaterial also supports alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.

    Kuesa::MetallicRoughnessMaterial supports two texture coordinate sets for
    each map. To specify the texture coordinate set a map must use, you can
    set the <map>UsesTexCoord1 property to true or false. If it's false, it
    will use the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials

    \note Kuesa::MetallicRoughnessMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \note The effect behind this material needs a brdfLut texture. If the brdfLut
    texture in the effect is not set and the material is added to a subtree of a
    Kuesa::SceneEntity, it will look for the Kuesa::SceneEntity and will use the
    brdfLut texture in the texture collection.
*/

/*!
    \property MetallicRoughnessMaterial::baseColorFactor

    Specifies the baseColorFactor of the material
*/

/*!
    \property MetallicRoughnessMaterial::baseColorMap

    Specifies a texture to be used as baseColorFactor. The content of the
    texture is expected to be in sRGB color space.

    \note If this property is nullptr and is set to a non nullptr value, will
    trigger a recompilation of the shader. If its a non nullptr value and is
    set to nullptr, it will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessMaterial::baseColorUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessMaterial::metallicFactor

    Specifies the metallic factor of the material. Set this value between 0.0
    and 1.0 to module how metallic the material is.
 */

/*!
    \property MetallicRoughnessMaterial::roughnessFactor

    Specifies the roughness factor of the material. Set the value between 0.0
    and 1.0 to modulate how rough the material is.
 */

/*!
    \property MetallicRoughnessMaterial::metalRoughMap

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
    \property MetallicRoughnessMaterial::metallicRoughnessUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessMaterial::normalMap

    Specifies a normal map for the material. This allows to simulate very
    detailed surfaces without a huge number of triangles. The content of the
    texture is expected to be in linear RGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessMaterial::normalUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessMaterial::normalScale

    Specifies a scale for the normal map calculations. A bigger number is used
    to simulated bigger bumps in the surface
 */

/*!
    \property MetallicRoughnessMaterial::ambientOcclusionMap

    Specifies a texture to be used for ambient occlusion. The content of the
    texture is expected to be a single R channel in linear space. This offers a
    way of combining the metalRoughMap's G and B channels in the same texture
    as the ambientOcclusionMap.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessMaterial::aoUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessMaterial::emissiveFactor

    Specifies an emissive factor to be used for emissive surfaces.
 */

/*!
    \property MetallicRoughnessMaterial::emissiveMap

    Specifies a texture to be used for emissive surfaces. The content of the
    texture is expected to be in sRGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessMaterial::emissiveUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property MetallicRoughnessMaterial::textureTransform

    Specifies the transform of the texture. This allows to scale, transform or
    rotate the textures of the material.
 */

/*!
    \property MetallicRoughnessMaterial::usingColorAttribute

    If true, base color calculations will take into account the vertex color
    property of the mesh

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessMaterial::doubleSided

    Allows to simulate double sided material. This are materials that should be
    rendered from the front and front the back. The normal for the back face
    will be the same as for the front face. This includes normal mapping
    calculations.

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessMaterial::useSkinning

    If the mesh that has this material applied must be animated using skinning
    techniques, this value must be true. This property will change the vertex
    shader of the material to use one with skinning capabilities.

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \property MetallicRoughnessMaterial::opaque

    If true, the material is opaque. If false, the material is transparent and
    will use alpha blending for transparency.

    \note This enable some extra render passes and will trigger a recompilation
    if changed from true to false or from false to true.
 */

/*!
    \property MetallicRoughnessMaterial::alphaCutoff

    Alpha cutoff threshold. Any fragment with an alpha value higher than this
    property will be considered opaque. Any fragment with an alpha value lower
    than this property will be discarded and wont have any effect on the final
    result. For alphaCutoff to have any effect, it must be activated.

    \sa alphaCutoffEnabled
 */

/*!
    \property MetallicRoughnessMaterial::toneMappingAlgoritm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \since Kuesa 1.1
 */

/*!
    \qmltype MetallicRoughnessMaterial
    \instantiates Kuesa::MetallicRoughnessMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa.MetallicRoughnessMaterial is a ready to use physically based
    rendering (PBR) material based on the glTF 2.0 material description.

    The material is defined using a common set of parameter that define how
    light interacts with it. This allows to use a common description of the
    material so they are rendered consistently across platforms and 3D software
    and can be described by an intuitive set of parameters.

    The material consists on the following properties:
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

    Kuesa.MetallicRoughnessMaterial also supports alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.

    Kuesa.MetallicRoughnessMaterial supports two texture coordinate sets for
    each map. To specify the texture coordinate set a map must use, you can
    set the <map>UsesTexCoord1 property to true or false. If it's false, it
    will use the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials

    \note Kuesa.MetallicRoughnessMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \note The effect behind this material needs a brdfLut texture. If the brdfLut
    texture in the effect is not set and the material is added to a subtree of a
    Kuesa.SceneEntity, it will look for the Kuesa.SceneEntity and will use the
    brdfLut texture in the texture collection.
 */

/*!
    \qmlproperty color MetallicRoughnessMaterial::baseColorFactor

    Specifies the baseColorFactor of the material
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessMaterial::baseColorMap

    Specifies a texture to be used as baseColorFactor. The content of the
    texture is expected to be in sRGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessMaterial::baseColorUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty float MetallicRoughnessMaterial::metallicFactor

    Specifies the metallic factor of the material. Set the value between 0.0
    and 1.0 to modulate how metallic the material is.
 */

/*!
    \qmlproperty float MetallicRoughnessMaterial::roughnessFactor

    Specifies the roughness factor of the material. Set the value between 0.0
    and 1.0 to modulate how rough the material is.
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessMaterial::metalRoughMap

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
    \qmlproperty bool MetallicRoughnessMaterial::metallicRoughnessUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessMaterial::normalMap

    Specifies a normal map for the material. This allows to simulate very
    detailed surfaces without a huge number of triangles. The content of the
    texture is expected to be in linear RGB color space.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessMaterial::normalUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty float MetallicRoughnessMaterial::normalScale

    Specifies a scale for the normal map calculations. A bigger number is used
    to simulated bigger bumps in the surface
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessMaterial::ambientOcclusionMap

    Specifies a texture to be used for ambient occlusion. The content of the
    texture is expected to be a single R channel in linear space. This offers a
    way of combining the metalRoughMap's G and B channels in the same texture
    as the ambientOcclusionMap.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessMaterial::aoUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty color MetallicRoughnessMaterial::emissiveFactor

    Specifies an emissive factor to be used for emissive surfaces.
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessMaterial::emissiveMap

    Specifies a texture to be used for emissive surfaces. The content of the
    texture is expected to be in sRGB color space.


    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool MetallicRoughnessMaterial::emissiveUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty MetallicRoughnessEffect.ToneMapping MetallicRoughnessMaterial::toneMappingAlgoritm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \since Kuesa 1.1
 */

MetallicRoughnessMaterial::MetallicRoughnessMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_baseColorUsesTexCoord1(new QParameter(QStringLiteral("baseColorUsesTexCoord1"), bool(false)))
    , m_metallicRoughnessUsesTexCoord1(new QParameter(QStringLiteral("metallicRoughnessUsesTexCoord1"), bool(false)))
    , m_normalUsesTexCoord1(new QParameter(QStringLiteral("normalUsesTexCoord1"), bool(false)))
    , m_aoUsesTexCoord1(new QParameter(QStringLiteral("aoUsesTexCoord1"), bool(false)))
    , m_emissiveUsesTexCoord1(new QParameter(QStringLiteral("emissiveUsesTexCoord1"), bool(false)))
    , m_baseColorFactorParameter(new QParameter(QStringLiteral("baseColorFactor"), QColor("gray")))
    , m_baseColorMapParameter(new QParameter(QStringLiteral("baseColorMap"), QVariant()))
    , m_metallicFactorParameter(new QParameter(QStringLiteral("metallicFactor"), 0.0f))
    , m_roughnessFactorParameter(new QParameter(QStringLiteral("roughnessFactor"), 0.0f))
    , m_metalRoughMapParameter(new QParameter(QStringLiteral("metalRoughMap"), QVariant()))
    , m_normalScaleParameter(new QParameter(QStringLiteral("normalScale"), 0.25f))
    , m_normalMapParameter(new QParameter(QStringLiteral("normalMap"), QVariant()))
    , m_ambientOcclusionMapParameter(new QParameter(QStringLiteral("ambientOcclusionMap"), QVariant()))
    , m_emissiveFactorParameter(new QParameter(QStringLiteral("emissiveFactor"), QColor("black")))
    , m_emissiveMapParameter(new QParameter(QStringLiteral("emissiveMap"), QVariant()))
    , m_effect(new MetallicRoughnessEffect(this))
{
    QObject::connect(m_baseColorUsesTexCoord1, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::baseColorUsesTexCoord1Changed));
    QObject::connect(m_metallicRoughnessUsesTexCoord1, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::metallicRoughnessUsesTexCoord1Changed));
    QObject::connect(m_normalUsesTexCoord1, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::normalUsesTexCoord1Changed));
    QObject::connect(m_aoUsesTexCoord1, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::aoUsesTexCoord1Changed));
    QObject::connect(m_emissiveUsesTexCoord1, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::emissiveUsesTexCoord1Changed));
    QObject::connect(m_baseColorFactorParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::baseColorFactorChanged));
    QObject::connect(m_baseColorMapParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::baseColorMapChanged));
    QObject::connect(m_metallicFactorParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::metallicFactorChanged));
    QObject::connect(m_roughnessFactorParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::roughnessFactorChanged));
    QObject::connect(m_metalRoughMapParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::metalRoughMapChanged));
    QObject::connect(m_normalScaleParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::normalScaleChanged));
    QObject::connect(m_normalMapParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::normalMapChanged));
    QObject::connect(m_ambientOcclusionMapParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::ambientOcclusionMapChanged));
    QObject::connect(m_emissiveFactorParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::emissiveFactorChanged));
    QObject::connect(m_emissiveMapParameter, &QParameter::valueChanged,
                     this, wrapParameterSignal(this, &MetallicRoughnessMaterial::emissiveMapChanged));

    QObject::connect(this, &MetallicRoughnessMaterial::usingColorAttributeChanged,
                     this, &MetallicRoughnessMaterial::updateEffect);
    QObject::connect(this, &MetallicRoughnessMaterial::doubleSidedChanged,
                     this, &MetallicRoughnessMaterial::updateEffect);
    QObject::connect(this, &MetallicRoughnessMaterial::useSkinningChanged,
                     this, &MetallicRoughnessMaterial::updateEffect);
    QObject::connect(this, &MetallicRoughnessMaterial::opaqueChanged,
                     this, &MetallicRoughnessMaterial::updateEffect);
    QObject::connect(this, &MetallicRoughnessMaterial::alphaCutoffEnabledChanged,
                     this, &MetallicRoughnessMaterial::updateEffect);

    QObject::connect(m_effect, &MetallicRoughnessEffect::toneMappingAlgorithmChanged,
                     this, &MetallicRoughnessMaterial::toneMappingAlgorithmChanged);

    addParameter(m_baseColorUsesTexCoord1);
    addParameter(m_metallicRoughnessUsesTexCoord1);
    addParameter(m_normalUsesTexCoord1);
    addParameter(m_aoUsesTexCoord1);
    addParameter(m_emissiveUsesTexCoord1);
    addParameter(m_baseColorFactorParameter);
    addParameter(m_metallicFactorParameter);
    addParameter(m_roughnessFactorParameter);
    addParameter(m_normalScaleParameter);
    addParameter(m_emissiveFactorParameter);

    setEffect(m_effect);
    updateEffect();

    QObject::connect(this, &Kuesa::MetallicRoughnessMaterial::addedToEntity,
                     this, &Kuesa::MetallicRoughnessMaterial::onAddedToEntity);
}

MetallicRoughnessMaterial::~MetallicRoughnessMaterial()
{
}

bool MetallicRoughnessMaterial::isBaseColorUsingTexCoord1() const
{
    return m_baseColorUsesTexCoord1->value().value<bool>();
}

bool MetallicRoughnessMaterial::isMetallicRoughnessUsingTexCoord1() const
{
    return m_metallicRoughnessUsesTexCoord1->value().value<bool>();
}

bool MetallicRoughnessMaterial::isNormalUsingTexCoord1() const
{
    return m_normalUsesTexCoord1->value().value<bool>();
}

bool MetallicRoughnessMaterial::isAOUsingTexCoord1() const
{
    return m_aoUsesTexCoord1->value().value<bool>();
}

bool MetallicRoughnessMaterial::isEmissiveUsingTexCoord1() const
{
    return m_emissiveUsesTexCoord1->value().value<bool>();
}

QColor MetallicRoughnessMaterial::baseColorFactor() const
{
    return m_baseColorFactorParameter->value().value<QColor>();
}

QAbstractTexture *MetallicRoughnessMaterial::baseColorMap() const
{
    return m_baseColorMapParameter->value().value<QAbstractTexture *>();
}

float MetallicRoughnessMaterial::metallicFactor() const
{
    return m_metallicFactorParameter->value().toFloat();
}

float MetallicRoughnessMaterial::roughnessFactor() const
{
    return m_roughnessFactorParameter->value().toFloat();
}

QAbstractTexture *MetallicRoughnessMaterial::metalRoughMap() const
{
    return m_metalRoughMapParameter->value().value<QAbstractTexture *>();
}

float MetallicRoughnessMaterial::normalScale() const
{
    return m_normalScaleParameter->value().toFloat();
}

QAbstractTexture *MetallicRoughnessMaterial::normalMap() const
{
    return m_normalMapParameter->value().value<QAbstractTexture *>();
}

QAbstractTexture *MetallicRoughnessMaterial::ambientOcclusionMap() const
{
    return m_ambientOcclusionMapParameter->value().value<QAbstractTexture *>();
}

QColor MetallicRoughnessMaterial::emissiveFactor() const
{
    return m_emissiveFactorParameter->value().value<QColor>();
}

QAbstractTexture *MetallicRoughnessMaterial::emissiveMap() const
{
    return m_emissiveMapParameter->value().value<QAbstractTexture *>();
}

MetallicRoughnessEffect::ToneMapping MetallicRoughnessMaterial::toneMappingAlgorithm() const
{
    return m_effect->toneMappingAlgorithm();
}

void MetallicRoughnessMaterial::setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1)
{
    m_baseColorUsesTexCoord1->setValue(baseColorUsesTexCoord1);
}

void MetallicRoughnessMaterial::setMetallicRoughnessUsesTexCoord1(bool metallicRoughnessUsesTexCoord1)
{
    m_metallicRoughnessUsesTexCoord1->setValue(metallicRoughnessUsesTexCoord1);
}

void MetallicRoughnessMaterial::setNormalUsesTexCoord1(bool normalUsesTexCoord1)
{
    m_normalUsesTexCoord1->setValue(normalUsesTexCoord1);
}

void MetallicRoughnessMaterial::setAOUsesTexCoord1(bool aoUsesTexCoord1)
{
    m_aoUsesTexCoord1->setValue(aoUsesTexCoord1);
}

void MetallicRoughnessMaterial::setEmissiveUsesTexCoord1(bool emissiveUsesTexCoord1)
{
    m_emissiveUsesTexCoord1->setValue(emissiveUsesTexCoord1);
}

void MetallicRoughnessMaterial::setBaseColorFactor(const QColor &baseColorFactor)
{
    m_baseColorFactorParameter->setValue(baseColorFactor);
}

void MetallicRoughnessMaterial::setBaseColorMap(QAbstractTexture *baseColorMap)
{
    if (m_baseColorMapParameter->value().value<QAbstractTexture *>() == baseColorMap)
        return;
#ifndef QT_OPENGL_ES_2
    baseColorMap->setFormat(QAbstractTexture::TextureFormat::SRGB8_Alpha8);
#endif
    m_effect->setBaseColorMapEnabled(baseColorMap);
    m_baseColorMapParameter->setValue(QVariant::fromValue(baseColorMap));
    if (baseColorMap) {
        addParameter(m_baseColorMapParameter);
    } else {
        removeParameter(m_baseColorMapParameter);
    }
    updateEffect();
}

void MetallicRoughnessMaterial::setMetallicFactor(float metallicFactor)
{
    m_metallicFactorParameter->setValue(metallicFactor);
}

void MetallicRoughnessMaterial::setRoughnessFactor(float roughnessFactor)
{
    m_roughnessFactorParameter->setValue(roughnessFactor);
}

void MetallicRoughnessMaterial::setMetalRoughMap(QAbstractTexture *metalRoughMap)
{
    if (m_metalRoughMapParameter->value().value<QAbstractTexture *>() == metalRoughMap)
        return;

    m_effect->setMetalRoughMapEnabled(metalRoughMap);
    m_metalRoughMapParameter->setValue(QVariant::fromValue(metalRoughMap));
    if (metalRoughMap) {
        addParameter(m_metalRoughMapParameter);
    } else {
        removeParameter(m_metalRoughMapParameter);
    }
    updateEffect();
}

void MetallicRoughnessMaterial::setNormalMap(QAbstractTexture *normalMap)
{
    if (m_normalMapParameter->value().value<QAbstractTexture *>() == normalMap)
        return;

    m_effect->setNormalMapEnabled(normalMap);
    m_normalMapParameter->setValue(QVariant::fromValue(normalMap));
    if (normalMap) {
        addParameter(m_normalMapParameter);
    } else {
        removeParameter(m_normalMapParameter);
    }
    updateEffect();
}

void MetallicRoughnessMaterial::setNormalScale(float normalScale)
{
    m_normalScaleParameter->setValue(normalScale);
}

void MetallicRoughnessMaterial::setAmbientOcclusionMap(QAbstractTexture *ambientOcclusionMap)
{
    if (m_ambientOcclusionMapParameter->value().value<QAbstractTexture *>() == ambientOcclusionMap)
        return;

    m_effect->setAmbientOcclusionMapEnabled(ambientOcclusionMap);
    m_ambientOcclusionMapParameter->setValue(QVariant::fromValue(ambientOcclusionMap));
    if (ambientOcclusionMap) {
        addParameter(m_ambientOcclusionMapParameter);
    } else {
        removeParameter(m_ambientOcclusionMapParameter);
    }
    updateEffect();
}

void MetallicRoughnessMaterial::setEmissiveFactor(const QColor &emissiveFactor)
{
    m_emissiveFactorParameter->setValue(emissiveFactor);
}

void MetallicRoughnessMaterial::setEmissiveMap(QAbstractTexture *emissiveMap)
{
    if (m_emissiveMapParameter->value().value<QAbstractTexture *>() == emissiveMap)
        return;
#ifndef QT_OPENGL_ES_2
    emissiveMap->setFormat(QAbstractTexture::TextureFormat::SRGB8_Alpha8);
#endif
    m_effect->setEmissiveMapEnabled(emissiveMap);
    m_emissiveMapParameter->setValue(QVariant::fromValue(emissiveMap));
    if (emissiveMap) {
        addParameter(m_emissiveMapParameter);
    } else {
        removeParameter(m_emissiveMapParameter);
    }
    updateEffect();
}

void MetallicRoughnessMaterial::setToneMappingAlgorithm(MetallicRoughnessEffect::ToneMapping algorithm)
{
    m_effect->setToneMappingAlgorithm(algorithm);
}

void MetallicRoughnessMaterial::onAddedToEntity(Qt3DCore::QEntity *entity)
{
    if (m_effect->brdfLUT() != nullptr)
        return;

    // Looks on the entity tree until we find a SceneEntity
    // while we find the SceneEntity, get the brdfLut texture and add it to the effect
    auto *sceneEntity = Kuesa::SceneEntity::findParentSceneEntity(entity);
    if (!sceneEntity) {
        qCWarning(kuesa) << "A material has been added to a subtree without a SceneEntity";
        return;
    }

    m_effect->setBrdfLUT(sceneEntity->brdfLut());
}

void MetallicRoughnessMaterial::updateEffect()
{
    m_effect->setBaseColorMapEnabled(baseColorMap() != nullptr);
    m_effect->setMetalRoughMapEnabled(metalRoughMap() != nullptr);
    m_effect->setNormalMapEnabled(normalMap() != nullptr);
    m_effect->setAmbientOcclusionMapEnabled(ambientOcclusionMap() != nullptr);
    m_effect->setEmissiveMapEnabled(emissiveMap() != nullptr);
    m_effect->setUsingColorAttribute(isUsingColorAttribute());
    m_effect->setDoubleSided(isDoubleSided());
    m_effect->setOpaque(isOpaque());
    m_effect->setAlphaCutoffEnabled(isAlphaCutoffEnabled());
    m_effect->setUseSkinning(m_usesSkinning);
}

} // namespace Kuesa

QT_END_NAMESPACE
