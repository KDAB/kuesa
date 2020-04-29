/*
    BloomTest.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtQuick 2.0
import QtQml 2.2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.10
import Kuesa 1.1 as Kuesa

Entity {
    id: root

    readonly property int sphereCount: 16
    property int activeLight: 0
    readonly property color currentColor: Qt.hsla(activeLight/sphereCount, 1, .5, 1)

    readonly property real angle: activeLight * 2 * Math.PI / sphereCount
    readonly property real sphereDistance: 2
    readonly property vector3d activeSpherePos : Qt.vector3d(sphereDistance * Math.sin(angle), 0, sphereDistance * Math.cos(angle))
    property bool paused: false

    property real lightIntensity: 0
    readonly property real maxLightIntesity: 10

    KeyboardDevice{
        id: keyboard
    }

    Entity {
        components: [
            KeyboardHandler {
                id: handler
                sourceDevice: keyboard
                focus: true
                onSpacePressed: {
                    root.paused = !root.paused
                 }
            }
        ]
    }


    Timer {
        property real time: 0
        interval: 20
        onTriggered: {
            time += interval/1000.0;
            var period = 3.0;
            var cycle = time/period;
            activeLight = cycle % sphereCount;
            lightIntensity = maxLightIntesity * Math.abs(Math.sin(cycle *Math.PI));
        }
        running: !root.paused
        repeat: true
    }

    Camera {
        id: mainCamera
        position: Qt.vector3d(5, 4, 8)
        viewCenter: Qt.vector3d(0, 0, 0)
    }

    Entity {
        components: [
            light,
            lightTransform,
            objectsLayer
        ]
        Kuesa.PointLight { id: light; color: currentColor; intensity: lightIntensity}
        Transform { id: lightTransform; translation: activeSpherePos }
    }

    Layer {id: objectsLayer }


    OrbitCameraController { camera: mainCamera }


    Entity {
        components: [
            cube,
            cubeMat
        ]
        SphereMesh {
            id: cube
            rings: 30
            slices: 30
            radius: 1
        }

        Kuesa.MetallicRoughnessMaterial {
            id: cubeMat
            effect: Kuesa.MetallicRoughnessEffect { }
            metallicRoughnessProperties: Kuesa.MetallicRoughnessProperties {
                baseColorFactor: Qt.rgba(1.0, 1.0, 1.0)
                metallicFactor: .75
                roughnessFactor: .2
            }
        }
    }

    NodeInstantiator {
        id: sphereInstantiator
        model: sphereCount
        delegate: Entity {
            property real theta: index * 2 * Math.PI / sphereCount
            property bool isActiveLight: model.index === activeLight
            components: [
                sphereMesh,
                sphereTransform,
                sphereMaterial,
                objectsLayer
            ]

            SphereMesh {
                id: sphereMesh
                rings: 30
                slices: 30
                radius: .25
            }

            Transform {
                id: sphereTransform
                translation: Qt.vector3d(sphereDistance * Math.sin(theta), 0, sphereDistance * Math.cos(theta))
            }

            Kuesa.MetallicRoughnessMaterial {
                id: sphereMaterial
                effect: Kuesa.MetallicRoughnessEffect { }
                metallicRoughnessProperties: Kuesa.MetallicRoughnessProperties {
                    baseColorFactor: isActiveLight ? currentColor: "gray"
                    emissiveFactor: isActiveLight ?  Qt.rgba(currentColor.r * lightIntensity/maxLightIntesity, currentColor.g * lightIntensity/maxLightIntesity, currentColor.b* lightIntensity/maxLightIntesity) : "gray"
                }
            }
        }
    }
}

