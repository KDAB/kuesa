/*
    gltf2material.cpp

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

#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qparameter.h>
#include <Kuesa/MorphController>
#include <type_traits>
#include "gltf2material.h"

namespace Kuesa {

QT_BEGIN_NAMESPACE

namespace {

template<class OutputType>
using SignalType = void (GLTF2Material::*)(OutputType);

template<class OutputType>
using ValueTypeMapper = typename std::remove_const<typename std::remove_reference<OutputType>::type>::type;

template<class OutputType>
struct WrappedSignal {
    GLTF2Material *self;
    SignalType<OutputType> sig;
    void operator()(const QVariant &value) const
    {
        std::mem_fn(sig)(self, value.value<ValueTypeMapper<OutputType>>());
    }
};
template<class OutputType>
WrappedSignal<OutputType> wrapParameterSignal(GLTF2Material *self, SignalType<OutputType> sig)
{
    return WrappedSignal<OutputType>{ self, sig };
}

} // namespace

/*!
    \class Kuesa::GLTF2Material
    \inheaderfile Kuesa/GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.1
    \brief Kuesa.GLTF2Material is a subclass of Qt3DRender::QMaterial which is the base class
    of all GLTF2 based material subclasses

    We assume a GLTF2 material will consists on the following base properties:
    \list
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \li morphController: The MorphTarget controller used to control the weights
    to interpolate between different morph targers
    \endlist

    Kuesa::GLTF2Material also contains the status of alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.
*/

/*!
    \property GLTF2Material::textureTransform

    Specifies the transform of the texture. This allows to scale, transform or
    rotate the textures of the material.
 */

/*!
    \property GLTF2Material::usingColorAttribute

    If true, base color calculations will take into account the vertex color
    property of the mesh

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \property GLTF2Material::doubleSided

    Allows to simulate double sided material. This are materials that should be
    rendered from the front and front the back. The normal for the back face
    will be the same as for the front face. This includes normal mapping
    calculations.

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \property GLTF2Material::useSkinning

    If the mesh that has this material applied must be animated using skinning
    techniques, this value must be true. This property will change the vertex
    shader of the material to use one with skinning capabilities.

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \property GLTF2Material::opaque

    If true, the material is opaque. If false, the material is transparent and
    will use alpha blending for transparency.

    \note This enable some extra render passes and will trigger a recompilation
    if changed from true to false or from false to true.
 */

/*!
    \property GLTF2Material::alphaCutoff

    Alpha cutoff threshold. Any fragment with an alpha value higher than this
    property will be considered opaque. Any fragment with an alpha value lower
    than this property will be discarded and wont have any effect on the final
    result. For alphaCutoff to have any effect, it must be activated.

    \sa alphaCutoffEnabled
 */

/*!
    \property GLTF2Material::alphaCutoffEnabled

    If true, alpha cutoff is enabled. Fragments with an alpha value above a
    threshold are rendered as opaque while fragment an alpha value below the
    threshold are discarded
 */

/*!
    \property GLTF2Material::morphController

    The morph controller instance used to control morph targets weights.
    This can be null if a material is not used with morph targets.

    \since Kuesa 1.1
*/

/*!
    \qmltype GLTF2Material
    \instantiates Kuesa::GLTF2Material
    \inqmlmodule Kuesa
    \since Kuesa 1.1

    Kuesa.GLTF2Material is a subclass of Qt3DRender::QMaterial which is the base class
    of all GLTF2 based material subclasses

    We assume a GLTF2 material will consists on the following base properties:
    \list
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \li morphController: The MorphTarget controller used to control the weights
    to interpolate between different morph targers
    \endlist

    Kuesa::GLTF2Material also contains the status of alpha blending and alpha
    cutoff. Alpha blending is used to simulate transparent materials, like a
    glass, while alpha cutoff is used to discard fragment that have an alpha
    value below a threshold. This is useful to render grass or trees for
    example.
 */

/*!
    \qmlproperty matrix3 GLTF2Material::textureTransform

    Specifies the transform of the texture. This allows to scale, transform or
    rotate the textures of the material.
 */

/*!
    \qmlproperty bool GLTF2Material::usingColorAttribute

    If true, base color calculations will take into account the vertex color
    property of the mesh

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool GLTF2Material::doubleSided

    Allows to simulate double sided material. This are materials that should be
    rendered from the front and front the back.

    The normal for the back face will be the same as for the front face. This
    includes normal mapping calculations.

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool GLTF2Material::useSkinning

    If the mesh that has this material applied must be animated using skinning
    techniques, this value must be true. This property will change the vertex
    shader of the material to use one with skinning capabilities.

    \note If this property is changed from true to false or from false to true
    will trigger a recompilation of the shader.
 */

/*!
    \qmlproperty bool GLTF2Material::opaque

    If true, the material is opaque. If false, the material is transparent and
    will use alpha blending for transparency.

    \note This enable some extra render passes and will trigger a recompilation
    if changed from true to false or from false to true.
 */

/*!
    \qmlproperty real GLTF2Material::alphaCutoff

    Alpha cutoff threshold. Any fragment with an alpha value higher than this
    property will be considered opaque. Any fragment with an alpha value lower
    than this property will be discarded and wont have any effect on the final
    result. For alphaCutoff to have any effect, it must be activated.

    \sa alphaCutoffEnabled
 */

/*!
    \qmlproperty bool GLTF2Material::alphaCutoffEnabled

    Enables/disables alphaCutoff

    \note This enable some extra render passes and will trigger a recompilation
    if changed from true to false or from false to true.
 */

/*!
    \qmlproperty Kuesa.MorphController GLTF2Material::morphController

    The morph controller instance used to control morph targets weights.
    This can be null if a material is not used with morph targets.

    \since Kuesa 1.1
*/

GLTF2Material::GLTF2Material(Qt3DCore::QNode *parent)
    : Qt3DRender::QMaterial(parent)
    , m_textureTransformParameter(new Qt3DRender::QParameter(QStringLiteral("texCoordTransform"), QVariant::fromValue(QMatrix3x3())))
    , m_alphaCutoffParameter(new Qt3DRender::QParameter(QStringLiteral("alphaCutoff"), 0.0))
    , m_morphControllerParameter(new Qt3DRender::QParameter(QStringLiteral("morphWeights"), QVariant()))
    , m_isUsingColorAttribute(false)
    , m_isDoubleSided(false)
    , m_usesSkinning(false)
    , m_isOpaque(true)
    , m_isAlphaCutOffEnabled(false)
{
    QObject::connect(m_alphaCutoffParameter, &Qt3DRender::QParameter::valueChanged,
                     this, wrapParameterSignal(this, &GLTF2Material::alphaCutoffChanged));
    QObject::connect(m_textureTransformParameter, &Qt3DRender::QParameter::valueChanged,
                     this, wrapParameterSignal(this, &GLTF2Material::textureTransformChanged));
    QObject::connect(m_morphControllerParameter, &Qt3DRender::QParameter::valueChanged,
                     this, wrapParameterSignal(this, &GLTF2Material::morphControllerChanged));

    addParameter(m_textureTransformParameter);
    addParameter(m_alphaCutoffParameter);
    addParameter(m_morphControllerParameter);
}

GLTF2Material::~GLTF2Material()
{
}

QMatrix3x3 Kuesa::GLTF2Material::textureTransform() const
{
    return m_textureTransformParameter->value().value<QMatrix3x3>();
}

bool GLTF2Material::isUsingColorAttribute() const
{
    return m_isUsingColorAttribute;
}

bool GLTF2Material::isDoubleSided() const
{
    return m_isDoubleSided;
}

bool GLTF2Material::useSkinning() const
{
    return m_usesSkinning;
}

bool GLTF2Material::isOpaque() const
{
    return m_isOpaque;
}

bool GLTF2Material::isAlphaCutoffEnabled() const
{
    return m_isAlphaCutOffEnabled;
}

float GLTF2Material::alphaCutoff() const
{
    return m_alphaCutoffParameter->value().toFloat();
}

MorphController *GLTF2Material::morphController() const
{
    return m_morphControllerParameter->value().value<MorphController *>();
}

void GLTF2Material::setTextureTransform(const QMatrix3x3 &textureTransform)
{
    m_textureTransformParameter->setValue(QVariant::fromValue(textureTransform));
}

void GLTF2Material::setUsingColorAttribute(bool usingColorAttribute)
{
    if (m_isUsingColorAttribute == usingColorAttribute)
        return;
    m_isUsingColorAttribute = usingColorAttribute;
    emit usingColorAttributeChanged(usingColorAttribute);
}

void GLTF2Material::setDoubleSided(bool doubleSided)
{
    if (m_isDoubleSided == doubleSided)
        return;
    m_isDoubleSided = doubleSided;
    emit doubleSidedChanged(doubleSided);
}

void GLTF2Material::setUseSkinning(bool useSkinning)
{
    if (m_usesSkinning == useSkinning)
        return;
    m_usesSkinning = useSkinning;
    emit useSkinningChanged(useSkinning);
}

void GLTF2Material::setOpaque(bool opaque)
{
    if (m_isOpaque == opaque)
        return;
    m_isOpaque = opaque;
    emit opaqueChanged(opaque);
}

void GLTF2Material::setAlphaCutoffEnabled(bool enabled)
{
    if (m_isAlphaCutOffEnabled == enabled)
        return;
    m_isAlphaCutOffEnabled = enabled;
    emit alphaCutoffEnabledChanged(enabled);
}

void GLTF2Material::setAlphaCutoff(float alphaCutoff)
{
    if (!m_isAlphaCutOffEnabled)
        setAlphaCutoffEnabled(true);
    m_alphaCutoffParameter->setValue(QVariant::fromValue(alphaCutoff));
}

void GLTF2Material::setMorphController(MorphController *morphController)
{
    if (morphController == m_morphControllerParameter->value().value<MorphController *>())
        return;

    m_morphControllerParameter->setValue(QVariant::fromValue(morphController));
}

} // namespace Kuesa

QT_END_NAMESPACE
