
/*
    iro2glasssemmaterial.cpp

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

#include "iro2glasssemmaterial.h"
#include "iro2glasssemproperties.h"
#include "iro2glasssemshaderdata_p.h"
#include <Qt3DRender/qparameter.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::Iro2GlassSemMaterial
    \inheaderfile Kuesa/Iro2GlassSemMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2GlassSemMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map to render transparent, glass like material.
It performs 2 render passes. The first one performs multiplicative blending using the glass filter simulating the tint of the glass. The second pass performs additive blending for the reflections. This variant uses a fake normal map and SEM reflection, see Iro2GlassEquiRect to use a real normal map and equi-rectangular projection.
*/

/*!
    \qmltype Iro2GlassSemMaterial
    \instantiates Kuesa::Iro2GlassSemMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2GlassSemMaterial is a simple material in terms of rendering that relies on a 2d spherical environment map to render transparent, glass like material.
It performs 2 render passes. The first one performs multiplicative blending using the glass filter simulating the tint of the glass. The second pass performs additive blending for the reflections. This variant uses a fake normal map and SEM reflection, see Iro2GlassEquiRect to use a real normal map and equi-rectangular projection.
*/

Iro2GlassSemMaterial::Iro2GlassSemMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("properties"), {}))
    , m_reflectionMapParameter(new Qt3DRender::QParameter(QStringLiteral("reflectionMap"), {}))
    , m_normalMapParameter(new Qt3DRender::QParameter(QStringLiteral("normalMap"), {}))
{
    addParameter(m_shaderDataParameter);
    addParameter(m_reflectionMapParameter);
    addParameter(m_normalMapParameter);
}

Iro2GlassSemMaterial::~Iro2GlassSemMaterial() = default;

/*!
    \property Iro2GlassSemMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty Iro2GlassSemProperties Iro2GlassSemMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.3
 */

Kuesa::Iro2GlassSemProperties *Iro2GlassSemMaterial::materialProperties() const
{
    return m_materialProperties;
}

void Iro2GlassSemMaterial::setMaterialProperties(Kuesa::Iro2GlassSemProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        if (m_materialProperties)
            m_materialProperties->disconnect(this);

        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            QObject::connect(m_materialProperties, &Iro2GlassSemProperties::reflectionMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_reflectionMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &Iro2GlassSemProperties::normalMapChanged, this, [this] (Qt3DRender::QAbstractTexture * t) { m_normalMapParameter->setValue(QVariant::fromValue(t)); });

            m_reflectionMapParameter->setValue(QVariant::fromValue(m_materialProperties->reflectionMap()));
            m_normalMapParameter->setValue(QVariant::fromValue(m_materialProperties->normalMap()));

            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void Iro2GlassSemMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::Iro2GlassSemProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
