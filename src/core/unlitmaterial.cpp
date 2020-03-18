/*
    unlitmaterial.cpp

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

#include "unlitmaterial.h"
#include "unlitproperties.h"
#include "unlitshaderdata_p.h"
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qabstracttexture.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::UnlitMaterial
    \inheaderfile Kuesa/UnlitMaterial
    \inmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa::UnlitMaterial is a simple material without shading. It
    implements the KHR_materials_unlit extension from glTF 2.0.

    It is configured through the set of properties held on the
    UnlitProperties instance it can receive.

    \note Kuesa::UnlitMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \sa Kuesa::UnlitProperties
*/

/*!
    \property UnlitMaterial::unlitProperties

    The properties defining the appearance of the material.
 */

/*!
    \qmltype UnlitMaterial
    \instantiates Kuesa::UnlitMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa.UnlitMaterial is a simple material without shading. It
    implements the KHR_materials_unlit extension from glTF 2.0.

    The material consists on the following properties:
    \list
    \li baseColorFactor: Base color of the material
    \li baseColorMap: A texture specifying the base color of the material
    \li textureTransform: Allows to transform (scale, translate, rotate) a
    texture.
    \endlist

    It is configured through the set of properties held on the
    UnlitProperties instance it can receive.

    \note Kuesa.UnlitMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \sa Kuesa::UnlitProperties
 */

/*!
    \qmlproperty UnlitProperties UnlitMaterial::unlitProperties

    The properties defining the appearance of the material.
 */

UnlitMaterial::UnlitMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_unlitProperties(nullptr)
    , m_unlitShaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("unlit"), {}))
{
    addParameter(m_unlitShaderDataParameter);
}

UnlitMaterial::~UnlitMaterial()
{
}

UnlitProperties *UnlitMaterial::unlitProperties() const
{
    return m_unlitProperties;
}

void UnlitMaterial::setUnlitProperties(UnlitProperties *unlitProperties)
{
    if (m_unlitProperties != unlitProperties) {

        if (m_unlitProperties)
            QObject::disconnect(m_textureTransformChangedConnection);

        m_unlitProperties = unlitProperties;
        emit unlitPropertiesChanged(unlitProperties);

        if (m_unlitProperties) {
            m_unlitShaderDataParameter->setValue(QVariant::fromValue(m_unlitProperties->shaderData()));
            m_unlitProperties->addClientMaterial(this);

            m_textureTransformChangedConnection = QObject::connect(m_unlitProperties,
                                                                   &UnlitProperties::textureTransformChanged,
                                                                   this,
                                                                   [this](const QMatrix3x3 &m) {
                                                                       m_textureTransformParameter->setValue(QVariant::fromValue(m));
                                                                   });
        }
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
