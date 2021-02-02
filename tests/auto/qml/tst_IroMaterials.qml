/*
    tst_IroMaterials.qml
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

import QtTest 1.15
import Kuesa.Iro 1.3 as KIro13

TestCase {
    KIro13.IroDiffuseMaterial {}
    KIro13.IroDiffuseProperties{}
    KIro13.IroDiffuseEffect{
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroDiffuseHemiMaterial {}
    KIro13.IroDiffuseHemiProperties {}
    KIro13.IroDiffuseHemiEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroGlassAddMaterial {}
    KIro13.IroGlassAddProperties {}
    KIro13.IroGlassAddEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroMatteOpaqueMaterial {}
    KIro13.IroMatteOpaqueProperties {}
    KIro13.IroMatteOpaqueEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroMatteAlphaMaterial {}
    KIro13.IroMatteAlphaProperties {}
    KIro13.IroMatteAlphaEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroMatteSkyboxMaterial {}
    KIro13.IroMatteSkyboxProperties {}
    KIro13.IroMatteSkyboxEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroMatteBackgroundMaterial {}
    KIro13.IroMatteBackgroundProperties {}
    KIro13.IroMatteBackgroundEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroMatteAddMaterial {}
    KIro13.IroMatteAddProperties {}
    KIro13.IroMatteAddEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroMatteMultMaterial {}
    KIro13.IroMatteMultProperties {}
    KIro13.IroMatteMultEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }

    KIro13.IroDiffuseAlphaMaterial {}
    KIro13.IroDiffuseAlphaProperties {}
    KIro13.IroDiffuseAlphaEffect {
        doubleSided: true
        usingSkinning: true
        opaque: true
        alphaCutoffEnabled: true
        usingColorAttribute: true
        usingNormalAttribute: true
        usingTangentAttribute: true
        usingTexCoordAttribute: true
        usingTexCoord1Attribute: true
        usingMorphTargets: true
    }
}
