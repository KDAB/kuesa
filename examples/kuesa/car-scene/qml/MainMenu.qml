/*
    IdleDemoAnimation.qml

    This file is part of Kuesa.

    Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

Item {
    id: root
    property alias carSpeed: speedC.value
    property alias openLeftDoor: openLeftDoorSwitch.checked
    property alias openRightDoor: openRightDoorSwitch.checked
    property alias openHood: openHoodSwitch.checked
    property string environmentMap: {
        if (envPinkSunrise.checked) return "pink_sunrise"
        if (envNeuerZollhof.checked) return "neuer_zollhof"
        if (envStudioSmall04.checked) return "kdab-studiosky-small"
    }
    property double environmentExposure: {
        if (envPinkSunrise.checked) return 0.0
        if (envNeuerZollhof.checked) return 0.0
        if (envStudioSmall04.checked) return -1.5
    }
    property alias showSkybox: showSkyboxSwitch.checked
    property alias exposure: exposureSlider.value
    property alias useOpacityMask: useOpacityMaskSwitch.checked
    property alias useBloomEffect: useBloomEffectSwitch.checked
    property color carBaseColorFactor: "white"
    property alias idleAnimationRunning: idleAnimation.running


    readonly property real largeFontSize: 15.0

    function reset() {
        speedC.value = 0
        openLeftDoorSwitch.checked = false
        openRightDoorSwitch.checked = false
        openHoodSwitch.checked = false
        useOpacityMaskSwitch.checked = false
        showSkyboxSwitch.checked = false
        useBloomEffectSwitch.checked = false
        bloomThreshold.value = .27
        bloomPasses.value = 2
    }

    Item {
        id: menu

        property int expandedWidth: 260
        property real switchWidth: 90

        width: menuIcon.expanded ? expandedWidth : 0
        height: parent.height
        clip: true
        visible: width > 0

        Behavior on width { NumberAnimation {duration: 500; easing.type: Easing.InOutQuad } }

        MouseArea {
            width: childrenRect.width
            height: parent.height

            Flickable {
                y: 55
                width: menu.expandedWidth - 10
                x: 5
                height: parent.height - y
                clip: true
                contentHeight: controlArea.height

                interactive: height < contentHeight

                ColumnLayout {
                    id: controlArea
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter

                    GroupBox {
                        label: Label {
                            text: "Car Control"
                            font.weight: Font.ExtraLight
                            font.pixelSize: largeFontSize
                        }

                        ColumnLayout {

                            Label {
                                text: "Car Wheels Speed"
                                font.weight: Font.ExtraLight
                                font.pixelSize: largeFontSize
                            }

                            Slider {
                                id: speedC
                                width: parent.width
                                from: 0
                                to: 8
                            }

                            Flow {
                                width: parent.width
                                spacing: parent.spacing

                                Switch {
                                    id: openHoodSwitch
                                    text: "Open Hood"
                                    checked: false
                                }

                                Switch {
                                    id: openLeftDoorSwitch
                                    text: "Left Door"
                                    checked: false
                                }

                                Switch {
                                    id: openRightDoorSwitch
                                    text: "Right Door"
                                    checked: false
                                }
                            }
                        }
                    }

                    GroupBox {
                        label: Label {
                            text: "Scene Control"
                            font.weight: Font.ExtraLight
                            font.pixelSize: largeFontSize
                        }
                        ColumnLayout {
                            Label {
                                text: "Exposure: " + parseFloat(Math.round(exposureSlider.value * 100) / 100).toFixed(2)
                                font.weight: Font.ExtraLight
                                font.pixelSize: largeFontSize
                            }
                            Slider {
                                id: exposureSlider
                                from: -5.0
                                to: 5
                                value: 1.0
                                width: parent.width
                            }
                            Label {
                                text: "Environment"
                            }
                            Flow {
                                width: parent.width
                                spacing: parent.spacing

                                ButtonGroup { id: radioButonsGroup }

                                RadioButton {
                                    id: envPinkSunrise

                                    text: "Pink Sunrise"
                                    ButtonGroup.group: radioButonsGroup
                                    checked: true
                                }

                                RadioButton {
                                    id: envNeuerZollhof

                                    text: "Neuer Zollhof"
                                    ButtonGroup.group: radioButonsGroup
                                }

                                RadioButton {
                                    id: envStudioSmall04

                                    text: "KDAB Studio"
                                    ButtonGroup.group: radioButonsGroup
                                }
                            }
                            Flow {
                                width: parent.width
                                spacing: parent.spacing
                                Switch {
                                    id: showSkyboxSwitch
                                    text: "Show Skybox"
                                    checked: false
                                }
                                Switch {
                                    id: useOpacityMaskSwitch
                                    text: "Use OpacityMask"
                                    checked: false
                                }
                            }
                        }
                    }

                    GroupBox {
                        label: Label {
                            text: "Car Color"
                            font.weight: Font.ExtraLight
                            font.pixelSize: largeFontSize
                        }
                        ColumnLayout {
                            Label {
                                text: "Red: " + parseFloat(Math.round(redColor.value * 255).toFixed(2))
                                font.weight: Font.ExtraLight
                                font.pixelSize: largeFontSize
                            }
                            Slider {
                                id: redColor
                                value: sceneContent.carBaseColorFactor.r
                                onValueChanged: if (value !== sceneContent.carBaseColorFactor.r)
                                                    sceneContent.carBaseColorFactor = Qt.rgba(redColor.value, greenColor.value, blueColor.value)
                            }
                            Label {
                                text: "Green: " + parseFloat(Math.round(greenColor.value * 255).toFixed(2))
                                font.weight: Font.ExtraLight
                                font.pixelSize: largeFontSize
                            }
                            Slider {
                                id: greenColor
                                value: sceneContent.carBaseColorFactor.g
                                onValueChanged: if (value !== sceneContent.carBaseColorFactor.g)
                                                    sceneContent.carBaseColorFactor = Qt.rgba(redColor.value, greenColor.value, blueColor.value)
                            }
                            Label {
                                text: "Blue: " + parseFloat(Math.round(blueColor.value * 255).toFixed(2))
                                font.weight: Font.ExtraLight
                                font.pixelSize: largeFontSize
                            }
                            Slider {
                                id: blueColor
                                value: sceneContent.carBaseColorFactor.b
                                onValueChanged: if (value !== sceneContent.carBaseColorFactor.b)
                                                    sceneContent.carBaseColorFactor = Qt.rgba(redColor.value, greenColor.value, blueColor.value)
                            }
                        }
                    }

                    GroupBox {
                        label: Switch {
                            id: useBloomEffectSwitch
                            text: "Bloom effect"
                            checked: false
                        }

                        ColumnLayout {

                            Label {
                                text: "th: " + parseFloat(bloomThreshold.value).toFixed(2)
                                font.weight: Font.ExtraLight
                                font.pixelSize: largeFontSize
                            }
                            Slider {
                                id: bloomThreshold
                                value: sceneContent.bloomEffect.threshold
                                onValueChanged: if (value !== sceneContent.bloomEffect.threshold)
                                                    sceneContent.bloomEffect.threshold = bloomThreshold.value
                            }
                            Label {
                                text: "pass: " + bloomPasses.value
                                font.weight: Font.ExtraLight
                                font.pixelSize: largeFontSize
                            }
                            Slider {
                                id: bloomPasses
                                from: 0
                                to: 8
                                stepSize: 1
                                snapMode: Slider.SnapAlways
                                value: sceneContent.bloomEffect.blurPassCount
                                onValueChanged: if (value !== sceneContent.bloomEffect.blurPassCount)
                                                    sceneContent.bloomEffect.blurPassCount = bloomPasses.value
                            }
                        }
                    }
                }
            }
        }
    }

    Image {
        id: edge
        source: "edge.png"
        anchors.left: menu.right
        anchors.leftMargin: -1
        height: parent.height
    }

    Item {
        id: menuIcon
        width: 30
        height: Math.ceil(width * 0.9)
        x: width / 2
        y: Math.ceil(width * 0.75)

        property bool expanded: false

        MouseArea {
            anchors.fill: parent
            anchors.margins: -width
            onClicked: parent.expanded = !parent.expanded
        }

        Rectangle {
            id: rect1
            width: parent.width * (1 + (-rotation / 200))

            height:  Math.ceil(width / 6)
            radius: height
            color: "#cccccc"
            rotation: parent.expanded? -24 : 0
            y: -rotation/150*parent.width
            Behavior on rotation { NumberAnimation {duration: 250; easing.type: Easing.OutCirc } }
        }

        Rectangle {
            x: parent.expanded ? -parent.x - height : 0
            width: parent.expanded ? rect1.height : parent.width
            height:  rect1.height
            anchors.verticalCenter: parent.verticalCenter
            radius: height
            color: "#cccccc"

            Behavior on x { NumberAnimation {duration: 300; easing.type: Easing.InOutQuad } }
            Behavior on width { NumberAnimation {duration: 200; easing.type: Easing.InOutQuad } }
        }

        Rectangle {
            width: parent.width * (1 + (rotation / 200))
            height:  rect1.height
            anchors.bottom: parent.bottom
            radius: height
            color: "#cccccc"
            rotation: -rect1.rotation
            anchors.bottomMargin: rect1.y
        }
    }

    MouseArea {
        id: mainMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            idleAnimation.restartTimer()
            mouse.accepted = false
        }
        onPositionChanged: {
            idleAnimation.restartTimer()
            mouse.accepted = false
        }
    }

    // Idle Animation
    IdleDemoAnimation {
        id: idleAnimation
        running: true
        anchors.fill: parent

        minWidth: menu.expandedWidth

        onReset: root.reset()
    }
}
