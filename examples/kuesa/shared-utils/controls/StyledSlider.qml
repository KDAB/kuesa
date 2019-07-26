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

import QtQuick 2.12
import QtQuick.Controls 2.12

BorderImage {
    id: slideN
    source: "bgImageraster.png"
    height: 41
    width: 41
    border.left: 20; border.right: 20
    border.top: 20; border.bottom: 20

    property alias minimumValue: controller.from
    property alias maximumValue: controller.to
    property alias value: controller.value
    property alias stepSize: controller.stepSize
    property alias snapMode: controller.snapMode
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

    Item {
        id: handleControl
        width: parent.height
        height: width
        x: Math.max(0, progress * (slideN.width - width))
    }

    // visual items shadows and edges
    BorderImage  {
        id: handle
        x: 5
        height: parent.height - Math.ceil((SharedAttributes.ldpi * 0.09) / 2) * 2
        width: height + handleControl.x
        anchors.verticalCenter: parent.verticalCenter
        border.left: 16; border.right: 16
        border.top: 16; border.bottom: 16
        source: "knobImageRaster.png"
        opacity: controller.pressed ? 0:1
        Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
        Image {
            source: "knobImageRasterDot.png"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    BorderImage  {
        anchors.fill: handle
        border.left: 16; border.right: 16
        border.top: 16; border.bottom: 16
        source: "knobImageRasterActive.png"
        opacity: controller.pressed ? 1:0
        Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
        Image {
            source: "knobImageRasterActiveDot.png"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
