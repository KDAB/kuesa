/*
    unlitmaterial.cpp

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

#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qabstracttexture.h>
#include "unlitmaterial.h"
#include "unliteffect.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::UnlitMaterial
    \inheaderfile Kuesa/UnlitMaterial
    \inmodule Kuesa
    \since 1.1

    \brief Kuesa::UnlitMaterial is a simple material without shading. It
    implements the KHR_materials_unlit extension from glTF 2.0.

    The material consists on the following properties:
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

    Kuesa::UnlitMaterial also supports alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.

    Kuesa::UnlitMaterial supports two texture coordinate sets for
    each map. To specify the texture coordinate set a map must use, you can
    set the <map>UsesTexCoord1 property to true or false. If it's false, it
    will use the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_unlit

    \note Kuesa::UnlitMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.
*/

/*!
    \property UnlitMaterial::baseColorFactor

    Specifies the baseColorFactor of the material
*/

/*!
    \property UnlitMaterial::baseColorMap

    Specifies a texture to be used as baseColorFactor.

    \note If this property is nullptr and is set to a non nullptr value, will
    trigger a recompilation of the shader. If its a non nullptr value and is
    set to nullptr, it will trigger a recompilation of the shader.
 */

/*!
    \property UnlitMaterial::baseColorUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmltype UnlitMaterial
    \instantiates Kuesa::UnlitMaterial
    \inqmlmodule Kuesa
    \since 1.1

    \brief Kuesa.UnlitMaterial is a simple material without shading. It
    implements the KHR_materials_unlit extension from glTF 2.0.

    The material consists on the following properties:
    \list
    \li baseColorFactor: Base color of the material
    \li baseColorMap: A texture specifying the base color of the material
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \endlist

    For a particular point of an object, the base color is computed as
    baseColorFactor*baseColorMap*vertexColor. The vertexColor is the
    interpolated value of the vertex color property of the mesh to which the
    material is applied.

    Kuesa::UnlitMaterial also supports alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.

    Kuesa.UnlitMaterial supports two texture coordinate sets for
    each map. To specify the texture coordinate set a map must use, you can
    set the <map>UsesTexCoord1 property to true or false. If it's false, it
    will use the 0th coordinate set of the primitive, attached to
    'defaultTextureCoordinateAttributeName` attribute. If it's true, it will
    use the 1st coordinate set of the primitive, attached to
    'defaultTextureCoordinate1AttributeName`.

    A more complete description of the material can be found in
    https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_unlit

    \note Kuesa.UnlitMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.
 */

/*!
    \qmlproperty color UnlitMaterial::baseColorFactor

    Specifies the baseColorFactor of the material
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture UnlitMaterial::baseColorMap

    Specifies a texture to be used as baseColorFactor.

    \note If this property is nullptr and is set to a non nullptr value or
    viceverse, it will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool UnlitMaterial::baseColorUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \qmlproperty bool UnlitMaterial::alphaCutoffEnabled

    Enables/disables alphaCutoff

    \note This enable some extra render passes and will trigger a recompilation
    if changed from true to false or from false to true.
 */

namespace {

template<class OutputType>
using SignalType = void (UnlitMaterial::*)(OutputType);

template<class OutputType>
using ValueTypeMapper = typename std::remove_const<typename std::remove_reference<OutputType>::type>::type;

template<class OutputType>
struct WrappedSignal {
    UnlitMaterial *self;
    SignalType<OutputType> sig;
    void operator()(const QVariant &value) const
    {
        std::mem_fn(sig)(self, value.value<ValueTypeMapper<OutputType>>());
    }
};
template<class OutputType>
WrappedSignal<OutputType> wrapParameterSignal(UnlitMaterial *self, SignalType<OutputType> sig)
{
    return WrappedSignal<OutputType>{ self, sig };
}

} // anonymous

UnlitMaterial::UnlitMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_effect(new UnlitEffect(this))
    , m_baseColorFactorParameter(new Qt3DRender::QParameter(QStringLiteral("baseColorFactor"), QColor("gray")))
    , m_baseColorMapParameter(new Qt3DRender::QParameter(QStringLiteral("baseColorMap"), QVariant()))
    , m_baseColorUsesTexCoord1(new Qt3DRender::QParameter(QStringLiteral("baseColorUsesTexCoord1"), bool(false)))
{
    QObject::connect(m_baseColorUsesTexCoord1, &Qt3DRender::QParameter::valueChanged,
                     this, wrapParameterSignal(this, &UnlitMaterial::baseColorUsesTexCoord1Changed));
    QObject::connect(m_baseColorFactorParameter, &Qt3DRender::QParameter::valueChanged,
                     this, wrapParameterSignal(this, &UnlitMaterial::baseColorFactorChanged));
    QObject::connect(m_baseColorMapParameter, &Qt3DRender::QParameter::valueChanged,
                     this, wrapParameterSignal(this, &UnlitMaterial::baseColorMapChanged));

    QObject::connect(this, &UnlitMaterial::usingColorAttributeChanged,
                     this, &UnlitMaterial::updateEffect);
    QObject::connect(this, &UnlitMaterial::doubleSidedChanged,
                     this, &UnlitMaterial::updateEffect);
    QObject::connect(this, &UnlitMaterial::useSkinningChanged,
                     this, &UnlitMaterial::updateEffect);
    QObject::connect(this, &UnlitMaterial::opaqueChanged,
                     this, &UnlitMaterial::updateEffect);
    QObject::connect(this, &UnlitMaterial::alphaCutoffEnabledChanged,
                     this, &UnlitMaterial::updateEffect);
    QObject::connect(this, &UnlitMaterial::baseColorMapChanged,
                     this, &UnlitMaterial::updateEffect);

    addParameter(m_baseColorUsesTexCoord1);
    addParameter(m_baseColorFactorParameter);
    addParameter(m_baseColorMapParameter);

    setEffect(m_effect);
    updateEffect();
}

UnlitMaterial::~UnlitMaterial()
{
}

bool UnlitMaterial::isBaseColorUsingTexCoord1() const
{
    return m_baseColorUsesTexCoord1->value().value<bool>();
}

QColor UnlitMaterial::baseColorFactor() const
{
    return m_baseColorFactorParameter->value().value<QColor>();
}

Qt3DRender::QAbstractTexture *UnlitMaterial::baseColorMap() const
{
    return m_baseColorMapParameter->value().value<Qt3DRender::QAbstractTexture *>();
}

void UnlitMaterial::setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1)
{
    m_baseColorUsesTexCoord1->setValue(baseColorUsesTexCoord1);
}

void UnlitMaterial::setBaseColorFactor(const QColor &baseColorFactor)
{
    m_baseColorFactorParameter->setValue(baseColorFactor);
}

void UnlitMaterial::setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap)
{
    if (baseColorMap == m_baseColorMapParameter->value().value<Qt3DRender::QAbstractTexture *>())
        return;
    m_baseColorMapParameter->setValue(QVariant::fromValue(baseColorMap));
    if (baseColorMap != nullptr)
        baseColorMap->setFormat(Qt3DRender::QAbstractTexture::TextureFormat::SRGB8_Alpha8);
}

void UnlitMaterial::updateEffect()
{
    m_effect->setBaseColorMapEnabled(baseColorMap() != nullptr);
    m_effect->setUsingColorAttribute(isUsingColorAttribute());
    m_effect->setDoubleSided(isDoubleSided());
    m_effect->setOpaque(isOpaque());
    m_effect->setAlphaCutoffEnabled(isAlphaCutoffEnabled());
    m_effect->setUseSkinning(m_usesSkinning);
}

} // namespace Kuesa

QT_END_NAMESPACE
