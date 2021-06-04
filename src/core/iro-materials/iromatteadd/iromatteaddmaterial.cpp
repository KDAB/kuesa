/*
    iromatteaddmaterial.cpp

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

#include "iromatteaddmaterial.h"
#include "iromatteaddproperties.h"
#include "iromatteaddshaderdata_p.h"
#include <Qt3DRender/qparameter.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::IroMatteAddMaterial
    \inheaderfile Kuesa/IroMatteAddMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteAddMaterial is a simple material in terms of rendering that perform lookups on a 2d map optionally blended with the per vertex color input and performs additive color blending.
*/

/*!
    \qmltype IroMatteAddMaterial
    \instantiates Kuesa::IroMatteAddMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteAddMaterial is a simple material in terms of rendering that perform lookups on a 2d map optionally blended with the per vertex color input and performs additive color blending.
*/

IroMatteAddMaterial::IroMatteAddMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("properties"), {}))
    , m_matteMapParameter(new Qt3DRender::QParameter(QStringLiteral("matteMap"), {}))
{
    addParameter(m_shaderDataParameter);
    addParameter(m_matteMapParameter);
}

IroMatteAddMaterial::~IroMatteAddMaterial() = default;

/*!
    \property IroMatteAddMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

/*!
    \qmlproperty IroMatteAddProperties IroMatteAddMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

Kuesa::IroMatteAddProperties *IroMatteAddMaterial::materialProperties() const
{
    return m_materialProperties;
}

void IroMatteAddMaterial::setMaterialProperties(Kuesa::IroMatteAddProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        if (m_materialProperties)
            m_materialProperties->disconnect(this);

        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            QObject::connect(m_materialProperties, &IroMatteAddProperties::matteMapChanged, this, [this](Qt3DRender::QAbstractTexture *t) { m_matteMapParameter->setValue(QVariant::fromValue(t)); });

            m_matteMapParameter->setValue(QVariant::fromValue(m_materialProperties->matteMap()));

            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void IroMatteAddMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::IroMatteAddProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
