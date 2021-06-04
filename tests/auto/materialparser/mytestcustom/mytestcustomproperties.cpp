
/*
    mytestcustomproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "mytestcustomproperties.h"
#include "mytestcustomshaderdata_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::MyTestCustomProperties
    \inheaderfile Kuesa/MyTestCustomProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::MyTestCustomProperties holds the properties controlling the visual appearance of a MyTestCustomMaterial instance
*/

/*!
    \qmltype MyTestCustomProperties
    \instantiates Kuesa::MyTestCustomProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::MyTestCustomProperties holds the properties controlling the visual appearance of a MyTestCustomMaterial instance
*/

MyTestCustomProperties::MyTestCustomProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new MyTestCustomShaderData(this))
{
    QObject::connect(m_shaderData, &MyTestCustomShaderData::customVec4ParameterChanged, this, &MyTestCustomProperties::customVec4ParameterChanged);
    QObject::connect(m_shaderData, &MyTestCustomShaderData::customVec3ParameterChanged, this, &MyTestCustomProperties::customVec3ParameterChanged);
    QObject::connect(m_shaderData, &MyTestCustomShaderData::customVec2ParameterChanged, this, &MyTestCustomProperties::customVec2ParameterChanged);
    QObject::connect(m_shaderData, &MyTestCustomShaderData::customFloatParameterChanged, this, &MyTestCustomProperties::customFloatParameterChanged);
}

MyTestCustomProperties::~MyTestCustomProperties() = default;

Qt3DRender::QShaderData *MyTestCustomProperties::shaderData() const
{
    return m_shaderData;
}

void MyTestCustomProperties::setCustomVec4Parameter(const QVector4D &customVec4Parameter)
{
    m_shaderData->setCustomVec4Parameter(customVec4Parameter);
}

void MyTestCustomProperties::setCustomVec3Parameter(const QVector3D &customVec3Parameter)
{
    m_shaderData->setCustomVec3Parameter(customVec3Parameter);
}

void MyTestCustomProperties::setCustomVec2Parameter(const QVector2D &customVec2Parameter)
{
    m_shaderData->setCustomVec2Parameter(customVec2Parameter);
}

void MyTestCustomProperties::setCustomFloatParameter(float customFloatParameter)
{
    m_shaderData->setCustomFloatParameter(customFloatParameter);
}

/*!
    \qmlproperty QVector4D MyTestCustomProperties::customVec4Parameter
    
*/
/*!
    \property MyTestCustomProperties::customVec4Parameter
    
*/
QVector4D MyTestCustomProperties::customVec4Parameter() const
{
    return m_shaderData->customVec4Parameter();
}

/*!
    \qmlproperty QVector3D MyTestCustomProperties::customVec3Parameter
    
*/
/*!
    \property MyTestCustomProperties::customVec3Parameter
    
*/
QVector3D MyTestCustomProperties::customVec3Parameter() const
{
    return m_shaderData->customVec3Parameter();
}

/*!
    \qmlproperty QVector2D MyTestCustomProperties::customVec2Parameter
    
*/
/*!
    \property MyTestCustomProperties::customVec2Parameter
    
*/
QVector2D MyTestCustomProperties::customVec2Parameter() const
{
    return m_shaderData->customVec2Parameter();
}

/*!
    \qmlproperty float MyTestCustomProperties::customFloatParameter
    
*/
/*!
    \property MyTestCustomProperties::customFloatParameter
    
*/
float MyTestCustomProperties::customFloatParameter() const
{
    return m_shaderData->customFloatParameter();
}

} // namespace Kuesa

QT_END_NAMESPACE
