/*
    gltf2materialproperties.cpp

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

#include "gltf2materialproperties.h"

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QTexture>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::GLTF2MaterialProperties
    \inheaderfile Kuesa/GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.1

    \brief GLTF2MaterialProperties is a base class holding properties common to
    all glTF2 Materials.

    A GLTF2MaterialProperties consists of the following base properties:
    \list
    \li alphaCutOff: The alpha cut off threshold
    \li baseColorUsesTexCoord1: Whether we use the first or second set of
    texture coordinates
    \li baseColorFactor: Base color of the material
    \li baseColorMap: A texture specifying the base color of the material
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \endlist
*/

/*!
    \qmlproperty bool GLTF2MaterialProperties::baseColorUsesTexCoord1

    False if the map must use the 0th texture coordinate set, binded to
    'defaultTextureCoordinateName' attribute.
    True if the map must use the 1st texture coordinate set, binded to
    'defaultTextureCoordinate1Name' attribute.
 */

/*!
    \property GLTF2MaterialProperties::textureTransform

    Specifies the transform of the texture. This allows to scale, transform or
    rotate the textures of the material.
 */

/*!
    \property GLTF2MaterialProperties::baseColorFactor

    Specifies the baseColorFactor of the material
*/

/*!
    \property GLTF2MaterialProperties::baseColorMap

    Specifies a texture to be used as baseColorFactor. The content of the
    texture is expected to be in sRGB color space.

    \note If this property is nullptr and was set to a non nullptr value, this
    will trigger a recompilation of the shader. If it was a non nullptr value
    and is set to nullptr, it will also trigger a recompilation of the shader.
 */

/*!
    \property GLTF2MaterialProperties::alphaCutoff

    Alpha cutoff threshold. Any fragment with an alpha value higher than this
    property will be considered opaque. Any fragment with an alpha value lower
    than this property will be discarded and wont have any effect on the final
    result. For alphaCutoff to have any effect, it must be activated.
 */

/*!
    \qmlproperty bool GLTF2MaterialProperties::receivesShadows

    Specifies whether a surface should show any shadows that are cast on it.  This
    Default is true
 */

/*!
    \property GLTF2MaterialProperties::receivesShadows

    Specifies whether a surface should show any shadows that are cast on it.  This
    Default is true
 */

GLTF2MaterialProperties::GLTF2MaterialProperties(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_alphaCutOff(0.0f)
    , m_usesTexCoord1(false)
    , m_receivesShadows(true)
    , m_baseColorTexture(nullptr)
    , m_baseColorFactor(QColor("gray"))
{
}

GLTF2MaterialProperties::~GLTF2MaterialProperties()
{
    for (auto &connection : m_connections)
        QObject::disconnect(connection);
}

void GLTF2MaterialProperties::addClientMaterial(Qt3DRender::QMaterial *material)
{
    m_clientMaterials.push_back(material);
    m_connections.push_back(QObject::connect(material, &Qt3DRender::QMaterial::nodeDestroyed,
                         [this, material]() {
        m_clientMaterials.removeAll(material);
    }));
}

QVector<Qt3DRender::QMaterial *> GLTF2MaterialProperties::clientMaterials() const
{
    return m_clientMaterials;
}

void GLTF2MaterialProperties::setShadowMapDepthTexture(Qt3DRender::QAbstractTexture *depthTexture)
{
    if (m_shadowMapTexture == depthTexture)
        return;

    m_shadowMapTexture = depthTexture;
    emit shadowMapDepthTextureChanged(m_shadowMapTexture);
}

void GLTF2MaterialProperties::setShadowMapCubeDepthTexture(Qt3DRender::QAbstractTexture *cubeMapDepthTexture)
{
    if (m_shadowMapCubeTexture == cubeMapDepthTexture)
        return;

    m_shadowMapCubeTexture = cubeMapDepthTexture;
    emit shadowMapCubeDepthTextureChanged(m_shadowMapCubeTexture);
}

Qt3DRender::QAbstractTexture *GLTF2MaterialProperties::shadowMapDepthTexture() const
{
    return m_shadowMapTexture;
}

Qt3DRender::QAbstractTexture *GLTF2MaterialProperties::shadowMapCubeDepthTexture() const
{
    return m_shadowMapCubeTexture;
}

bool GLTF2MaterialProperties::receivesShadows() const
{
    return m_receivesShadows;
}

bool GLTF2MaterialProperties::isBaseColorUsingTexCoord1() const
{
    return m_usesTexCoord1;
}

QColor GLTF2MaterialProperties::baseColorFactor() const
{
    return m_baseColorFactor;
}

Qt3DRender::QAbstractTexture *GLTF2MaterialProperties::baseColorMap() const
{
    return m_baseColorTexture;
}

float GLTF2MaterialProperties::alphaCutoff() const
{
    return m_alphaCutOff;
}

void GLTF2MaterialProperties::setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1)
{
    if (m_usesTexCoord1 == baseColorUsesTexCoord1)
        return;
    m_usesTexCoord1 = baseColorUsesTexCoord1;
    emit baseColorUsesTexCoord1Changed(m_usesTexCoord1);
}

void GLTF2MaterialProperties::setBaseColorFactor(const QColor &baseColorFactor)
{
    if (m_baseColorFactor == baseColorFactor)
        return;
    m_baseColorFactor = baseColorFactor;
    emit baseColorFactorChanged(m_baseColorFactor);
}

void GLTF2MaterialProperties::setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap)
{
    if (m_baseColorTexture == baseColorMap)
        return;
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_baseColorTexture != nullptr)
        d->unregisterDestructionHelper(m_baseColorTexture);
    m_baseColorTexture = baseColorMap;
    if (m_baseColorTexture != nullptr)
        d->registerDestructionHelper(m_baseColorTexture, &GLTF2MaterialProperties::setBaseColorMap, m_baseColorTexture);
    emit baseColorMapChanged(m_baseColorTexture);
}

void GLTF2MaterialProperties::setAlphaCutoff(float alphaCutoff)
{
    if (qFuzzyCompare(m_alphaCutOff, alphaCutoff))
        return;
    m_alphaCutOff = alphaCutoff;
    emit alphaCutoffChanged(m_alphaCutOff);
}

void Kuesa::GLTF2MaterialProperties::setReceivesShadows(bool receivesShadows)
{
    if (m_receivesShadows == receivesShadows)
        return;

    m_receivesShadows = receivesShadows;
    emit receivesShadowsChanged(m_receivesShadows);
}

} // namespace Kuesa

QT_END_NAMESPACE
