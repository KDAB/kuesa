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
import Kuesa 1.3 as Kuesa
import Kuesa.Utils 1.3 as KuesaUtils

Kuesa.SceneEntity {
    id: scene

    property int sourceNo: 0;
    readonly property bool bigScene: sourceNo == 3
    readonly property var sources: [
        "file:///" + ASSETS + "manual/assets/Duck/glTF/Duck.gltf",
        "file:///" + ASSETS + "auto/assets/simple_cube.gltf",
        "file:///" + ASSETS + "manual/assets/Box/glTF/Box.gltf",
        "file:///" + ASSETS + "manual/assets/AntiqueCamera/glTF-Binary/AntiqueCamera.glb",
    ]

    property real lightRotation: 0;
    readonly property real lightDist: bigScene ? 3 : 1;
    property bool softShadows: true
    property bool showEnvironmentLight: true
    property bool showShadowCoverage: false
    readonly property int numLights: 6
    property var lightArray: [pointLight, directionalLight, spotLight, pointLight2, directionalLight2, spotLight2];
    readonly property size textureSize: bigScene ? Qt.size(1024, 1024) : Qt.size(512, 512)

    function calculateLightPos(lightIndex)
    {
        var rotationAmt = lightRotation + lightIndex * 360/numLights;
        return Qt.vector3d( 1.5 * lightDist * Math.sin(rotationAmt * Math.PI/180), 2 * lightDist, .75 *lightDist * Math.cos(rotationAmt * Math.PI/180))
    }



    Entity {
        components: [
            Transform {
                translation: calculateLightPos(pointLight.lightIndex)
            },
            Kuesa.PointLight{
                id: pointLight
                readonly property int lightIndex: 0
                intensity: 5 * lightDist;
                castsShadows: true
                softShadows: scene.softShadows
                textureSize: scene.textureSize
            }
        ]
    }
    Entity {
        components: [
            Transform {
                translation: calculateLightPos(pointLight2.lightIndex)
            },
            Kuesa.PointLight{
                id: pointLight2
                readonly property int lightIndex: 3
                intensity: pointLight.intensity
                castsShadows: true
                softShadows: pointLight.softShadows
                textureSize: scene.textureSize
            }
        ]
    }


    Entity {
        components: [
            Kuesa.DirectionalLight{
                id: directionalLight
                readonly property int lightIndex: 1
                intensity: 5;
                direction: calculateLightPos(lightIndex).times(-1);
                castsShadows: true
                softShadows: scene.softShadows
                textureSize: scene.textureSize
            }
        ]
    }
    Entity {
        components: [
            Kuesa.DirectionalLight{
                id: directionalLight2
                readonly property int lightIndex: 4
                intensity: directionalLight.intensity;
                direction: calculateLightPos(lightIndex).times(-1);
                castsShadows: true
                softShadows: directionalLight.softShadows
                textureSize: scene.textureSize
            }
        ]
    }

    Entity {
        components: [
            Transform {
                translation: calculateLightPos(spotLight.lightIndex)
            },
            Kuesa.SpotLight{
                id: spotLight
                readonly property int lightIndex: 2
                intensity: 5 * lightDist;
                outerConeAngle: 45
                localDirection: calculateLightPos(lightIndex).times(-1)
                castsShadows: true
                softShadows: scene.softShadows
                textureSize: scene.textureSize
            }
        ]
    }
    Entity {
        components: [
            Transform {
                translation: calculateLightPos(spotLight2.lightIndex)
             },
            Kuesa.SpotLight{
                id: spotLight2
                readonly property int lightIndex: 5
                intensity: 5 * lightDist;
                outerConeAngle: spotLight.outerConeAngle
                localDirection: calculateLightPos(lightIndex).times(-1)
                castsShadows: true
                softShadows: spotLight.softShadows
                textureSize: scene.textureSize
            }
        ]
    }

    Entity {
        id: plane
        components: [
            PlaneMesh {
                width: bigScene ? 15 : 7
                height: bigScene ? 15 : 7
            },
            Transform {
                translation: Qt.vector3d(0, -.1, 0)
            },
            Kuesa.MetallicRoughnessMaterial {
                id: planeMat
                effect: Kuesa.MetallicRoughnessEffect {
                    usingNormalAttribute: true
                }
                materialProperties: Kuesa.MetallicRoughnessProperties {
                    id: planeMatProperties
                    baseColorFactor: Qt.rgba(1.0, 1.0, 1.0)
                    metallicFactor: .75
                    roughnessFactor: .2
                    receivesShadows: true
                }
            }
        ]
    }
    NodeInstantiator
    {
        model: lightArray.length
        delegate: Entity {
            id: lightSphere
            enabled: lightArray[model.index].enabled
            components: [
                SphereMesh{
                    radius: .1
                    slices: 10
                    rings: 10
                },
                Transform {
                    translation: calculateLightPos(model.index)
                },
                Kuesa.MetallicRoughnessMaterial {
                    id: sphereMat
                    effect: Kuesa.MetallicRoughnessEffect {
                        usingNormalAttribute: true
                    }
                    materialProperties: Kuesa.MetallicRoughnessProperties {
                        baseColorFactor: model.index % 3 == 0 ? Qt.rgba(1.0, 0.0, 0.0) : model.index % 3 == 1 ? Qt.rgba(0.0, 1.0, 0.0) : Qt.rgba(0.0, 0.0, 1.0)
                        metallicFactor: .75
                        roughnessFactor: .2
                    }
                }
            ]
        }
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
            id: environmentLight
            enabled: showEnvironmentLight
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
        duration: 12000
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
        source: scene.sources[0]
    }

    onLoadingDone: {
        for (var i = 1; i< lightArray.length; i++) {
            lightArray[i].enabled = false;
       }
        scene.lights.add("point light", pointLight);
        scene.lights.add("point light 2", pointLight2);
        scene.lights.add("directional light", directionalLight);
        scene.lights.add("directional light 2", directionalLight2);
        scene.lights.add("spotlight", spotLight);
        scene.lights.add("spotlight 2", spotLight2);
        scene.materials.add("planeMaterial", planeMatProperties);
        mainCamera.viewAll()
    }

    KeyboardDevice { id: kb }
    KeyboardHandler {
        sourceDevice: kb
        focus: true
        onSpacePressed: lightAnimation.paused = !lightAnimation.paused
        onPressed: {
            if (event.key == Qt.Key_E)
                scene.showEnvironmentLight = !showEnvironmentLight;
            else if (event.key == Qt.Key_S)
            {
                ++scene.sourceNo;
                gltf2importer.source = scene.sources[sourceNo%sources.length];
            }
            else if (event.key == Qt.Key_Plus)
                ++scene.lightDist;
            else if (event.key == Qt.Key_Minus)
                --scene.lightDist;
            else if (event.key == Qt.Key_P)
                scene.softShadows = !scene.softShadows;
            else if (event.key >= Qt.Key_1 && event.key <= Qt.Key_6)
            {
                var light = lightArray[event.key - Qt.Key_1];
                light.enabled = !light.enabled;
                console.log("light "+ light + " enabled = " + light.enabled);
            }
        }
    }
}
