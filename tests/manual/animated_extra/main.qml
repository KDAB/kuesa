/*
    main.qml

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

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Scene3D 2.15
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15
import Kuesa 1.3 as Kuesa

Scene3D {
    id: root
    focus: true
    aspects: ["input", "logic", "animation"]

    Kuesa.SceneEntity {
        id: scene

        components: [
            RenderSettings {
                activeFrameGraph: Kuesa.ForwardRenderer {
                    id: frameGraph
                    clearColor: "white"
                    showDebugOverlay: true
                    camera: Camera {
                        id: sceneCamera
                        position: Qt.vector3d(0, 0, 10)
                        viewCenter: Qt.vector3d(0, 0, 0)
                    }
                }
            },
            InputSettings { }
        ]

        Kuesa.GLTF2Importer {
            id: gltf2importer
            source: "qrc:/anim_extra.gltf"
        }

        Kuesa.AnimationPlayer {
            id: player
            clip: "CubeAction"
            running: true
            loops: Kuesa.AnimationPlayer.Infinite
        }

        // Retrieve Node with a custom extra property
        Kuesa.Asset {
            id: cubeAsset
            collection: scene.entities
            name: "Cube"
            property real prop // Maps to an extra property of the Cube node
        }

        Kuesa.TransformTracker {
            name: "Cube"
            camera: sceneCamera
            screenSize: Qt.size(root.width, root.height)
            viewportRect: Qt.rect(0, 0, 1, 1)
            scale: cubeAsset.prop // Bind scale to animated extra property
        }

        OrbitCameraController {
            id: controller
            camera: sceneCamera
            linearSpeed: 1
            lookSpeed: 180
        }
    }
}
