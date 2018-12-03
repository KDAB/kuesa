/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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
import Kuesa 1.0 as Kuesa

Kuesa.SceneEntity {
    id: scene

    components: [
        RenderSettings {
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: mainCamera
                clearColor: "white"
            }
        },
        InputSettings { },
        DirectionalLight {
            worldDirection: frameGraph.camera.viewVector
        },
        EnvironmentLight {
            irradiance: TextureLoader {
                source: "qrc:/wobbly_bridge_irradiance.dds"
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: "qrc:/wobbly_bridge_specular.dds"
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
        }
    ]

    Camera {
        id: mainCamera
        position: Qt.vector3d(0.0, 0.0, 7.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    OrbitCameraController {
        id: controller
        camera: frameGraph.camera
        linearSpeed: 5
        lookSpeed: 180
    }

    Kuesa.GLTF2Importer {
        id: gltf2importer
        sceneEntity: scene
        source: "file:///" + ASSETS + "manual/assets/AnimatedCubes/glTF/AnimatedCubes.gltf"
    }

    Kuesa.AnimationPlayer {
        id: player
        sceneEntity: scene
        clip: "animation_AnimatedCube"
        mapper: "animation_AnimatedCube"
        running: player.status === Kuesa.AnimationPlayer.Ready
        loops: Kuesa.AnimationPlayer.Infinite
    }

    KeyboardDevice {
        id: keyboard
    }

    KeyboardHandler {
        sourceDevice: keyboard
        focus: true

        property var targets: [ "AnimatedCube", "NonAnimatedCube" ]
        property int ndx: 0

        onTabPressed: {
            ndx = (ndx + 1) % targets.length
            var t = scene.transformForEntity( targets[ndx] )
            if (t) {
                player.targets = [ t ]
                player.running = true
            }
        }
    }

    onLoadingDone: {
        mainCamera.viewAll()
    }
}
