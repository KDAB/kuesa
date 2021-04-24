/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2020-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan José Casafranca <jjcasmar@gmail.com>

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
import Kuesa 1.3 as Kuesa

import Test 1.0

Item {

    Scene3D
    {
        id: scene3D
        anchors.fill: parent
        multisample: true
        focus: true
        aspects: ["animation", "logic", "input"]

        Kuesa.SceneEntity {
            id: kuesaScene

            components: [
                RenderSettings {
                    // Configures renderer (which camera to use, which effects to apply ...)
                    activeFrameGraph: Kuesa.ForwardRenderer {
                        id: frameGraph
                        clearColor: Qt.rgba(0.01, 0.01, 0.01, 1.0);
                        camera: sceneCamera.node
                        showDebugOverlay: true
                        usesStencilMask: true
                        postProcessingEffects: [
                            StencilEffect {
                                color: "blue"
                            }
                        ]
                    }


                    renderPolicy: RenderSettings.Always
                },
                InputSettings { }
            ]

            Kuesa.Asset {
                id: cube
                sceneEntity: kuesaScene
                name: "Cube"
            }

            Kuesa.Asset {
                id: sceneCamera
                collection: kuesaScene.cameras
                name: "Camera_Orientation"
            }

            StencilHelper {
                entity: cube.node
            }

            // Load glTF2 file
            Kuesa.GLTF2Importer {
                id: gltfImporter
                sceneEntity: kuesaScene
                source: "qrc:/scene.gltf"
            }
        }
    }
}
