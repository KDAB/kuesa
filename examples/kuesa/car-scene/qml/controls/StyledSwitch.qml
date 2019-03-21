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
    implicitHeight: bg.sourceSize.height
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

    ListView {
        id: sliderControl
        property bool onn: currentIndex==0
        property bool move: false
        interactive: bg.width>= bg.height+4

        x: SharedAttributes.ldpi * 0.04
        width: parent.width-x*2.1
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
        MouseArea {
            id: controller
            anchors.fill: parent
            onClicked: checked=!checked
        }
    }


    // visual items shadows and edges
    Item {
        anchors.fill: parent

        BorderImage {
            id: bg
            source: "file:///tmp/bgImageraster.png"
            width: Math.max(sliderN.width,height)
            height: parent.height

            border.left:height/2-1; border.top: border.left
            border.right: border.left; border.bottom: border.left
        }

        Image {
            id: handle
            source: "file:///tmp/knobImageRaster.png"

            anchors.verticalCenter: parent.verticalCenter
            width: Math.ceil(parent.height - SharedAttributes.ldpi * 0.1)
            height: width
            anchors.centerIn: sliderControl.interactive?undefined:parent
            x: -sliderControl.contentX + sliderControl.width - width + sliderControl.x
            scale: radio && !enabled ? 0.8 : 1
            opacity: controller.pressed||checked ? 0:1
            Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
            Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
        }
        Image {
         source: "file:///tmp/knobImageRasterActive.png"
         anchors.fill: handle
         opacity: controller.pressed||checked ? 1:0
         Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
         scale: handle.scale
        }
    }
}
