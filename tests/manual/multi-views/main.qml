/*
    main.qml

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
                    Kuesa.View {
                        viewportRect: Qt.rect(0, 0, 0.5, 0.5)
                        camera: mainCamera.node
                        layers: [mainLayer.node]
                        reflectionPlanes: [
                            Kuesa.ReflectionPlane {
                                equation: Qt.vector4d(0.0, 1.0, 0.0, 0.0)
                                layers: [mainLayer.node]
                            }
                        ]
                    },
                    Kuesa.View {
                        viewportRect: Qt.rect(0.5, 0, 0.5, 0.5)
                        camera: coneCamera.node
                        layers: [coneLayer.node]
                    },
                    Kuesa.View {
                        viewportRect: Qt.rect(0.5, 0.5, 0.5, 0.5)
                        camera: mainCamera.node
                        layers: [mainLayer.node, coneLayer.node]

                        reflectionPlanes: [
                            Kuesa.ReflectionPlane {
                                equation: Qt.vector4d(0.0, 1.0, 0.0, 0.0)
                                layers: [coneLayer.node]
                            }
                        ]
                    }
                ]
            }
        },
        InputSettings { }
    ]

    Kuesa.Asset {
        id: mainCamera
        collection: scene.cameras
        name: "MainCamera_Orientation"
    }

    Kuesa.Asset {
        id: coneCamera
        collection: scene.cameras
        name: "ConeCamera_Orientation"
    }

    Kuesa.Asset {
        id: mainLayer
        collection: scene.layers
        name: "Main"
    }

    Kuesa.Asset {
        id: coneLayer
        collection: scene.layers
        name: "Cones"
    }

    Kuesa.GLTF2Importer {
        id: gltf2importer
        sceneEntity: scene
        source: "file:///" + ASSETS + "manual/assets/multi-views/multi-views.gltf"
    }
}
