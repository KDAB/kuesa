/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import Qt3D.Core 2.11
import Qt3D.Render 2.11
import Qt3D.Input 2.1
import Qt3D.Extras 2.10
import QtQuick 2.10 as QQ2
import Kuesa 1.1 as Kuesa
import GltfEditor 1.0 as GltfEditor

Kuesa.SceneEntity {
    id: scene

    QQ2.Component.onCompleted: _mainWindow.setup(mainCamera, gltf2PlaceHolderEntity)
    readonly property Layer texturePreviewLayer: Layer {}
    readonly property Layer materialPreviewLayer: Layer {}

    // let this point light wander around with the camera to create some shiny lighting
    Entity {
        parent: frameGraph.camera
        components: [
            Kuesa.PointLight {}
        ]
    }

    components: [
        RenderSettings {
            activeFrameGraph: frameGraphNode
            pickingSettings.pickMethod: PickingSettings.TrianglePicking
        },
        InputSettings { },
        EnvironmentLight {
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
        },
        ObjectPicker {
            property point __downPos
            onPressed: __downPos = pick.position
            onClicked: {
                if (pick.position !== __downPos)
                    return
                if (pick.modifiers & Qt.ControlModifier)
                    mainCamera.viewCenter = pick.worldIntersection
                else
                    _mainWindow.pickEntity(pick)
            }
        }
    ]

    FrameGraphNode {
        id: frameGraphNode
        // Render GLTF Scene in main viewport
        LayerFilter {
            layers: [texturePreviewLayer, materialPreviewLayer]
            filterMode: LayerFilter.DiscardAnyMatchingLayers

            Kuesa.ForwardRenderer {
                id: frameGraph
                camera: _mainWindow.activeCamera >= 0 && _mainWindow.activeCamera < scene.cameras.names.length ?
                            scene.camera(scene.cameras.names[_mainWindow.activeCamera]) : mainCamera
                frustumCulling: true
                clearColor: _mainWindow.clearColor
                toneMappingAlgorithm: _mainWindow.toneMappingAlgorithm
                gamma: _mainWindow.gamma
                exposure: _mainWindow.exposure
            }
        }
        // Render texture/material previews and capture
        RenderPreviewGraph {
            id: texturePreviewGraph
            layers: texturePreviewLayer
            surface: frameGraph.renderSurface
            previewSize: _textureCaptureReceiver.previewSize
            receiver: _textureCaptureReceiver
        }
        RenderPreviewGraph {
            id: materialPreviewGraph
            layers: materialPreviewLayer
            surface: frameGraph.renderSurface
            previewSize: _materialCaptureReceiver.previewSize
            receiver: _materialCaptureReceiver
        }

        // TexturePreview
        Entity {
            readonly property PlaneMesh mesh: PlaneMesh {
                width: 2.0
                height: 2.0
                meshResolution: Qt.size(2, 2)
            }
            readonly property Transform transform: Transform {
                rotation: fromAxisAndAngle(Qt.vector3d(1.0, 0.0, 0.0), 90.0)
            }
            readonly property Material material: GltfEditor.TexturePreviewMaterial {
                texture: _textureInspector.texture
                onTextureChanged: texturePreviewGraph.requestCapture()
            }
            components: [mesh, transform, texturePreviewLayer, material]
        }

        // MaterialPreview
        Entity {
            readonly property Material previewMaterial : _materialInspector.material
            readonly property var previewMaterialProperties : _materialInspector.materialProperties

            onPreviewMaterialPropertiesChanged: materialPreviewGraph.requestCapture()

            readonly property SphereMesh sphereMesh: SphereMesh {
                radius: .8
                rings: 30
                slices: 30
            }
            readonly property PlaneMesh planeMesh: PlaneMesh {
                width: 2.0
                height: 2.0
                meshResolution: Qt.size(2, 2)
            }
            readonly property var materialMesh : _materialInspector.hasTextures ? planeMesh : sphereMesh

            readonly property Transform materialTransform: Transform {
                rotation: fromAxisAndAngle(Qt.vector3d(1.0, .0, 0.0), 90.0)
            }
            readonly property Kuesa.DirectionalLight materialLight: Kuesa.DirectionalLight {
                direction: Qt.vector3d(0, 0, -1)
                intensity: 1.0
            }

            components: [materialMesh, materialTransform, materialPreviewLayer, previewMaterial, materialLight]
        }
    }

    Camera {
        id: mainCamera
        position: Qt.vector3d(0.0, 0.0, 7.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    GltfEditor.OrbitCameraController {
        camera: mainCamera
        enabled: _mainWindow.activeCamera === -1
        windowSize: _mainWindow.renderAreaSize
    }

    Entity {
        id: gltf2PlaceHolderEntity
        Kuesa.GLTF2Importer {
            id: gltf2importer
            source: _mainWindow.filePath
            sceneEntity: scene
            assignNames: true
            options.generateTangents:  _mainWindow.generateRuntimeTangents
            options.generateNormals: _mainWindow.generateRuntimeNormals
            onStatusChanged: _mainWindow.updateScene(scene)
        }
    }
}
