
/*
    mytestcustommaterial.cpp

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

#include "mytestcustommaterial.h"
#include "mytestcustomproperties.h"
#include "mytestcustomshaderdata_p.h"
#include <Qt3DRender/qparameter.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::MyTestCustomMaterial
    \inheaderfile Kuesa/MyTestCustomMaterial
    \inherits Kuesa::GLTF2Material
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::MyTestCustomMaterial 
*/

/*!
    \qmltype MyTestCustomMaterial
    \instantiates Kuesa::MyTestCustomMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::MyTestCustomMaterial 
*/

MyTestCustomMaterial::MyTestCustomMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent),
    m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("properties"), {}))
{
    addParameter(m_shaderDataParameter);
}

MyTestCustomMaterial::~MyTestCustomMaterial() = default;

/*!
    \property MyTestCustomMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

/*!
    \qmlproperty MyTestCustomProperties MyTestCustomMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

Kuesa::MyTestCustomProperties *MyTestCustomMaterial::materialProperties() const
{
    return m_materialProperties;
}

void MyTestCustomMaterial::setMaterialProperties(Kuesa::MyTestCustomProperties *materialProperties)
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

void MyTestCustomMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::MyTestCustomProperties *>(materialProperties));
}

} // namespace Kuesa

QT_END_NAMESPACE
