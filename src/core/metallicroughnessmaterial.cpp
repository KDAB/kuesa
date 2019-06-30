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

#include "metallicroughnessproperties.h"
#include "metallicroughnessshaderdata_p.h"

#include "sceneentity.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::MetallicRoughnessMaterial
    \inheaderfile Kuesa/MetallicRoughnessMaterial
    \inmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa::MetallicRoughnessMaterial is a ready to use physically based
    rendering (PBR) material based on the glTF 2.0 material description.

    It is configured through the set of properties held on the
    MetallicRoughnessProperties instance it can receive.

    \note Kuesa::MetallicRoughnessMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \note The effect behind this material needs a brdfLut texture. If the brdfLut
    texture in the effect is not set and the material is added to a subtree of a
    Kuesa::SceneEntity, it will look for the Kuesa::SceneEntity and will use the
    brdfLut texture in the texture collection.

    \sa Kuesa::MetallicRoughnessProperties
*/

/*!
    \property MetallicRoughnessProperties::metallicRoughnessProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.1
 */

/*!
    \qmltype MetallicRoughnessMaterial
    \instantiates Kuesa::MetallicRoughnessMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa.MetallicRoughnessMaterial is a ready to use physically based
    rendering (PBR) material based on the glTF 2.0 material description.

    It is configured through the set of properties held on the
    MetallicRoughnessProperties instance it can receive.

    \note Kuesa.MetallicRoughnessMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \note The effect behind this material needs a brdfLut texture. If the brdfLut
    texture in the effect is not set and the material is added to a subtree of a
    Kuesa.SceneEntity, it will look for the Kuesa.SceneEntity and will use the
    brdfLut texture in the texture collection.

    \sa Kuesa.MetallicRoughnessProperties
 */

/*!
    \qmlproperty MetallicRoughnessProperties MetallicRoughnessMaterial::metallicRoughnessProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.1
 */


MetallicRoughnessMaterial::MetallicRoughnessMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_metallicRoughnessProperties(nullptr)
    , m_metallicRoughnessShaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("metallicRoughness"), {}))
{
    addParameter(m_metallicRoughnessShaderDataParameter);
}

MetallicRoughnessMaterial::~MetallicRoughnessMaterial()
{
}

MetallicRoughnessProperties *MetallicRoughnessMaterial::metallicRoughnessProperties() const
{
    return m_metallicRoughnessProperties;
}

void MetallicRoughnessMaterial::setMetallicRoughnessProperties(MetallicRoughnessProperties *metallicRoughnessProperties)
{
    if (m_metallicRoughnessProperties != metallicRoughnessProperties) {

        if (m_metallicRoughnessProperties)
            QObject::disconnect(m_textureTransformChangedConnection);

        m_metallicRoughnessProperties = metallicRoughnessProperties;
        emit metallicRoughnessPropertiesChanged(metallicRoughnessProperties);

        if (m_metallicRoughnessProperties) {
            m_metallicRoughnessShaderDataParameter->setValue(QVariant::fromValue(metallicRoughnessProperties->shaderData()));
            metallicRoughnessProperties->addClientMaterial(this);

            m_textureTransformChangedConnection = QObject::connect(m_metallicRoughnessProperties,
                                                                   &MetallicRoughnessProperties::textureTransformChanged,
                                                                   this,
                                                                   [this] (const QMatrix3x3 &m) {
                m_textureTransformParameter->setValue(QVariant::fromValue(m));
            });
        }
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
