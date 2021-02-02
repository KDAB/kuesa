/*
    iro-materials.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "iro2-materials_p.h"
#include <Kuesa/GLTF2Importer>

#include <Kuesa/Iro2DiffuseEquiRectMaterial>
#include <Kuesa/Iro2DiffuseEquiRectEffect>
#include <Kuesa/Iro2DiffuseEquiRectProperties>

#include <Kuesa/Iro2DiffuseSemMaterial>
#include <Kuesa/Iro2DiffuseSemEffect>
#include <Kuesa/Iro2DiffuseSemProperties>

#include <Kuesa/Iro2AlphaEquiRectMaterial>
#include <Kuesa/Iro2AlphaEquiRectEffect>
#include <Kuesa/Iro2AlphaEquiRectProperties>

#include <Kuesa/Iro2AlphaSemMaterial>
#include <Kuesa/Iro2AlphaSemEffect>
#include <Kuesa/Iro2AlphaSemProperties>

#include <Kuesa/Iro2GlassEquiRectMaterial>
#include <Kuesa/Iro2GlassEquiRectEffect>
#include <Kuesa/Iro2GlassEquiRectProperties>

#include <Kuesa/Iro2GlassSemMaterial>
#include <Kuesa/Iro2GlassSemEffect>
#include <Kuesa/Iro2GlassSemProperties>

#include <Kuesa/Iro2MatteAlphaMaterial>
#include <Kuesa/Iro2MatteAlphaEffect>
#include <Kuesa/Iro2MatteAlphaProperties>

#include <Kuesa/Iro2PlanarReflectionEquiRectMaterial>
#include <Kuesa/Iro2PlanarReflectionEquiRectEffect>
#include <Kuesa/Iro2PlanarReflectionEquiRectProperties>

#include <Kuesa/Iro2PlanarReflectionSemMaterial>
#include <Kuesa/Iro2PlanarReflectionSemEffect>
#include <Kuesa/Iro2PlanarReflectionSemProperties>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace Iro2Materials {

void registerIro2Materials()
{
    GLTF2Importer::registerCustomMaterial<Iro2DiffuseEquiRectMaterial, Iro2DiffuseEquiRectProperties, Iro2DiffuseEquiRectEffect>(QStringLiteral("Iro2DiffuseEquiRect"));
    GLTF2Importer::registerCustomMaterial<Iro2DiffuseSemMaterial, Iro2DiffuseSemProperties, Iro2DiffuseSemEffect>(QStringLiteral("Iro2DiffuseSem"));

    GLTF2Importer::registerCustomMaterial<Iro2AlphaEquiRectMaterial, Iro2AlphaEquiRectProperties, Iro2AlphaEquiRectEffect>(QStringLiteral("Iro2AlphaEquiRect"));
    GLTF2Importer::registerCustomMaterial<Iro2AlphaSemMaterial, Iro2AlphaSemProperties, Iro2AlphaSemEffect>(QStringLiteral("Iro2AlphaSem"));

    GLTF2Importer::registerCustomMaterial<Iro2GlassEquiRectMaterial, Iro2GlassEquiRectProperties, Iro2GlassEquiRectEffect>(QStringLiteral("Iro2GlassEquiRect"));
    GLTF2Importer::registerCustomMaterial<Iro2GlassSemMaterial, Iro2GlassSemProperties, Iro2GlassSemEffect>(QStringLiteral("Iro2GlassSem"));

    GLTF2Importer::registerCustomMaterial<Iro2MatteAlphaMaterial, Iro2MatteAlphaProperties, Iro2MatteAlphaEffect>(QStringLiteral("Iro2MatteAlpha"));

    GLTF2Importer::registerCustomMaterial<Iro2PlanarReflectionEquiRectMaterial, Iro2PlanarReflectionEquiRectProperties, Iro2PlanarReflectionEquiRectEffect>(QStringLiteral("Iro2PlanarReflectionEquiRect"));
    GLTF2Importer::registerCustomMaterial<Iro2PlanarReflectionSemMaterial, Iro2PlanarReflectionSemProperties, Iro2PlanarReflectionSemEffect>(QStringLiteral("Iro2PlanarReflectionSem"));
}

// This will call registerIroMaterials on startup automatically
Q_CONSTRUCTOR_FUNCTION(registerIro2Materials)

} // namespace SimpleMaterials

} // namespace Kuesa

QT_END_NAMESPACE
