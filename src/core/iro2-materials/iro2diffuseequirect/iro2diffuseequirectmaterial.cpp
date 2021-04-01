/*
    iro2diffuseequirectmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    This file was auto-generated

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

#include "iro2diffuseequirectmaterial.h"
#include "iro2diffuseequirectproperties.h"
#include "iro2diffuseequirectshaderdata_p.h"
#include <Qt3DRender/qparameter.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::Iro2DiffuseEquiRectMaterial
    \inheaderfile Kuesa/Iro2DiffuseEquiRectMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseEquiRectMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and a 2d diffuse map to render semi glossy like finishes like clear coated paint. This variant uses a real normal map and equi-rectangular projection, see Iro2DiffuseSem to use a fake normal map and SEM reflection.
*/

/*!
    \qmltype Iro2DiffuseEquiRectMaterial
    \instantiates Kuesa::Iro2DiffuseEquiRectMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2DiffuseEquiRectMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and a 2d diffuse map to render semi glossy like finishes like clear coated paint. This variant uses a real normal map and equi-rectangular projection, see Iro2DiffuseSem to use a fake normal map and SEM reflection.
*/

Iro2DiffuseEquiRectMaterial::Iro2DiffuseEquiRectMaterial(Qt3DCore::QNode *parent)
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

Iro2DiffuseEquiRectMaterial::~Iro2DiffuseEquiRectMaterial() = default;

/*!
    \property Iro2DiffuseEquiRectMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty Iro2DiffuseEquiRectProperties Iro2DiffuseEquiRectMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

Kuesa::Iro2DiffuseEquiRectProperties *Iro2DiffuseEquiRectMaterial::materialProperties() const
{
    return m_materialProperties;
}

void Iro2DiffuseEquiRectMaterial::setMaterialProperties(Kuesa::Iro2DiffuseEquiRectProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        if (m_materialProperties)
            m_materialProperties->disconnect(this);

        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            QObject::connect(m_materialProperties, &Iro2DiffuseEquiRectProperties::diffuseMapChanged, this, [this](Qt3DRender::QAbstractTexture *t) { m_diffuseMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &Iro2DiffuseEquiRectProperties::reflectionMapChanged, this, [this](Qt3DRender::QAbstractTexture *t) { m_reflectionMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &Iro2DiffuseEquiRectProperties::normalMapChanged, this, [this](Qt3DRender::QAbstractTexture *t) { m_normalMapParameter->setValue(QVariant::fromValue(t)); });

            m_diffuseMapParameter->setValue(QVariant::fromValue(m_materialProperties->diffuseMap()));
            m_reflectionMapParameter->setValue(QVariant::fromValue(m_materialProperties->reflectionMap()));
            m_normalMapParameter->setValue(QVariant::fromValue(m_materialProperties->normalMap()));

            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void Iro2DiffuseEquiRectMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::Iro2DiffuseEquiRectProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
