
/*
    iro2planarreflectionequirectmaterial.h

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

#ifndef KUESA_IRO2PLANARREFLECTIONEQUIRECTMATERIAL_H
#define KUESA_IRO2PLANARREFLECTIONEQUIRECTMATERIAL_H

#include <Kuesa/GLTF2Material>
#include <Kuesa/kuesa_global.h>
#include <Kuesa/Iro2PlanarReflectionEquiRectProperties>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class GLTF2MaterialProperties;

class KUESASHARED_EXPORT Iro2PlanarReflectionEquiRectMaterial : public GLTF2Material
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::Iro2PlanarReflectionEquiRectProperties *materialProperties READ materialProperties WRITE setMaterialProperties NOTIFY materialPropertiesChanged)

public:
    Q_INVOKABLE explicit Iro2PlanarReflectionEquiRectMaterial(Qt3DCore::QNode *parent = nullptr);
    ~Iro2PlanarReflectionEquiRectMaterial();

    Iro2PlanarReflectionEquiRectProperties *materialProperties() const;

public Q_SLOTS:
    void setMaterialProperties(Kuesa::Iro2PlanarReflectionEquiRectProperties *materialProperties);
    void setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties);

Q_SIGNALS:
    void materialPropertiesChanged(Kuesa::Iro2PlanarReflectionEquiRectProperties *materialProperties);

private:
    Iro2PlanarReflectionEquiRectProperties *m_materialProperties = nullptr;
    Qt3DRender::QParameter *m_shaderDataParameter;
    Qt3DRender::QParameter *m_normalMapParameter;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_IRO2PLANARREFLECTIONEQUIRECTMATERIAL_H
