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

Kuesa.SceneEntity {
    id: scene

    components: [
        RenderSettings {
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                clearColor: "white"
                showDebugOverlay: true

                views: [
                    // Ground Plane
                    Kuesa.View {
                        camera: mainCamera.node
                        layers: planeSceneLayer.node
                    },
                    // Scene and Reflection
                    Kuesa.View {
                        camera: mainCamera.node
                        layers: mainSceneLayer.node
                        reflectionPlanes: sceneReflectionPlane.node
                    }
                ]
            }
        },
        InputSettings { },
        EnvironmentLight { // For PBR reflections
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
    ]

    Kuesa.Asset {
        id: mainCamera
        collection: scene.cameras
        name: "Camera_Orientation"
    }
    Kuesa.Asset {
        id: mainSceneLayer
        collection: scene.layers
        name: "MainScene"
    }
    Kuesa.Asset {
        id: planeSceneLayer
        collection: scene.layers
        name: "PlaneScene"
    }
    Kuesa.Asset {
        id: sceneReflectionPlane
        collection: scene.reflectionPlanes
        name: "ReflectionPlane"
    }

    OrbitCameraController {
        id: controller
        camera: mainCamera.node
        linearSpeed: 5
        lookSpeed: 180
    }
    Kuesa.GLTF2Importer {
        id: gltf2importer
        sceneEntity: scene
        source: "file:///" + ASSETS + "manual/assets/PlanarReflections/reflections.gltf"
    }
}
