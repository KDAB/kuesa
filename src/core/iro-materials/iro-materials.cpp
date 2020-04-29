/*
    iro-materials.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "iro-materials_p.h"
#include <Kuesa/GLTF2Importer>
#include <Kuesa/IroDiffuseMaterial>
#include <Kuesa/IroDiffuseEffect>
#include <Kuesa/IroDiffuseProperties>
#include <Kuesa/IroDiffuseHemiMaterial>
#include <Kuesa/IroDiffuseHemiEffect>
#include <Kuesa/IroDiffuseHemiProperties>
#include <Kuesa/IroGlassAddMaterial>
#include <Kuesa/IroGlassAddEffect>
#include <Kuesa/IroGlassAddProperties>
#include <Kuesa/IroMatteOpaqueMaterial>
#include <Kuesa/IroMatteOpaqueEffect>
#include <Kuesa/IroMatteOpaqueProperties>
#include <Kuesa/IroMatteAlphaMaterial>
#include <Kuesa/IroMatteAlphaEffect>
#include <Kuesa/IroMatteAlphaProperties>
#include <Kuesa/IroMatteSkyboxMaterial>
#include <Kuesa/IroMatteSkyboxEffect>
#include <Kuesa/IroMatteSkyboxProperties>
#include <Kuesa/IroMatteBackgroundMaterial>
#include <Kuesa/IroMatteBackgroundEffect>
#include <Kuesa/IroMatteBackgroundProperties>
#include <Kuesa/IroMatteAddMaterial>
#include <Kuesa/IroMatteAddEffect>
#include <Kuesa/IroMatteAddProperties>
#include <Kuesa/IroMatteMultMaterial>
#include <Kuesa/IroMatteMultEffect>
#include <Kuesa/IroMatteMultProperties>
#include <Kuesa/IroDiffuseAlphaMaterial>
#include <Kuesa/IroDiffuseAlphaEffect>
#include <Kuesa/IroDiffuseAlphaProperties>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace IroMaterials {

void registerIroMaterials()
{
    GLTF2Importer::registerCustomMaterial<IroDiffuseMaterial, IroDiffuseProperties, IroDiffuseEffect>(QStringLiteral("IroDiffuse"));
    GLTF2Importer::registerCustomMaterial<IroDiffuseHemiMaterial, IroDiffuseHemiProperties, IroDiffuseHemiEffect>(QStringLiteral("IroDiffuseHemi"));
    GLTF2Importer::registerCustomMaterial<IroGlassAddMaterial, IroGlassAddProperties, IroGlassAddEffect>(QStringLiteral("IroGlassAdd"));
    GLTF2Importer::registerCustomMaterial<IroMatteOpaqueMaterial, IroMatteOpaqueProperties, IroMatteOpaqueEffect>(QStringLiteral("IroMatteOpaque"));
    GLTF2Importer::registerCustomMaterial<IroMatteAlphaMaterial, IroMatteAlphaProperties, IroMatteAlphaEffect>(QStringLiteral("IroMatteAlpha"));
    GLTF2Importer::registerCustomMaterial<IroMatteSkyboxMaterial, IroMatteSkyboxProperties, IroMatteSkyboxEffect>(QStringLiteral("IroMatteSkybox"));
    GLTF2Importer::registerCustomMaterial<IroMatteBackgroundMaterial, IroMatteBackgroundProperties, IroMatteBackgroundEffect>(QStringLiteral("IroMatteBackground"));
    GLTF2Importer::registerCustomMaterial<IroMatteAddMaterial, IroMatteAddProperties, IroMatteAddEffect>(QStringLiteral("IroMatteAdd"));
    GLTF2Importer::registerCustomMaterial<IroMatteMultMaterial, IroMatteMultProperties, IroMatteMultEffect>(QStringLiteral("IroMatteMult"));
    GLTF2Importer::registerCustomMaterial<IroDiffuseAlphaMaterial, IroDiffuseAlphaProperties, IroDiffuseAlphaEffect>(QStringLiteral("IroDiffuseAlpha"));
}

// This will call registerIroMaterials on startup automatically
Q_CONSTRUCTOR_FUNCTION(registerIroMaterials)

} // namespace SimpleMaterials

} // namespace Kuesa

QT_END_NAMESPACE
