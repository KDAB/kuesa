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

import QtQuick 2.12
import QtQuick.Controls 2.12

import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Extras 2.12
import Kuesa 1.3 as Kuesa
import Kuesa.Utils 1.3 as KuesaUtils

Item {
    id: mainRoot

    // 3D Content
    Kuesa.View3D {
        id: scene3D
        anchors.fill: parent
        focus: true
        multisample: true
        // showDebugOverlay: true

        source: "qrc:/car.gltf"
        camera: "CamSweep_Orientation"

        // Adding animation
        property bool running: true
        animations: [
            Kuesa.AnimationPlayer { name: "MatMotorBlockAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running; id: mainAnimation },
            Kuesa.AnimationPlayer { name: "TriggerMotorInfoAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running },
            Kuesa.AnimationPlayer { name: "DoorLeftAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running },
            Kuesa.AnimationPlayer { name: "DoorRightAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running },
            Kuesa.AnimationPlayer { name: "HoodAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running },
            Kuesa.AnimationPlayer { name: "SweepCamAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running },
            Kuesa.AnimationPlayer { name: "SweepCamCenterAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running },
            Kuesa.AnimationPlayer { name: "SweepCamPitchAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene3D.running }
        ]

        transformTrackers: [
            Kuesa.TransformTracker { id: motorTracker; name: "TriggerMotorInfo" }
        ]

        Kuesa.QuickSceneMaterial {
            name: "MatScreen"
            collection: scene3D.materials

            Item {
                width: 512
                height: 512

                Text {
                    anchors.centerIn: parent
                    text: "Hello! I'm a QtQuick Text Item"
                    font.pixelSize: 36
                    color: "white"
                }
            }
        }
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 5
        y: 20

        Button {
            text: "|<"
            onClicked: {
                scene3D.stop()
                scene3D.gotoStart()
                scene3D.running = false
            }
        }

        Button {
            text: scene3D.running ? "Stop" : "Resume"
            onClicked: scene3D.running = !scene3D.running
        }

        Button {
            text: ">|"
            onClicked: {
                scene3D.stop()
                scene3D.gotoEnd()
                scene3D.running = false
            }
        }

        Slider {
            id: slider
            from: 0
            to: 1
            value: mainAnimation.normalizedTime
            onMoved: {
                scene3D.stop()
                scene3D.gotoNormalizedTime(value)
            }
            width: mainRoot.width * .2
        }
    }

    Text {
        id: innerText

        // Bind position and opacity to motorLabelScreenPosition and motorLabelOpacity
        readonly property point position: motorTracker.screenPosition
        x: position.x - implicitWidth * 0.5
        y: position.y - implicitHeight * 0.5
        opacity: 0.5 * motorTracker.translation.y

        font.bold: true

        color: "white"
        text: "Overheating"
        style: Text.Sunken
        styleColor: "red"

        // Animate the size of the text
        NumberAnimation on scale {
            from: 2; to: 4
            duration: 700
            easing.type: Easing.InOutCubic
            loops: Animation.Infinite
        }
    }

    // Logos
    Image {
        id: kdabLogoBottomRight
        width: parent.width/13
        smooth: true
        antialiasing: true
        source: "kdab_logo_white.png"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: width/4
        anchors.rightMargin: width/4
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onPressAndHold: menu.idleAnimationRunning = true
        }
    }

    Image {
        id: qtLogo
        property bool kuesaLogo: true
        anchors.verticalCenter: kdabLogoBottomRight.verticalCenter
        anchors.right: kdabLogoBottomRight.left
        anchors.rightMargin: kdabLogoBottomRight.anchors.rightMargin/1.3
        width: parent.width/15* (kuesaLogo ? 2.3 : 1.8)
        smooth: true
        antialiasing: true
        source: kuesaLogo ? "Kuesa-logo-black.png" : "Qt-logo.png"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onPressAndHold: qtLogo.kuesaLogo = !qtLogo.kuesaLogo
        }
    }
}
