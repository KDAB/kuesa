/*
    DrillScene.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import Qt3D.Core 2.15
import Qt3D.Input 2.15
import Kuesa 1.3
import Kuesa.Utils 1.3
import Drill 1.0
import QtQuick 2.15 as QQ2

//! [0]
View3D {
    id: view3D
    focus: true
    asynchronous: true
    backgroundColor: "transparent"
    opacity: ready ? 1.0 : 0.0

    //! [1]

    // Controllers
    // Readonly properties to expose controllers for external access
    readonly property ScreenController controller: _controller

    ScreenController {
        id: _controller
    }

    // We rely on the controller providing the scene configuration
    // This provides the source, camera, trackers, animation players...
    activeScene: controller.sceneConfiguration
    //! [1]

    Entity {
        components: [
            MouseHandler {
                id: mouseHandler
                sourceDevice: MouseDevice {}
                // Use progress to control the orbit animation when in user manual mode
                property real progressOffset
                property bool isPressed: false

                onPressed: {
                    isPressed = true;
                    idleDetectionTimer.restart()
                    // Switch to the User Manual mode when pressing the screen
                    // while on the status screen
                    if (controller.mode === ScreenController.StatusMode) {
                        controller.mode = ScreenController.UserManualMode
                    } else if (controller.mode === ScreenController.UserManualMode){
                        // Record camera curve offset
                        progressOffset = controller.positionOnCameraOrbit + mouse.x / view3D.width
                    } else { // GuidedDrillingMode
                        controller.nextStep();
                    }
                }

                onReleased: isPressed = false;

                onPositionChanged: {
                    // Move camera along orbit curve
                    if (isPressed)
                        controller.positionOnCameraOrbit = Math.min(1.0, Math.max(0, progressOffset - (mouse.x / view3D.width)))
                }
            }
        ]
    }

    QQ2.Timer {
        id: idleDetectionTimer
        running: controller.mode !== ScreenController.StatusMode
        interval: 5 * 60 * 1000 // 5 minutes
        onTriggered: controller.mode = ScreenController.StatusMode
    }
}
//! [0]
