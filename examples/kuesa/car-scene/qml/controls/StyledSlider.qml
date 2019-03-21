/*
    StyledSlider.qml

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
import QtQuick.Controls 2.3

Item {
    id: slideN
    implicitHeight: bg.sourceSize.height
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

    BorderImage {
        id: bg
        source: "file:///tmp/bgImageraster.png"
        width: slideN.width
        height: parent.height

        border.left:height/2; border.top: height/2
        border.right: height/2; border.bottom: height/2
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

        Item {
            id: handle
            x: SharedAttributes.ldpi * 0.045
            width: height+handleControl.x
            height:  parent.height - Math.ceil(( SharedAttributes.ldpi * 0.09)/2)*2

            anchors.verticalCenter: parent.verticalCenter
            BorderImage  {
                width: Math.ceil(parent.width)
                height: Math.ceil(parent.height)
                border.bottom: (Math.floor(height/2)-2)
                border.right: border.bottom; border.top: border.bottom ; border.left: border.bottom
                source: "file:///tmp/knobImageRaster.png"
                opacity: controller.pressed ? 0:1
                Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
                Image {

                    source: "file:///tmp/knobImageRasterDot.png"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            BorderImage  {
                width: Math.ceil(parent.width)
                height: Math.ceil(parent.height)
                border.bottom: (Math.floor(height/2)-2)
                border.right: border.bottom; border.top: border.bottom ; border.left: border.bottom
                source: "file:///tmp/knobImageRasterActive.png"
                opacity: controller.pressed ? 1:0
                Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
                Image {

                    source: "file:///tmp/knobImageRasterActiveDot.png"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }


        }
    }
}
