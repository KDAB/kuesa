
/*
    diffusesphericalenvmapmaterial.cpp

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

#include "diffusesphericalenvmapmaterial.h"
#include "diffusesphericalenvmapproperties.h"
#include "diffusesphericalenvmapshaderdata_p.h"
#include <Qt3DRender/qparameter.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::DiffuseSphericalEnvMapMaterial
    \inheaderfile Kuesa/DiffuseSphericalEnvMapMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::DiffuseSphericalEnvMapMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and a 2d diffuse map to render semi glossy like finishes like clear coated paint.
*/

/*!
    \qmltype DiffuseSphericalEnvMapMaterial
    \instantiates Kuesa::DiffuseSphericalEnvMapMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::DiffuseSphericalEnvMapMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and a 2d diffuse map to render semi glossy like finishes like clear coated paint.
*/

DiffuseSphericalEnvMapMaterial::DiffuseSphericalEnvMapMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent),
    m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("properties"), {}))
{
    addParameter(m_shaderDataParameter);
}

DiffuseSphericalEnvMapMaterial::~DiffuseSphericalEnvMapMaterial() = default;

/*!
    \property DiffuseSphericalEnvMapMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

/*!
    \qmlproperty DiffuseSphericalEnvMapProperties DiffuseSphericalEnvMapMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

Kuesa::DiffuseSphericalEnvMapProperties *DiffuseSphericalEnvMapMaterial::materialProperties() const
{
    return m_materialProperties;
}

void DiffuseSphericalEnvMapMaterial::setMaterialProperties(Kuesa::DiffuseSphericalEnvMapProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void DiffuseSphericalEnvMapMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::DiffuseSphericalEnvMapProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
