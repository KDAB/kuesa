/*
    effectproperties.h

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


#ifndef EFFECT_PROPERTIES_H
#define EFFECT_PROPERTIES_H

#include <QFlags>
#include <Kuesa/kuesa_global.h>
#include <QObject>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class KUESASHARED_EXPORT EffectProperties
{
    Q_GADGET

public:
    enum Property {
        // Material type
        MetallicRoughness = 1 << 1,
        Unlit = 1 << 2,
        Custom = 1 << 3,

        // Material properties
        BaseColorMap = 1 << 4,
        MetalRoughnessMap = 1 << 5,
        NormalMap = 1 << 6,
        AOMap = 1 << 7,
        EmissiveMap = 1 << 8,
        Blend = 1 << 10,
        Mask = 1 << 11,
        DoubleSided = 1 << 12,

        // Mesh properties
        VertexColor = 1 << 13,
        Skinning = 1 << 14,
        VertexNormal = 1 << 15,
        VertexTangent = 1 << 16,
        VertexTexCoord = 1 << 17,
        VertexTexCoord1 = 1 << 18,
        MorphTargets = 1 << 19,
    };
    Q_DECLARE_FLAGS(Properties, Property)
    Q_FLAG(Properties)
    Q_ENUM(Property)
};

} // namespace Kuesa

Q_DECLARE_OPERATORS_FOR_FLAGS(Kuesa::EffectProperties::Properties)

QT_END_NAMESPACE

#endif // EFFECT_PROPERTIES_H
