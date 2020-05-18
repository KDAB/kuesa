/*
    CarCameraController.qml

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

import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Logic 2.2
import QtQml 2.12
import QtQuick 2.12 as QQ2

Entity {
    id: root
    property Camera camera
    property real linearSpeed: 8.0
    property real lookSpeed: 100.0

    QtObject {
        id: d
        readonly property vector3d firstPersonUp: Qt.vector3d(0, 1, 0)
        readonly property bool leftMouseButtonPressed: leftMouseButtonAction.active
        readonly property real vx: txAxis.value * linearSpeed;
        readonly property real vy: tyAxis.value * linearSpeed;
        readonly property real vz: tzAxis.value * linearSpeed;
        property real dx: rxAxis.value * lookSpeed
        property real dy: ryAxis.value * lookSpeed
        property bool actionJustStarted: true
        readonly property bool fineMotion: fineMotionAction.active
        QQ2.Behavior on dx { QQ2.NumberAnimation { duration: 250; easing.type: QQ2.Easing.OutCubic } }
        QQ2.Behavior on dy { QQ2.NumberAnimation { duration: 250; easing.type: QQ2.Easing.OutCubic } }
    }

    KeyboardDevice {
        id: keyboardSourceDevice
    }

    MouseDevice {
        id: mouseSourceDevice
        sensitivity: d.fineMotion ? 0.01 : 0.1
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
                    id: fineMotionAction
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Shift]
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
                // Translation
                Axis {
                    id: txAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Left]
                        scale: -1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Right]
                        scale: 1.0
                    }
                },
                Axis {
                    id: tzAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Up]
                        scale: 1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Down]
                        scale: -1.0
                    }
                },
                Axis {
                    id: tyAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_PageUp]
                        scale: 1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_PageDown]
                        scale: -1.0
                    }
                }
            ] // axes
        },

        FrameAction {
            property var radius: 0
            onTriggered: {
                if (!root.enabled)
                    return

                var oldPos = Qt.vector3d(root.camera.position.x,
                                         root.camera.position.y,
                                         root.camera.position.z);
                var oldViewCenter = Qt.vector3d(root.camera.viewCenter.x,
                                                root.camera.viewCenter.y,
                                                root.camera.viewCenter.z)
                var oldUpVec = Qt.vector3d(root.camera.upVector.x,
                                           root.camera.upVector.y,
                                           root.camera.upVector.z)

                if (d.actionJustStarted) {
                    // Record the radius the first time we are called
                    radius = oldPos.minus(root.camera.viewCenter).length()
                    d.actionJustStarted = false
                }

                // The time difference since the last frame is passed in as the
                // argument dt. It is a floating point value in units of seconds.
                root.camera.translate(Qt.vector3d(d.vx, d.vy, d.vz).times(dt))

                if (d.leftMouseButtonPressed && !d.actionJustStarted) {
                    root.camera.panAboutViewCenter(-d.dx * dt, d.firstPersonUp)
                    root.camera.tiltAboutViewCenter(-d.dy * dt)
                }

                // Compute angle between viewVector and upVector
                var normalizedViewVect = root.camera.viewVector.normalized()
                var normalizedUpVect = root.camera.upVector.normalized()
                var viewAngle = Math.acos(normalizedViewVect.dotProduct(d.firstPersonUp)) * 180 / Math.PI;
                var upAngle = Math.acos(normalizedUpVect.dotProduct(d.firstPersonUp)) * 180 / Math.PI;

                // 90 == floor level, 180 would be camera looking straight down at floor from top
                if (viewAngle < 90 || upAngle > 85) {
                    root.camera.position = oldPos
                    root.camera.viewCenter = oldViewCenter
                    root.camera.upVector = oldUpVec
                    root.camera.panAboutViewCenter(-d.dx * dt, d.firstPersonUp)
                }
            }
        }
    ] // components
}
