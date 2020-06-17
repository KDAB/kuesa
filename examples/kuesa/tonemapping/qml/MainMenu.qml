/*
    MainMenu.qml

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Robert Brock <robert.brock@kdab.com>

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
    property string toneMappingAlgorithmName: "None"
    property alias toggleRotation: rotationToggleSwitch.checked
    property alias toggleLightRotation: lightToggleSwitch.checked
    property alias exposure: exposureSlider.value

    readonly property real largeFontSize: 15.0

    function reset() {
        speedC.value = 0
        lightToggleSwitch.checked = true
        rotationToggleSwitch.checked = false
        toneToggleSwitch.checked = false
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
                        text: "Tone Mapping Effect"
                        font.weight: Font.ExtraLight
                        font.pixelSize: largeFontSize
                    }

                    ColumnLayout {

                        ButtonGroup { id: radioButtonsGroup}

                        RadioButton {
                            ButtonGroup.group: radioButtonsGroup
                            text: "None"
                            onCheckedChanged: {
                                if (checked)
                                    toneMappingAlgorithmName = text
                            }
                        }

                        RadioButton {
                            checked: true
                            ButtonGroup.group: radioButtonsGroup
                            text: "Reinhard"
                            onCheckedChanged: {
                                if (checked)
                                    toneMappingAlgorithmName = text
                            }
                        }

                        RadioButton {
                            id: filmicEffectToneMappingSwitch
                            ButtonGroup.group: radioButtonsGroup
                            text: "Filmic"
                            onCheckedChanged: {
                                if (checked)
                                    toneMappingAlgorithmName = text
                            }
                        }

                        RadioButton {
                            id: unchartedEffectToneMappingSwitch
                            ButtonGroup.group: radioButtonsGroup
                            text: "Uncharted"
                            onCheckedChanged: {
                                if (checked)
                                    toneMappingAlgorithmName = text
                            }
                        }
                    }
                }

                GroupBox {
                    label: Label {
                        text: "Effects Control"
                        font.weight: Font.ExtraLight
                        font.pixelSize: largeFontSize
                    }

                    ColumnLayout {
                        spacing: 5

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

                        Flow {
                            width: parent.width
                            spacing: parent.spacing
                            Switch {
                                id: lightToggleSwitch
                                text: "Light Paused"
                                checked: false
                            }

                            Switch {
                                id: rotationToggleSwitch
                                text: "Helmet Rotation"
                                checked: false
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

        property bool expanded: true

        MouseArea {
            anchors.fill: parent
            anchors.margins: -width
            onClicked: parent.expanded = !parent.expanded
        }

        Rectangle {
            id: rect1
            width: parent.width * (1 + (-rotation / 200))
            y: -rotation / height
            height:  Math.ceil(width / 6)
            radius: height
            color: "#cccccc"
            rotation: parent.expanded? -24 : 0

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
            y: -rotation/height
        }
    }

    MouseArea {
        id: mainMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            mouse.accepted = false
        }
        onPositionChanged: {
            mouse.accepted = false
        }
    }
}
