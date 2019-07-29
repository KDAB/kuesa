/*
    CameraController.qml

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
import Qt3D.Logic 2.12
import QtQml 2.11
import QtQuick 2.11 as QQ2

Entity {
    id: root

    property Camera camera

    property real lookSpeed: 30000.0
    property real mouseWheelZoomSpeed: 0.001
    property real zoomSpeed: 0.01
    property real panSpeed: 0.00125

    property real minCameraToViewCenterDistance: 2.
    property real maxCameraToViewCenterDistance: 20.

    QtObject {
        id: d
        readonly property vector3d firstPersonUp: Qt.vector3d(0, 1, 0)

        readonly property bool leftMouseButtonPressed: leftMouseButtonAction.active || (Qt.platform.os == "osx" && rightMouseButtonAction.active && metaAction.active)

        property real dx: rxAxis.value * lookSpeed
        property real dy: ryAxis.value * lookSpeed
        property real dz: rzAxis.value * lookSpeed

        QQ2.Behavior on dx { QQ2.NumberAnimation { duration: 250; easing.type: QQ2.Easing.OutCubic } }
        QQ2.Behavior on dy { QQ2.NumberAnimation { duration: 250; easing.type: QQ2.Easing.OutCubic } }
        QQ2.Behavior on dz { QQ2.NumberAnimation { duration: 250; easing.type: QQ2.Easing.OutCubic } }
    }

    function rotateCamera(dx, dy) {
        camera.panAboutViewCenter(-dx, d.firstPersonUp)

        // avoid going upside down
        var tiltCamera = false;
        if (dy > 0) {
            // allow tilt down
            tiltCamera = true;
        } else {
            // but constraint tilting up
            var vv = camera.viewVector.normalized();
            var uv = camera.upVector.normalized();

            var a = vv.crossProduct(uv); // orthogonal to the viewvector and upvector
            var b = vv.crossProduct(a); // orthogonal to the viewvector, on vv/uv plane

            if (Math.abs(b.y) > 0.2) // allow tilting only if we're not almost horizontal
                tiltCamera = true;
        }

        var cameraToCenter = camera.position.minus(camera.viewCenter).length()

        if (tiltCamera)
            camera.tiltAboutViewCenter(-dy);

        // Keep camera above the floor plane
        if (camera.position.y < 0) {
            camera.position.y = 0;

            // we may get closer to the view center if we simply clamp y on the camera
            // position, so make sure the previous distance to the view center is preserved
            var v = camera.position.minus(camera.viewCenter).normalized()
            camera.position = camera.viewCenter.plus(v.times(cameraToCenter))
        }

        // need to do this to force a recomputation of the viewMatrix, otherwise errors
        // will pile up and the rotation component (the upper 3x3 submatrix) of the
        // viewMatrix will quickly become non-orthogonal
        camera.upVector = d.firstPersonUp
    }

    function zoomCamera(dz) {
        var cameraToCenter = camera.position.minus(camera.viewCenter)

        var distance = cameraToCenter.length()
        distance -= distance* dz
        distance = Math.min(Math.max(distance, minCameraToViewCenterDistance), maxCameraToViewCenterDistance)

        camera.position = camera.viewCenter.plus(cameraToCenter.normalized().times(distance))
    }

    KeyboardDevice {
        id: keyboardSourceDevice
    }

    KeyboardHandler {
        sourceDevice: keyboardSourceDevice
        focus: true
    }

    MouseDevice {
        id: mouseSourceDevice
        sensitivity: d.fineMotion ? 0.0001 : 0.001
    }

    components: [
        LogicalDevice {
            id: cameraControlDevice

            actions: [
                Action {
                    id: leftMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.LeftButton]
                    }
                },
                Action {
                    id: middleMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.MiddleButton]
                    }
                },
                Action {
                    id: rightMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.RightButton]
                    }
                },
                Action {
                    id: shiftAction
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Shift]
                    }
                },
                Action {
                    id: ctrlAction
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Control]
                    }
                },
                Action {
                    id: metaAction
                    enabled: Qt.platform.os == "osx"
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Meta]
                    }
                },
                Action {
                    id: altAction
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Alt]
                    }
                }
            ] // actions

            axes: [
                // Rotation
                Axis {
                    id: rxAxis
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.X
                    }
                },
                Axis {
                    id: ryAxis
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.Y
                    }
                },
                // Zoom
                Axis {
                    id: rzAxis
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.WheelY
                    }
                }
            ] // axes
        },

        FrameAction {
            onTriggered: {
                if (!root.enabled)
                    return

                // zoom with mouse wheel
                zoomCamera(d.dz * mouseWheelZoomSpeed * dt)

                if (d.leftMouseButtonPressed)
                    rotateCamera(d.dx * dt, d.dy * dt);
            }
        }
    ] // components
}
