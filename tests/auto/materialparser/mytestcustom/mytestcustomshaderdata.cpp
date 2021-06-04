
/*
    mytestcustomshaderdata.cpp

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

#include "mytestcustomshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

MyTestCustomShaderData::MyTestCustomShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_customVec4Parameter()
    , m_customVec3Parameter()
    , m_customVec2Parameter()
    , m_customFloatParameter()
{
}

MyTestCustomShaderData::~MyTestCustomShaderData() = default;

QVector4D MyTestCustomShaderData::customVec4Parameter() const
{
    return m_customVec4Parameter;
}

QVector3D MyTestCustomShaderData::customVec3Parameter() const
{
    return m_customVec3Parameter;
}

QVector2D MyTestCustomShaderData::customVec2Parameter() const
{
    return m_customVec2Parameter;
}

float MyTestCustomShaderData::customFloatParameter() const
{
    return m_customFloatParameter;
}

void MyTestCustomShaderData::setCustomVec4Parameter(const QVector4D &customVec4Parameter)
{
    if (m_customVec4Parameter == customVec4Parameter)
        return;
    m_customVec4Parameter = customVec4Parameter;
    emit customVec4ParameterChanged(customVec4Parameter);
}

void MyTestCustomShaderData::setCustomVec3Parameter(const QVector3D &customVec3Parameter)
{
    if (m_customVec3Parameter == customVec3Parameter)
        return;
    m_customVec3Parameter = customVec3Parameter;
    emit customVec3ParameterChanged(customVec3Parameter);
}

void MyTestCustomShaderData::setCustomVec2Parameter(const QVector2D &customVec2Parameter)
{
    if (m_customVec2Parameter == customVec2Parameter)
        return;
    m_customVec2Parameter = customVec2Parameter;
    emit customVec2ParameterChanged(customVec2Parameter);
}

void MyTestCustomShaderData::setCustomFloatParameter(float customFloatParameter)
{
    if (m_customFloatParameter == customFloatParameter)
        return;
    m_customFloatParameter = customFloatParameter;
    emit customFloatParameterChanged(customFloatParameter);
}

} // namespace Kuesa

QT_END_NAMESPACE
