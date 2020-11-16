
/*
    iro2alphaequirectmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "iro2alphaequirectmaterial.h"
#include "iro2alphaequirectproperties.h"
#include "iro2alphaequirectshaderdata_p.h"
#include <Qt3DRender/qparameter.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::Iro2AlphaEquiRectMaterial
    \inheaderfile Kuesa/Iro2AlphaEquiRectMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2AlphaEquiRectMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and performs alpha blending. This variant uses a real normal map and equi-rectangular projection, see Iro2AlphaSem to use a fake normal map and SEM reflection.
*/

/*!
    \qmltype Iro2AlphaEquiRectMaterial
    \instantiates Kuesa::Iro2AlphaEquiRectMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2AlphaEquiRectMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map and performs alpha blending. This variant uses a real normal map and equi-rectangular projection, see Iro2AlphaSem to use a fake normal map and SEM reflection.
*/

Iro2AlphaEquiRectMaterial::Iro2AlphaEquiRectMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("properties"), {}))
    , m_reflectionMapParameter(new Qt3DRender::QParameter(QStringLiteral("reflectionMap"), {}))
    , m_normalMapParameter(new Qt3DRender::QParameter(QStringLiteral("normalMap"), {}))
{
    addParameter(m_shaderDataParameter);
    addParameter(m_reflectionMapParameter);
    addParameter(m_normalMapParameter);
}

Iro2AlphaEquiRectMaterial::~Iro2AlphaEquiRectMaterial() = default;

/*!
    \property Iro2AlphaEquiRectMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty Iro2AlphaEquiRectProperties Iro2AlphaEquiRectMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

Kuesa::Iro2AlphaEquiRectProperties *Iro2AlphaEquiRectMaterial::materialProperties() const
{
    return m_materialProperties;
}

void Iro2AlphaEquiRectMaterial::setMaterialProperties(Kuesa::Iro2AlphaEquiRectProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        if (m_materialProperties)
            m_materialProperties->disconnect(this);

        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            QObject::connect(m_materialProperties, &Iro2AlphaEquiRectProperties::reflectionMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_reflectionMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &Iro2AlphaEquiRectProperties::normalMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_normalMapParameter->setValue(QVariant::fromValue(t)); });

            m_reflectionMapParameter->setValue(QVariant::fromValue(m_materialProperties->reflectionMap()));
            m_normalMapParameter->setValue(QVariant::fromValue(m_materialProperties->normalMap()));

            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void Iro2AlphaEquiRectMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::Iro2AlphaEquiRectProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
