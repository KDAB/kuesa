
/*
    examplecustommaterialproperties.cpp

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

#include "examplecustommaterialproperties.h"
#include "examplecustommaterialshaderdata_p.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::ExampleCustomMaterialProperties
    \inheaderfile Kuesa/ExampleCustomMaterialProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::ExampleCustomMaterialProperties holds the properties controlling the visual appearance of a ExampleCustomMaterialMaterial instance
*/

/*!
    \qmltype ExampleCustomMaterialProperties
    \instantiates Kuesa::ExampleCustomMaterialProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::ExampleCustomMaterialProperties holds the properties controlling the visual appearance of a ExampleCustomMaterialMaterial instance
*/

ExampleCustomMaterialProperties::ExampleCustomMaterialProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new ExampleCustomMaterialShaderData(this))
{
    QObject::connect(m_shaderData, &ExampleCustomMaterialShaderData::modulatorChanged, this, &ExampleCustomMaterialProperties::modulatorChanged);
    QObject::connect(m_shaderData, &ExampleCustomMaterialShaderData::inputTextureChanged, this, &ExampleCustomMaterialProperties::inputTextureChanged);

}

ExampleCustomMaterialProperties::~ExampleCustomMaterialProperties() = default;

Qt3DRender::QShaderData *ExampleCustomMaterialProperties::shaderData() const
{
    return m_shaderData;
}

void ExampleCustomMaterialProperties::setModulator(float modulator)
{
    m_shaderData->setModulator(modulator);
}

void ExampleCustomMaterialProperties::setInputTexture(Qt3DRender::QAbstractTexture * inputTexture)
{
    m_shaderData->setInputTexture(inputTexture);
}


/*!
    \qmlproperty float ExampleCustomMaterialProperties::modulator
    
*/
/*!
    \property ExampleCustomMaterialProperties::modulator
    
*/
float ExampleCustomMaterialProperties::modulator() const
{
    return m_shaderData->modulator();
}

/*!
    \qmlproperty Qt3DRender::QAbstractTexture * ExampleCustomMaterialProperties::inputTexture
    
*/
/*!
    \property ExampleCustomMaterialProperties::inputTexture
    
*/
Qt3DRender::QAbstractTexture * ExampleCustomMaterialProperties::inputTexture() const
{
    return m_shaderData->inputTexture();
}


} // namespace Kuesa

QT_END_NAMESPACE
