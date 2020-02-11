
/*
    mytestcustomproperties.h

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

#ifndef KUESA_MYTESTCUSTOMPROPERTIES_H
#define KUESA_MYTESTCUSTOMPROPERTIES_H

#include <QVector2D>
#include <QVector4D>
#include <QVector3D>
#include <Kuesa/GLTF2MaterialProperties>
#include <Kuesa/kuesa_global.h>


QT_BEGIN_NAMESPACE

namespace Kuesa {

class MyTestCustomShaderData;

class MyTestCustomProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
    Q_PROPERTY(QVector4D customVec4Parameter READ customVec4Parameter WRITE setCustomVec4Parameter NOTIFY customVec4ParameterChanged)
    Q_PROPERTY(QVector3D customVec3Parameter READ customVec3Parameter WRITE setCustomVec3Parameter NOTIFY customVec3ParameterChanged)
    Q_PROPERTY(QVector2D customVec2Parameter READ customVec2Parameter WRITE setCustomVec2Parameter NOTIFY customVec2ParameterChanged)
    Q_PROPERTY(float customFloatParameter READ customFloatParameter WRITE setCustomFloatParameter NOTIFY customFloatParameterChanged)

public:
    Q_INVOKABLE explicit MyTestCustomProperties(Qt3DCore::QNode *parent = nullptr);
    ~MyTestCustomProperties();

    Qt3DRender::QShaderData *shaderData() const override;
    QVector4D customVec4Parameter() const;
    QVector3D customVec3Parameter() const;
    QVector2D customVec2Parameter() const;
    float customFloatParameter() const;

public Q_SLOTS:
    void setCustomVec4Parameter(const QVector4D &customVec4Parameter);
    void setCustomVec3Parameter(const QVector3D &customVec3Parameter);
    void setCustomVec2Parameter(const QVector2D &customVec2Parameter);
    void setCustomFloatParameter(float customFloatParameter);

Q_SIGNALS:
    void customVec4ParameterChanged(QVector4D);
    void customVec3ParameterChanged(QVector3D);
    void customVec2ParameterChanged(QVector2D);
    void customFloatParameterChanged(float);

private:
    MyTestCustomShaderData *m_shaderData;
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_MYTESTCUSTOMPROPERTIES_H
