/*
    main.qml

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

import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Extras 2.10
import Qt3D.Animation 2.10
import QtQuick 2.10 as QQ2
import Kuesa 1.3 as Kuesa

KuesaUtils.View3D {
    id: scene
    camera: "Camera_Orientation"
    source: "file:///" + ASSETS + "manual/assets/Rock/rock.gltf"
    showDebugOverlay: true
    anchors.fill: parent

    Entity {
        id: environmentLight
        components: EnvironmentLight { // For PBR reflections
            id: envLight

            irradiance: TextureLoader {
                source: "qrc:/wobbly_bridge_16f_irradiance.dds"
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: "qrc:/wobbly_bridge_16f_specular.dds"
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
        }
    }

    OrbitCameraController {
        id: controller
        camera: scene.activeCamera
        linearSpeed: 5
        lookSpeed: 180
    }

    Kuesa.MeshInstantiator
    {
        entityName: "rock"
        transforms: [
            Transform { translation: Qt.vector3d(5, 0, 0) },
            Transform { translation: Qt.vector3d(-5, 0, 0) },
            Transform { translation: Qt.vector3d(0, 0, 5) },
            Transform { translation: Qt.vector3d(0, 0, -5) }
        ]
    }
}
