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

import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Extras 2.12
import Qt3D.Animation 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Scene3D 2.0
import Kuesa 1.2 as Kuesa
import Kuesa.Effects 1.1 as Effects

ApplicationWindow {
    visible: true
    width: 1024
    height: 768

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            Label {
                text: "Layer 1"
            }
            Switch {
                id: layer1Switch
                checked: false
            }
            Item {
                Layout.fillWidth: true
            }
            Label {
                text: "Layer 2"
            }
            Switch {
                id: layer2Switch
                checked: false
            }
            Item {
                Layout.fillWidth: true
            }
            Label {
                text: "Layer 3"
            }
            Switch {
                id: layer3Switch
                checked: false
            }
        }
    }

    Scene3D {
        anchors.fill: parent
        aspects: ["input", "render", "logic"]
        multisample: true

        Kuesa.SceneEntity {
            id: scene

            components: [
                RenderSettings {
                    activeFrameGraph: Kuesa.ForwardRenderer {
                        id: frameGraph
                        camera: mainCamera
                        //                        showDebugOverlay: true
                        clearColor: "white"
                        layers: {
                            var layers = []
                            if (layer1Switch.checked)
                                layers.push(layer1Asset.node)
                            if (layer2Switch.checked)
                                layers.push(layer2Asset.node)
                            if (layer3Switch.checked)
                                layers.push(layer3Asset.node)
                            return layers;
                        }
                    }
                },
                InputSettings { },
                EnvironmentLight {
                    irradiance: TextureLoader {
                        source: "file:///" + ASSETS + "../examples/kuesa/assets/envmaps/neuer_zollhof/neuer_zollhof_16f_irradiance.dds"
                        wrapMode {
                            x: WrapMode.ClampToEdge
                            y: WrapMode.ClampToEdge
                        }
                        generateMipMaps: false
                    }
                    specular: TextureLoader {
                        source: "file:///" + ASSETS + "../examples/kuesa/assets/envmaps/neuer_zollhof/neuer_zollhof_16f_specular.dds"
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
                position: Qt.vector3d(50.0, 0.0, 0.0)
                upVector: Qt.vector3d(0.0, 1.0, 0.0)
                viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
            }

            OrbitCameraController {
                id: controller
                camera: frameGraph.camera
                linearSpeed: 5
                lookSpeed: 180
            }

            Kuesa.Asset {
                id: layer1Asset
                collection: scene.layers
                name: "Layer1"
            }

            Kuesa.Asset {
                id: layer2Asset
                collection: scene.layers
                name: "Layer2"
            }

            Kuesa.Asset {
                id: layer3Asset
                collection: scene.layers
                name: "Layer3"
            }

            Kuesa.GLTF2Importer {
                id: gltf2importer
                sceneEntity: scene
                source: "file:///" + ASSETS + "manual/assets/FrameGraph/LayeredRendering/layered.gltf"
            }

            onLoadingDone: {
                mainCamera.viewAll()
            }
        }
    }
}
