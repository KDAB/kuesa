/*
    gltf2material.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "gltf2material.h"
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qparameter.h>
#include <Kuesa/MorphController>
#include <type_traits>
#include <QMatrix3x3>

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

    \brief GLTF2Material is a subclass of Qt3DRender::QMaterial which is the
    base class of all GLTF2 based material subclasses.

    We assume a GLTF2 material consists of the following base properties:
    \list
    \li morphController: The MorphTarget controller used to control the weights
    to interpolate between different morph targers
    \endlist
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

    \brief GLTF2Material is a subclass of Qt3DRender::QMaterial which is the
    base class of all GLTF2 based material subclasses.

    We assume a GLTF2 material consists of the following base properties:
    \list
    \li morphController: The MorphTarget controller used to control the weights
    to interpolate between different morph targers
    \endlist
 */

/*!
    \qmlproperty Kuesa.MorphController GLTF2Material::morphController

    The morph controller instance used to control morph targets weights.
    This can be null if a material is not used with morph targets.

    \since Kuesa 1.1
*/

GLTF2Material::GLTF2Material(Qt3DCore::QNode *parent)
    : Qt3DRender::QMaterial(parent)
    , m_morphControllerParameter(new Qt3DRender::QParameter(QStringLiteral("morphWeights"), QVariant()))
    , m_textureTransformParameter(new Qt3DRender::QParameter(QStringLiteral("texCoordTransform"), QVariant::fromValue(QMatrix3x3())))
    , m_shadowMapDepthTextureParameter(new Qt3DRender::QParameter(QStringLiteral("shadowMapDepthTextureArray"), {}))
    , m_shadowMapCubeDepthTextureParameter(new Qt3DRender::QParameter(QStringLiteral("shadowMapCubeDepthTexture"), {}))
{
    QObject::connect(m_morphControllerParameter, &Qt3DRender::QParameter::valueChanged,
                     this, wrapParameterSignal(this, &GLTF2Material::morphControllerChanged));

    MorphController *defaultMorphController = new MorphController(this);
    defaultMorphController->setCount(8);
    m_morphControllerParameter->setValue(QVariant::fromValue(defaultMorphController));

    addParameter(m_morphControllerParameter);
    addParameter(m_textureTransformParameter);
    addParameter(m_shadowMapDepthTextureParameter);
    addParameter(m_shadowMapCubeDepthTextureParameter);
}

GLTF2Material::~GLTF2Material()
{
}

MorphController *GLTF2Material::morphController() const
{
    return m_morphControllerParameter->value().value<MorphController *>();
}

void GLTF2Material::setMorphController(MorphController *morphController)
{
    if (morphController == m_morphControllerParameter->value().value<MorphController *>())
        return;

    m_morphControllerParameter->setValue(QVariant::fromValue(morphController));
}

void GLTF2Material::setShadowMapDepthTexture(Qt3DRender::QAbstractTexture *depthTexture)
{
    m_shadowMapDepthTextureParameter->setValue(QVariant::fromValue(depthTexture));
}

void GLTF2Material::setShadowMapCubeDepthTexture(Qt3DRender::QAbstractTexture *cubeDepthTexture)
{
    m_shadowMapCubeDepthTextureParameter->setValue(QVariant::fromValue(cubeDepthTexture));
}

} // namespace Kuesa

QT_END_NAMESPACE
