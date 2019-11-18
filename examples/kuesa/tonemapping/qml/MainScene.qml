/*
    SceneEntity.qml

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Robert Brock <robert.brock@kdab.com>

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

//! [0]
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Extras 2.12
import QtQuick 2.12

import Kuesa 1.1 as Kuesa
import Kuesa.Effects 1.1 as KuesaFX
import KuesaDemo 1.0 as KuesaDemo

Kuesa.SceneEntity {
    id: root3D

    property string toneMappingAlgorithmName: "None"
//! [0]
    property int screenWidth
    property int screenHeight

    property double exposure: 1.7
    property bool rotating: false
    property bool lightRotating: true

    property var __winSize: Qt.size(_view.width, _view.height)

    Kuesa.Skybox {
        // Optional: Use the irradiance instead of the radiance for a simple blurry background
        baseName: _assetsPrefix + "pink_sunrise_16f_radiance"
        extension: ".dds"
    }

    components: [
        RenderSettings {
            //! [2.2]
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: cameraAsset.node ? cameraAsset.node : fallbackCamera
                clearColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
                exposure: root3D.exposure
                toneMappingAlgorithm: {
                    if (toneMappingAlgorithmName == "Filmic")
                        return KuesaFX.ToneMappingAndGammaCorrectionEffect.Filmic
                    if (toneMappingAlgorithmName == "Reinhard")
                        return KuesaFX.ToneMappingAndGammaCorrectionEffect.Reinhard
                    if (toneMappingAlgorithmName == "Uncharted")
                        return KuesaFX.ToneMappingAndGammaCorrectionEffect.Uncharted
                    return KuesaFX.ToneMappingAndGammaCorrectionEffect.None
                }
            }
            //! [2.2]
        },
        InputSettings { },
        EnvironmentLight {
            irradiance: TextureLoader {
                source: _assetsPrefix + "pink_sunrise_16f_irradiance" + ((!root3D.es2) ? ".dds" : "_es2.dds")

                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: _assetsPrefix + "pink_sunrise_16f_specular" + ((!root3D.es2) ? ".dds" : "_es2.dds")

                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
        }
    ]

    Entity {
        Transform {
            id: transform
            property real angle: 0.0

            readonly property real radianAngle: angle * 2 * Math.PI / 360
            readonly property real radius: 10
            readonly property real x: radius * Math.cos(radianAngle)
            readonly property real y: 0
            readonly property real z: radius * Math.sin(radianAngle)

            scale: 10
            translation: Qt.vector3d(x, y, z)

            NumberAnimation on angle {
                loops: Animation.Infinite
                duration: 10000
                from: 0
                to: 360
                paused: root3D.lightRotating
            }
        }

        Kuesa.PointLight {
            id: pointLight
            color: "white"
            intensity: 0.2
        }
        components: [ pointLight, transform ]
    }

    Camera {
        id: fallbackCamera
        position: Qt.vector3d(0.0, 0.0, 7.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        exposure: root3D.exposure
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        aspectRatio: root3D.__winSize.width / root3D.__winSize.height
    }

    KuesaDemo.OrbitCameraController {
        id: controller
        camera: frameGraph.camera
        windowSize: root3D.__winSize
    }

    Kuesa.AnimationPlayer {
        id: helmetSpin
        sceneEntity: root3D
        clip: "damagedHelmet_Action"
        loops: Kuesa.AnimationPlayer.Infinite
        running: root3D.rotating
    }

    Kuesa.Asset {
        id: cameraAsset
        name: root3D.cameras.names.length ? root3D.cameras.names[0] : ""
        collection: root3D.cameras
        onNodeChanged: {
            if (node)
                node.aspectRatio = root3D.__winSize.width / root3D.__winSize.height
        }
    }

//! [2.1]
    Kuesa.Asset {
        id: materialsAsset
        name: "Material_MR"
        collection: root3D.materials
    }
//! [2.1]

//! [1]
    Kuesa.GLTF2Importer {
        id: gltf2importer
        sceneEntity: root3D
        assignNames: true
        source: _assetsPrefix + "RotatingHelmet.gltf"
        options.generateTangents: true
    }
//! [1]

}
