/*
    MainMenu.qml

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
import QtQuick.Controls 1.4
import "controls" as Controls
import QtGraphicalEffects 1.0

Item {
    id: root
    readonly property string toneMappingAlgorithmName: radioButtonsGroup.current && radioButtonsGroup.current.parent ? radioButtonsGroup.current.parent.text : "None"
    property alias toggleRotation: rotationToggleSwitch.checked
    property alias toggleLightRotation: lightToggleSwitch.checked
    property alias exposure: exposureSlider.value

    function reset() {
        speedC.value = 0
        lightToggleSwitch.checked = true
        rotationToggleSwitch.checked = false
        toneToggleSwitch.checked = false
    }

    Item {
        id: menu

        property int expandedWidth: Math.min(Controls.SharedAttributes.ldpi * 3, root.width / 3)
        property real switchWidth: Math.floor( Math.min((expandedWidth - Controls.SharedAttributes.defaultSpacing * 4) / 3, Controls.SharedAttributes.ldpi * 1.3) ) - 1
        property real radioButtonWidth: Math.floor( Math.min( (expandedWidth - Controls.SharedAttributes.defaultSpacing * 4) / 3, Controls.SharedAttributes.ldpi * 1.3) ) - 10

        width: menuIcon.expanded ? expandedWidth : 0
        height: parent.height
        clip: true
        visible: width > 0

        Behavior on width { NumberAnimation {duration: 500; easing.type: Easing.InOutQuad } }

        Component {
            id: blurBg

            Item {
                Rectangle {
                    id: cliper

                    property real scaleReducer: 2.5
                    width: menu.expandedWidth / scaleReducer
                    height: root.height / scaleReducer
                    clip: true

                    ShaderEffectSource {
                        id: copy3D
                        width: baseUI.width / cliper.scaleReducer
                        height: baseUI.height / cliper.scaleReducer
                        sourceItem: baseUI
                        textureSize: Qt.size(width / cliper.scaleReducer, height / cliper.scaleReducer)
                        sourceRect : Qt.rect(0, 0, width * cliper.scaleReducer, height * cliper.scaleReducer)
                        samples: 1
                        mipmap: false
                    }
                }

                FastBlur {
                    anchors.fill: cliper
                    source: cliper
                    radius: 140/cliper.scaleReducer
                    transform: Scale {
                        origin.x: 0
                        origin.y: 0
                        xScale: root.height / cliper.height
                        yScale: xScale
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: "#40202020"
                    }
                }

                Image {
                    fillMode: Image.Tile
                    source: "noise.png"
                    width: cliper.width * (root.height / cliper.height)
                    height: root.height
                    opacity: 0.8
                }
            }
        }

        Component {
            id: noiseBg

            Rectangle {
                anchors.fill: parent
                color: "#33000000"

                Image {
                    fillMode: Image.Tile
                    source: "noise.png"
                    anchors.fill: parent
                    opacity: 0.8
                }
            }
        }

        Loader {
            anchors.fill: parent
            sourceComponent: blurBg
        }

        MouseArea {
            width: childrenRect.width
            height: parent.height

            Flickable {
                y: Controls.SharedAttributes.ldpi * 0.75
                width: menu.expandedWidth - Math.ceil(Controls.SharedAttributes.ldpi / 10) * 2
                x: Math.ceil(Controls.SharedAttributes.ldpi / 10)
                height: parent.height - y
                clip: true
                contentHeight: controlArea.height

                interactive: height < contentHeight

                Column {
                    x: 1
                    width: parent.width - 2
                    id: controlArea

                    Controls.GroupBox {
                        width: parent.width

                        Controls.StyledLabel {
                            text: "Tone Mapping Effect"
                            font.weight: Font.ExtraLight
                            font.pixelSize: Controls.SharedAttributes.largeFontSize
                        }

                        Rectangle{
                            width: parent.width
                            height: Math.ceil(Controls.SharedAttributes.ldpi / 150)
                            color: "#70ffffff"
                        }

                        ExclusiveGroup { id: radioButtonsGroup }

                        Controls.HorizontalLabeledSwitch {
                            exclusiveGroup: radioButtonsGroup
                            text: "None"
                        }

                        Controls.HorizontalLabeledSwitch {
                            checked: true
                            exclusiveGroup: radioButtonsGroup
                            text: "Reinhard"
                        }

                        Controls.HorizontalLabeledSwitch {
                            id: filmicEffectToneMappingSwitch
                            exclusiveGroup: radioButtonsGroup
                            text: "Filmic"
                        }

                        Controls.HorizontalLabeledSwitch {
                            id: unchartedEffectToneMappingSwitch
                            exclusiveGroup: radioButtonsGroup
                            text: "Uncharted"
                        }

                        Item {
                            height: Controls.SharedAttributes.defaultSpacing
                            width: 1
                        }

                        Controls.StyledLabel {
                            text: "Effects Control"
                            font.weight: Font.ExtraLight
                            font.pixelSize: Controls.SharedAttributes.largeFontSize
                        }

                        Rectangle{
                            width: parent.width
                            height: Math.ceil(Controls.SharedAttributes.ldpi / 150)
                            color: "#70ffffff"
                        }

                        Controls.LabeledSlider {
                            id: exposureSlider
                            text: "Exposure: " + parseFloat(Math.round(exposureSlider.value * 100) / 100).toFixed(2)
                            minimumValue: -5.0
                            maximumValue: 5
                            value: 1.7
                            width: parent.width
                        }

                        Flow {
                            width: menu.expandedWidth
                            spacing: parent.spacing

                            Controls.LabeledSwitch {
                                id: lightToggleSwitch
                                text: "Light Paused"
                                checked: false
                                width: menu.switchWidth
                            }

                            Controls.LabeledSwitch {
                                id: rotationToggleSwitch
                                text: "Helmet Rotation"
                                checked: false
                                width: menu.switchWidth
                            }
                        }
                    }

                    Item {
                        height: Controls.SharedAttributes.ldpi / 10
                        width: 1
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
        width: Math.ceil(Controls.SharedAttributes.ldpi / 3.5)
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
            rotation: parent.expanded? -0.25 * Controls.SharedAttributes.ldpi : 0

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
