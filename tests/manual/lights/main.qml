/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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
import Kuesa 1.1 as Kuesa
import Kuesa.Utils 1.3 as KuesaUtils

Kuesa.SceneEntity {
    id: scene

    property int lightType: 0;
    property real lightRotation: 0;
    readonly property real lightDist: 2;
    readonly property vector3d lightPos: Qt.vector3d( lightDist * Math.sin(lightRotation * Math.PI/180), lightDist, lightDist * Math.cos(lightRotation * Math.PI/180))

    Entity {
        components: [
            Transform {
                translation: lightPos
            },
            Kuesa.PointLight{
                id: pointLight
                color: "#ffffff"
                intensity: 5;
                enabled: lightType == 0
            }
        ]
    }
    Entity {
        components: [
            Transform {
                rotationY: lightRotation
            },
            Kuesa.DirectionalLight{
                id: directionalLight
                color: "#ffffff"
                intensity: 5;
                enabled: lightType == 1
                direction: lightPos.times(-1);
            }
        ]
    }
    Entity {
        components: [
            Transform {
                translation: lightPos
            },
            Kuesa.SpotLight{
                id: spotLight
                color: "#ffffff"
                intensity: 5;
                enabled: lightType == 2
                outerConeAngle: 5
                localDirection:  lightPos.times(-1);
            }
        ]
    }

    components: [
        RenderSettings {
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: mainCamera
                clearColor: "black"
            }
        },
        InputSettings { },
        EnvironmentLight {
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
    ]

    QQ2.RotationAnimation on lightRotation {
        id: lightAnimation
        loops: QQ2.Animation.Infinite
        from: 0
        to: 360
        running: true
        duration: 6000
    }


    Camera {
        id: mainCamera
        position: Qt.vector3d(0.0, 0.0, 7.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    KuesaUtils.OrbitCameraController {
        id: controller
        camera: frameGraph.camera
        windowSize: _winSize
    }

    Kuesa.GLTF2Importer {
        id: gltf2importer
        sceneEntity: scene
        source: "file:///" + ASSETS + "manual/assets/Duck/glTF/Duck.gltf"
    }

    onLoadingDone: {
        mainCamera.viewAll()
    }

    KeyboardDevice { id: kb }
    KeyboardHandler {
        sourceDevice: kb
        focus: true
        onSpacePressed: lightAnimation.running = !lightAnimation.running

        onDigit1Pressed: {
            scene.lightType = 0;
        }
        onDigit2Pressed: {
            scene.lightType = 1;
        }
        onDigit3Pressed: {
            scene.lightType = 2;
        }
        onDigit4Pressed: {
            scene.lightType = 4;
        }
    }
}
