/*
    StyledSlider.qml

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

import QtQuick 2.11
import QtQuick.Controls 2.3

Item {
    id: slideN
    implicitHeight: Math.ceil(SharedAttributes.ldpi / 2.39)
    property alias minimumValue: controller.from
    property alias maximumValue: controller.to
    property alias value: controller.value
    readonly property real progress: (value - minimumValue) / (maximumValue - minimumValue)

    Slider {
        id: controller
        anchors.fill: parent
        handle: Item {
                anchors.centerIn: parent
                width: slideN.height
                height: width
            }
        background: Item { }
    }

    Rectangle {
        id: mask
        anchors.fill: parent
        radius: SharedAttributes.ldpi / 4.8
        color: "#33000000"

        Rectangle {
            width: handle.width
            height: handle.height
            radius: width
            anchors.verticalCenter: parent.verticalCenter
            x: SharedAttributes.ldpi * 0.04
            color: "#11aa0000"

            Rectangle {
                width: parent.height / 2.5
                height: width
                radius: width
                anchors.right: parent.right
                anchors.rightMargin: (parent.height - height) / 2
                color: "#1100aa00"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Item {
        id: handleControl
        width: parent.height
        height: width
        x: progress * (slideN.width - width)
    }

    // visual items shadows and edges
    Item {
        anchors.fill: parent
        anchors.margins: -1

        Repeater {
            model: [ "#65000000", "#20000000", "#10000000" ]
            delegate: Rectangle {
                anchors.fill: parent
                anchors.margins: model.index + 1
                radius: height
                border.color: modelData
                color: "transparent"
            }
        }

        Repeater {
            model: [ "#75000000", "#25000000", "#10000000" ]
            delegate: Rectangle {
                anchors.fill: parent
                anchors.margins: -(model.index + 1)
                radius: height
                border.color: modelData
                color: "transparent"
            }
        }

        Rectangle {
            id: handle
            anchors.verticalCenter: parent.verticalCenter
            width: parent.height - SharedAttributes.ldpi * 0.08 + handleControl.x
            height: Math.ceil(parent.height - SharedAttributes.ldpi * 0.08)
            radius: height
            x: SharedAttributes.ldpi * 0.04
            border.color: controller.pressed ? "#3996ff" : "#65ffffff"
            color: controller.pressed ? "#603996ff" : "#16ffffff"

            Behavior on color { ColorAnimation { duration: 200 } }
            Behavior on border.color { ColorAnimation { duration: 200 } }
            Behavior on border.width { NumberAnimation { duration: 400 } }

            Rectangle {
                width: parent.height/2.5
                height: width
                radius: width
                anchors.right: parent.right
                anchors.rightMargin: (parent.height-height)/2
                color: controller.pressed? "#90000000" : "#00000000"
                border.color: parent.border.color
                border.width:  controller.pressed? 2 : 1
                anchors.verticalCenter: parent.verticalCenter

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: height
                    border.color: "#55000000"
                    color: "transparent"
                }

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 2
                    radius: height
                    border.color: "#15000000"
                    color: "transparent"
                }
            }
        }

        Rectangle {
            anchors.fill: parent
            border.color: "#65ffffff"
            color: "#00000000"
            radius: height
        }
    }
}
