/*
    MainScene.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
import Qt3D.Render 2.11
import Qt3D.Input 2.0
import Qt3D.Extras 2.11
import Qt3D.Animation 2.10
import QtQuick 2.11 as QQ2

//! [0]
import Kuesa 1.2 as Kuesa
import Kuesa.Effects 1.1 as Effects


Kuesa.SceneEntity {
    id: scene
//! [0]

    property int screenWidth
    property int screenHeight

    property bool animated: false
    property bool explodedView
    property int carSpeed
    property bool openLeftDoor
    property bool openRightDoor
    property bool openHood
    property bool enableClipping
    property string environmentMap: "pink_sunrise"
    property double environmentExposure: 0.0
    property alias showSkybox: skybox.enabled
    property double exposure: 0.0
    property bool useOpacityMask: false
    property bool useBloomEffect: false
    property color carBaseColorFactor: "black"
    property bool es2: _isES2
    property alias bloomEffect: bloomFx

    Entity {
        id: d
        property Camera sweepCam: null

        function flipAnimation(open, animation) {
            if (open) {
                animation.clock.playbackRate = 1
                if (animation.normalizedTime > 0)
                    animation.normalizedTime = 1 - animation.normalizedTime
                animation.start()
            } else {
                animation.clock.playbackRate = -1
                if (animation.normalizedTime > 0 && animation.normalizedTime < 1)
                    animation.normalizedTime = 1 - animation.normalizedTime
                animation.start()
            }
        }
    }

//![2.3]
    // We create a baseColorFactor property. If the property exists in the node, it will be a proxy of the node property
    // When we set the qml property, the node property will be updated accordingly
    // When the node property is updated in C++, the qml property will be updated
    // As soon as the node is changed, the qml property is updated if it has a C++ equivalent
    Kuesa.Asset {
        property color baseColorFactor: scene.carBaseColorFactor
        id: carMaterial
        collection: scene.materials
        name: "Mat_CarPaint"
        onBaseColorFactorChanged: console.log("Binding 1 works. Color: " + baseColorFactor)
        onNodeChanged: scene.carBaseColorFactor = carMaterial.baseColorFactor
    }
//![2.3]

//![2.4]
    property var baseColorFactorProp: carMaterial.baseColorFactor
    onBaseColorFactorPropChanged: console.log("Binding 2 works. Color: " + baseColorFactorProp)
//![2.4]

//! [4.1]
    Kuesa.AnimationPlayer {
        id: hoodAnimator
        sceneEntity: scene
        clock: Clock { }
        clip: "HoodAction"
    }

    Kuesa.AnimationPlayer {
        id: leftDoorAnimator
        sceneEntity: scene
        clock: Clock { }
        clip: "DoorLAction"
    }

    Kuesa.AnimationPlayer {
        id: rightDoorAnimator
        sceneEntity: scene
        clock: Clock { }
        clip: "DoorRAction"
    }
//! [4.1]

    Kuesa.AnimationPlayer {
        id: sweepCamCenterAnimation
        sceneEntity: scene
        clip: "SweepCamCenterAction"
        loops: Kuesa.AnimationPlayer.Infinite
        running: scene.animated
    }
    Kuesa.AnimationPlayer {
        id: sweepCamPitchAnimation
        sceneEntity: scene
        clip: "SweepCamPitchAction"
        loops: Kuesa.AnimationPlayer.Infinite
        running: scene.animated
    }

    NodeInstantiator {
        id: wheelAnimators
        property var clock: Clock { playbackRate: scene.carSpeed / 2 }

        model: [ "WheelFLDriveAction", "WheelFRDriveAction", "WheelBLDriveAction", "WheelBRDriveAction" ]

        delegate: Kuesa.AnimationPlayer {
            sceneEntity: scene
            clip: modelData
            clock: wheelAnimators.clock
            running: scene.carSpeed > 0
            loops: Kuesa.AnimationPlayer.Infinite
        }
    }

//! [2.1]
    Kuesa.Asset {
        id: sweepCam
        collection: scene.cameras
        name: "SweepCam"
    }
//! [2.1]

//! [2.2]
    QQ2.Binding {
        target: sweepCam.node
        property: "aspectRatio"
        value: mainCamera.aspectRatio
    }
//! [2.2]

    onOpenHoodChanged: {
        d.flipAnimation(openHood, hoodAnimator)
    }

    onOpenLeftDoorChanged: {
        d.flipAnimation(openLeftDoor, leftDoorAnimator)
    }

    onOpenRightDoorChanged: {
        d.flipAnimation(openRightDoor, rightDoorAnimator)
    }

    // let this point light wander around with the camera to create some shiny lighting
    Entity {
        id: pointLightEntity
        parent: frameGraph.camera
        components: [
            Kuesa.PointLight {
            }
        ]
    }

    components: [
//! [3.2]
        RenderSettings {
            // FrameGraph
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: scene.animated && sweepCam.node ? sweepCam.node : mainCamera
                postProcessingEffects: {
                    var effects = []
                    if (useBloomEffect)
                        effects.push(bloomFx)
                    if (useOpacityMask)
                        effects.push(opacityMaskEffect)
                    return effects
                }
                backToFrontSorting: true
                toneMappingAlgorithm: Effects.ToneMappingAndGammaCorrectionEffect.Reinhard
//                showDebugOverlay: true
            }
        },
//! [3.2]
        InputSettings { },
        EnvironmentLight {
            irradiance: TextureLoader {
                source: _assetsPrefix + environmentMap + "_16f_irradiance" + ((!scene.es2) ? ".dds" : "_es2.dds")
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: _assetsPrefix + environmentMap + "_16f_specular" + ((!scene.es2) ? ".dds" : "_es2.dds")
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
        }
    ]

//! [3.1]
    Effects.OpacityMask {
        id: opacityMaskEffect
        mask: TextureLoader {
            source: "qrc:/opacity_mask.png";
            generateMipMaps: false
        }
        premultipliedAlpha: true // This is what Scene3D/QtQuick expects
    }

    Effects.BloomEffect {
        id: bloomFx
        threshold: 0.34
        blurPassCount: 2
    }
//! [3.1]

    QQ2.Binding {
        target: frameGraph
        property: "exposure"
        value: scene.exposure + scene.environmentExposure
    }

    Camera {
        id: mainCamera
        fieldOfView: scene.explodedView ? 55 : 35
        position: Qt.vector3d(4.5, 1.5, 4.5)
        upVector: Qt.vector3d(0, 1, 0)
        viewCenter: Qt.vector3d(0, .5, 0)

        QQ2.Behavior on fieldOfView { QQ2.NumberAnimation { duration: 750; easing.type: QQ2.Easing.OutQuad } }
    }

    CarCameraController {
        camera: mainCamera
        enabled: !scene.animated
    }

//! [1]
    // Loads GLTF 2.0 asset
    Kuesa.GLTF2Importer {
        sceneEntity: scene
        source: _assetsPrefix + "DodgeViper" + _modelSuffix + ".gltf"
    }
//! [1]

    Kuesa.Skybox {
        id: skybox
        baseName: _assetsPrefix + environmentMap + "_skybox"
        extension: ".dds"
    }
}
