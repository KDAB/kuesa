/*
    StyledSwitch.qml

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2

Item {
    id: sliderN
    implicitHeight: Math.ceil(SharedAttributes.ldpi / 2.42)
    implicitWidth: SharedAttributes.ldpi

    property bool radio: false
    property alias checked: sliderControl.onn
    property ExclusiveGroup exclusiveGroup: null
    enabled: !(radio && checked)

    onExclusiveGroupChanged: {
        if (exclusiveGroup)
            exclusiveGroup.bindCheckable(sliderN)
    }
    onCheckedChanged: sliderControl.currentIndex=checked ? 0 : 1

    Rectangle {
        id: mask
        anchors.fill: parent
        radius: SharedAttributes.ldpi / 4.8
        color: "#33000000"

        Rectangle {
            width: parent.height - SharedAttributes.ldpi * 0.04
            height: width
            radius: width
            anchors.verticalCenter: parent.verticalCenter
            x: -sliderControl.contentX + sliderControl.width - width + sliderControl.x
            color: "#11aa0000"
            scale: handle.scale
        }
    }

    ListView {
        id: sliderControl
        property bool onn: currentIndex==0
        property bool move: false
        interactive: !radio

        x: SharedAttributes.ldpi * 0.04
        width: parent.width - x * 2 + 2
        height: parent.height
        boundsBehavior: Flickable.StopAtBounds
        model: 2
        orientation: ListView.Horizontal
        highlightRangeMode: ListView.StrictlyEnforceRange
        currentIndex: sliderControl.onn ? 0 : 1
        onCurrentIndexChanged: checked = currentIndex == 0

        delegate: Item {
            width: sliderControl.width-handle.width
            height: sliderControl.height
        }
    }

    MouseArea {
        id: marea
        anchors.fill: sliderControl
        onClicked: sliderControl.currentIndex = (sliderControl.currentIndex + 1) % 2

        Binding {
            target: sliderControl
            property: "move"
            value: marea.pressed
        }
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
                anchors.centerIn: handle
                width: handle.width + 2 * (model.index + 1) / handle.scale
                height: width
                radius: height
                scale: handle.scale
                border.width: 1 / scale
                border.color: modelData
                color: "transparent"
            }
        }

        Rectangle {
            id: handle
            anchors.verticalCenter: parent.verticalCenter
            width: Math.ceil(parent.height - SharedAttributes.ldpi * 0.08)
            height: width
            radius: width
            x: -sliderControl.contentX + sliderControl.width - width + sliderControl.x
            border.color: sliderN.enabled && (sliderControl.move || sliderControl.moving) ? "#3996ff" : "#65ffffff"
            border.width: sliderN.enabled && (sliderControl.move || sliderControl.moving) ? 2 : 1
            color: checked ? (sliderN.width === sliderN.height? "#903996ff" : "#603996ff" ) : "#16ffffff"
            scale: radio && !enabled ? 0.5 : 1

            Behavior on color {ColorAnimation { duration: 200 } }
            Behavior on border.color { ColorAnimation { duration: 200 } }
            Behavior on border.width { NumberAnimation { duration: 400 } }
            Behavior on scale {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
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
