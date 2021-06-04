/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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
    focus: true
    aspects: ["input", "logic"]

    property Entity nodeWithExtras: null
    readonly property real intermediateProp: nodeWithExtras ? nodeWithExtras.extraProp : 0

    onIntermediatePropChanged: {
        console.log("Intermediate Property Changed to: " + intermediateProp + ", bindings on dynamic properties are working")
    }

    Kuesa.SceneEntity {
        id: scene

        components: [
            RenderSettings {
                activeFrameGraph: Kuesa.ForwardRenderer {
                    id: frameGraph
                    clearColor: "white"
                    showDebugOverlay: true
                    camera: mainCamera.node
                }
            },
            InputSettings { }
        ]

        Kuesa.GLTF2Importer {
            id: gltf2importer
            source: "qrc:/extras.gltf"
        }

        onLoadingDone: {
            nodeWithExtras = entity("NodeWithExtras")
        }

        Connections {
            target: nodeWithExtras
            function onExtraPropChanged() { console.log("extraPropChanged fired, dynamic property signals are working") }
            ignoreUnknownSignals: true
        }

        // Simulate extra property being animated
        Timer {
            running: true
            repeat: true
            interval: 200
            onTriggered: {
                if (nodeWithExtras === null)
                    return;
                nodeWithExtras.extraProp += 1
            }
        }

        // Also try to retrieve and watch properties with a Kuesa Asset
        Kuesa.Asset {
            collection: scene.entities
            name: "NodeWithExtras"
            property real extraProp

            onExtraPropChanged: console.log("Asset property forwarding of dynamic property " + extraProp)
        }

        Kuesa.Asset {
            id: mainCamera
            collection: scene.cameras
            name: "Camera_Orientation"
        }

        OrbitCameraController {
            id: controller
            camera: mainCamera.node
            linearSpeed: 5
            lookSpeed: 180
        }
    }
}
