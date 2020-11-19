/*
    unlitmaterial.h

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#ifndef KUESA_UNLITMATERIAL_H
#define KUESA_UNLITMATERIAL_H

#include <Kuesa/gltf2material.h>
#include <Kuesa/kuesa_global.h>
#include <Kuesa/UnlitEffect>
#include <Kuesa/UnlitProperties>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class KUESASHARED_EXPORT UnlitMaterial : public GLTF2Material
{
    Q_OBJECT

    Q_PROPERTY(Kuesa::UnlitProperties *materialProperties READ materialProperties WRITE setMaterialProperties NOTIFY materialPropertiesChanged)

public:
    explicit UnlitMaterial(Qt3DCore::QNode *parent = nullptr);
    ~UnlitMaterial();

    UnlitProperties *materialProperties() const;
    void setMaterialProperties(Kuesa::UnlitProperties *materialProperties);

Q_SIGNALS:
    void materialPropertiesChanged(Kuesa::UnlitProperties *properties);

private:
    UnlitEffect *m_effect;

    UnlitProperties *m_materialProperties;
    Qt3DRender::QParameter *m_unlitShaderDataParameter;
    Qt3DRender::QParameter *m_baseColorMapParameter;
    QMetaObject::Connection m_textureTransformChangedConnection;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_UNLITMATERIAL_H
