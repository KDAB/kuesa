
/*
    iro2diffusesemmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Nicolas Guichard <nicolas.guichard@kdab.com>

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

#include "iro2diffusesemmaterial.h"
#include "iro2diffusesemproperties.h"
#include "iro2diffusesemshaderdata_p.h"
#include <Qt3DRender/qparameter.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::Iro2DiffuseSemMaterial
    \inheaderfile Kuesa/Iro2DiffuseSemMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseSemMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and a 2d diffuse map to render semi glossy like finishes like clear coated paint. This variant uses a fake normal map and SEM reflection, see Iro2DiffuseEquiRect to use a real normal map and equi-rectangular projection.
*/

/*!
    \qmltype Iro2DiffuseSemMaterial
    \instantiates Kuesa::Iro2DiffuseSemMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseSemMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and a 2d diffuse map to render semi glossy like finishes like clear coated paint. This variant uses a fake normal map and SEM reflection, see Iro2DiffuseEquiRect to use a real normal map and equi-rectangular projection.
*/

Iro2DiffuseSemMaterial::Iro2DiffuseSemMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("properties"), {}))
    , m_diffuseMapParameter(new Qt3DRender::QParameter(QStringLiteral("diffuseMap"), {}))
    , m_reflectionMapParameter(new Qt3DRender::QParameter(QStringLiteral("reflectionMap"), {}))
    , m_normalMapParameter(new Qt3DRender::QParameter(QStringLiteral("normalMap"), {}))
{
    addParameter(m_shaderDataParameter);
    addParameter(m_diffuseMapParameter);
    addParameter(m_reflectionMapParameter);
    addParameter(m_normalMapParameter);
}

Iro2DiffuseSemMaterial::~Iro2DiffuseSemMaterial() = default;

/*!
    \property Iro2DiffuseSemMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty Iro2DiffuseSemProperties Iro2DiffuseSemMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

Kuesa::Iro2DiffuseSemProperties *Iro2DiffuseSemMaterial::materialProperties() const
{
    return m_materialProperties;
}

void Iro2DiffuseSemMaterial::setMaterialProperties(Kuesa::Iro2DiffuseSemProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        if (m_materialProperties)
            m_materialProperties->disconnect(this);

        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            QObject::connect(m_materialProperties, &Iro2DiffuseSemProperties::diffuseMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_diffuseMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &Iro2DiffuseSemProperties::reflectionMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_reflectionMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &Iro2DiffuseSemProperties::normalMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_normalMapParameter->setValue(QVariant::fromValue(t)); });

            m_diffuseMapParameter->setValue(QVariant::fromValue(m_materialProperties->diffuseMap()));
            m_reflectionMapParameter->setValue(QVariant::fromValue(m_materialProperties->reflectionMap()));
            m_normalMapParameter->setValue(QVariant::fromValue(m_materialProperties->normalMap()));

            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void Iro2DiffuseSemMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::Iro2DiffuseSemProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
