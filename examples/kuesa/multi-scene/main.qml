/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    KuesaUtils.View3D {
        id: scene3D
        anchors.fill: parent
        focus: true
        multisample: true
        showDebugOverlay: true
        asynchronous: true
        activeScene: scene1

        KuesaUtils.SceneConfiguration {
            id: scene1
            source: "qrc:/car.gltf"
            cameraName: "CamSweep_Orientation"

            property bool running: true
            animations: [
                Kuesa.AnimationPlayer { name: "MatMotorBlockAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running; id: mainAnimation },
                Kuesa.AnimationPlayer { name: "TriggerMotorInfoAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running },
                Kuesa.AnimationPlayer { name: "DoorLeftAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running },
                Kuesa.AnimationPlayer { name: "DoorRightAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running },
                Kuesa.AnimationPlayer { name: "HoodAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running },
                Kuesa.AnimationPlayer { name: "SweepCamAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running },
                Kuesa.AnimationPlayer { name: "SweepCamCenterAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running },
                Kuesa.AnimationPlayer { name: "SweepCamPitchAction"; loops: Kuesa.AnimationPlayer.Infinite; running: scene1.running }
            ]

            transformTrackers: [
                Kuesa.TransformTracker { id: motorTracker; name: "TriggerMotorInfo" },
                Kuesa.TransformTracker { id: motorEntityTracker; name: "MotorBlock" }
            ]
        }

        KuesaUtils.SceneConfiguration {
            id: scene2
            source: "qrc:/car2.gltf"
            cameraName: "CamCockpit_Orientation"
        }

        Kuesa.QuickSceneMaterial {
            name: "MatScreen"
            collection: scene3D.materials

            Item {
                width: 512
                height: 512
                // To mirror content vertically
                transform: Scale { origin.x: 256; origin.y: 256; xScale: 1; yScale: -1 }

                Text {
                    anchors.centerIn: parent
                    text: "Hello! I'm a QtQuick Text Item"
                    font.pixelSize: 36
                    color: "white"
                }
            }
        }

        Kuesa.Asset {
            id: carAsset
            name: "CAR"
            collection: scene3D.scene.transforms
        }

        onLoadingDone: {
            console.log("Loaded", source)
            console.log(scene3D.cameras.names)
        }
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 5
        y: 20

        ButtonGroup {
            buttons: [scene1button, scene2button]
        }

        Button {
            id: scene1button
            checkable: true
            checked: true
            text: "Scene 1"
            onClicked: scene3D.activeScene = scene1
        }

        Button {
            id: scene2button
            checkable: true
            text: "Scene 2"
            onClicked: scene3D.activeScene = scene2
        }

        Item { width: 200; height: 1 }

        Button {
            text: "|<"
            onClicked: {
                scene3D.stop()
                scene3D.gotoStart()
                scene3D.running = false
            }
        }

        Button {
            text: scene1.running ? "Stop" : "Resume"
            onClicked: scene1.running = !scene1.running
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

        Button {
            text: "Bump!"
            onClicked: carAnimation.start()
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

    SequentialAnimation {
        running: motorTracker.translation.y > 0
        loops: NumberAnimation.Infinite

        NumberAnimation {
            target: motorEntityTracker
            property: "scale"
            from: 1
            to: 1.02
            duration: 250
        }
        NumberAnimation {
            target: motorEntityTracker
            property: "scale"
            from: 1.02
            to: 1
            duration: 250
        }

        onStopped: motorEntityTracker.scale = 1
    }

    SequentialAnimation {
        id: carAnimation

        NumberAnimation {
            target: carAsset.node
            property: "scale"
            duration: 200
            easing.type: Easing.InOutQuad
            to: 1.2
        }

        NumberAnimation {
            target: carAsset.node
            property: "scale"
            duration: 100
            easing.type: Easing.OutQuad
            to: 1
        }
    }

    // Loading animation
    Rectangle {
        anchors.fill: parent
        visible: !scene3D.ready

        BusyIndicator {
            anchors.centerIn: parent

            height: parent.height * .5
            width: height
            running: parent.visible
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
