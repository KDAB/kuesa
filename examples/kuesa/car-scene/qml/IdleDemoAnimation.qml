/*
    IdleDemoAnimation.qml

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtQuick 2.11
import "controls" as Controls

Item {
    id: root
    property alias running: mainAnimation.running
    signal reset()

    function restartTimer() {
        idleTimer.restart()
        if (mainAnimation.running) {
            mainAnimation.running = false
            root.reset()
        }
    }

    Timer {
        id: idleTimer
        repeat: false
        running: true
        interval: 2 * 60 * 1000 // 2 minutes
        onTriggered: mainAnimation.running = true
    }

    SequentialAnimation {
        id: mainAnimation

        // Reset everything
        ScriptAction { script: root.reset() }

        // Show control panel
        ScriptAction { script: menuIcon.expanded = true }
        PauseAnimation { duration: 1000 }
        ScriptAction { script: titlePanel.showTitle("Use GLTF Cameras") }

        SequentialAnimation {
            // Start the engine
            NumberAnimation { target: speedC; property: "value"; to: 10; duration: 100 }
            // Wait a bit
            PauseAnimation { duration: 2500 }

            // Accelerate the engine
            NumberAnimation { target: speedC; property: "value"; to: 40; duration: 1100 }

            // Wait a bit
            PauseAnimation { duration: 1500 }
        }

        SequentialAnimation {
            ScriptAction { script: titlePanel.showTitle("Trigger GLTF Animations") }
            PauseAnimation { duration: 1000 }

            // Animate the panels
            PropertyAction { target: openRightDoorSwitch; property: "checked"; value: true }
            PauseAnimation { duration: 5000 }

            PropertyAction { target: openRightDoorSwitch; property: "checked"; value: false }
            PropertyAction { target: openHoodSwitch; property: "checked"; value: true }
            PauseAnimation { duration: 2500 }
            PropertyAction { target: openLeftDoorSwitch; property: "checked"; value: true }
            PauseAnimation { duration: 5000 }

            PropertyAction { target: openLeftDoorSwitch; property: "checked"; value: false }
            PropertyAction { target: openHoodSwitch; property: "checked"; value: false }

            PauseAnimation { duration: 1000 }
        }

        SequentialAnimation {
            // Show skybox
            PauseAnimation { duration: 1000 }
            PropertyAction { targets: showSkyboxSwitch; property: "checked"; value: true }
            ScriptAction { script: titlePanel.showTitle("Environment Maps") }
            PauseAnimation { duration: 2000 }

            // Hide control panel
            ScriptAction { script: menuIcon.expanded = false }
            PauseAnimation { duration: 1000 }

            // Change env map
            PropertyAction { targets: envNeuerZollhof; property: "checked"; value: true }
            PauseAnimation { duration: 5000 }

            PropertyAction { targets: envStudioSmall04; property: "checked"; value: true }
            PauseAnimation { duration: 5000 }

            PropertyAction { targets: envPinkSunrise; property: "checked"; value: true }
            PauseAnimation { duration: 2000 }

            // Hide skybox
            PropertyAction { targets: showSkyboxSwitch; property: "checked"; value: false }
            PauseAnimation { duration: 1000 }
        }

        // Show opacity mask
        PauseAnimation { duration: 1000 }
        PropertyAction { targets: useOpacityMaskSwitch; property: "checked"; value: true }
        ScriptAction { script: titlePanel.showTitle("Opacity Mask") }
        PauseAnimation { duration: 10000 }

        // Show control panel
        ScriptAction { script: menuIcon.expanded = true }

        SequentialAnimation {
            id: colorAnimations

            PauseAnimation { duration: 2000 }
            ScriptAction { script: titlePanel.showTitle("Dynamic Materials") }

            ParallelAnimation {
                NumberAnimation { target: redColor; property: "value"; to: 1.; duration: 1000 }
                NumberAnimation { target: greenColor; property: "value"; to: 1.; duration: 1000 }
                NumberAnimation { target: blueColor; property: "value"; to: 0.; duration: 1000 }
            }
            PauseAnimation { duration: 5000 }

            ParallelAnimation {
                NumberAnimation { target: redColor; property: "value"; to: 1.; duration: 1000 }
                NumberAnimation { target: greenColor; property: "value"; to: 0.; duration: 1000 }
                NumberAnimation { target: blueColor; property: "value"; to: 0.; duration: 1000 }
            }
            PauseAnimation { duration: 5000 }

            ParallelAnimation {
                NumberAnimation { target: redColor; property: "value"; to: 0.; duration: 1000 }
                NumberAnimation { target: greenColor; property: "value"; to: 0.07; duration: 1000 }
                NumberAnimation { target: blueColor; property: "value"; to: 0.2; duration: 1000 }
            }
            PauseAnimation { duration: 1000 }
        }

        // Hide opacity mask
        PropertyAction { targets: useOpacityMaskSwitch; property: "checked"; value: false }
        PauseAnimation { duration: 5000 }

        // Stop the engine
        PropertyAction { target: speedC; property: "value"; value: 0 }
        PauseAnimation { duration: 1500 }

        // Hide control panel
        ScriptAction { script: menuIcon.expanded = false }

        // Wait a bit
        PauseAnimation { duration: 2000 }

        loops: Animation.Infinite
    }


    // Title Panel & Animation
    Rectangle {
        id: titlePanel
        visible: opacity > 0
        opacity: 0
        scale: 0
        radius: 20
        color: "#33ffffff"
        width: childrenRect.width + 20
        height: childrenRect.height + 20

        anchors {
            bottom: parent.bottom
            bottomMargin: 2 * Controls.SharedAttributes.ldpi
            horizontalCenter: parent.horizontalCenter
            horizontalCenterOffset: menu.width / 2
        }

        function showTitle(title) {
            titleLabel.text = title
            titleAnimation.running = true
        }

        function dismiss() {
            titleAnimation.running = false
            opacity = 0
        }

        Controls.StyledLabel {
            id: titleLabel
            x: 10
            y: 10
            width: implicitWidth
            color: "#99ffffff"
            font.pixelSize: Controls.SharedAttributes.ldpi
        }
    }

    SequentialAnimation {
        id: titleAnimation

        ParallelAnimation {
            PropertyAnimation { target: titlePanel; property: "opacity"; from: .1; to: 1.; duration: 500; easing.type: Easing.InOutQuad }
            PropertyAnimation { target: titlePanel; property: "scale"; from: .1; to: 1.; duration: 500; easing.type: Easing.OutBack }
        }
        PauseAnimation { duration: 3000 }
        ParallelAnimation {
            PropertyAnimation { target: titlePanel; property: "opacity"; to: 0.; duration: 500; easing.type: Easing.OutQuad }
            PropertyAnimation { target: titlePanel; property: "scale"; to: 0.; duration: 500; easing.type: Easing.OutQuad }
        }
    }

    onRunningChanged: {
        if (!running)
            titlePanel.dismiss()
    }
}
