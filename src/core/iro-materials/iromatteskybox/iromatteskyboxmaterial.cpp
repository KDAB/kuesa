
/*
    iromatteskyboxmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "iromatteskyboxmaterial.h"
#include "iromatteskyboxproperties.h"
#include "iromatteskyboxshaderdata_p.h"
#include <Qt3DRender/qparameter.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::IroMatteSkyboxMaterial
    \inheaderfile Kuesa/IroMatteSkyboxMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteSkyboxMaterial is a simple material in terms of
    rendering that allows to render a skybox for arbitrary meshes by
    translating the vertices to the eye position.

    \note The View or ForwardRenderer should have its \l
    {Kuesa::View::skinning} property set to true to be visible.
*/

/*!
    \qmltype IroMatteSkyboxMaterial
    \instantiates Kuesa::IroMatteSkyboxMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteSkyboxMaterial is a simple material in terms of
    rendering that allows to render a skybox for arbitrary meshes by
    translating the vertices to the eye position.

    \note The View or ForwardRenderer should have its \l [QML]
    {Kuesa::View::skinning} property set to true to be visible.
*/

IroMatteSkyboxMaterial::IroMatteSkyboxMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("properties"), {}))
    , m_matteMapParameter(new Qt3DRender::QParameter(QStringLiteral("matteMap"), {}))
{
    addParameter(m_shaderDataParameter);
    addParameter(m_matteMapParameter);
}

IroMatteSkyboxMaterial::~IroMatteSkyboxMaterial() = default;

/*!
    \property IroMatteSkyboxMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

/*!
    \qmlproperty IroMatteSkyboxProperties IroMatteSkyboxMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

Kuesa::IroMatteSkyboxProperties *IroMatteSkyboxMaterial::materialProperties() const
{
    return m_materialProperties;
}

void IroMatteSkyboxMaterial::setMaterialProperties(Kuesa::IroMatteSkyboxProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        if (m_materialProperties)
            m_materialProperties->disconnect(this);

        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            QObject::connect(m_materialProperties, &IroMatteSkyboxProperties::matteMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_matteMapParameter->setValue(QVariant::fromValue(t)); });

            m_matteMapParameter->setValue(QVariant::fromValue(m_materialProperties->matteMap()));

            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void IroMatteSkyboxMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::IroMatteSkyboxProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
