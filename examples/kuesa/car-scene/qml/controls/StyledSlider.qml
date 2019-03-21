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

BorderImage {
    id: slideN
    source: SharedAttributes.dpiAssetPrefix + "_bgImageraster.png"

    property alias minimumValue: controller.from
    property alias maximumValue: controller.to
    property alias value: controller.value
    readonly property real progress: (value - minimumValue) / (maximumValue - minimumValue)

    border {
        left: height * 0.5 - 4
        right: height * 0.5 - 4
        top: height * 0.5 - 4
        bottom: height * 0.5 - 4
    }

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
        x: SharedAttributes.ldpi * 0.05
        height: parent.height - Math.ceil((SharedAttributes.ldpi * 0.09) / 2) * 2
        width: height + handleControl.x
        anchors.verticalCenter: parent.verticalCenter
        readonly property int borderWidth: Math.max(0, Math.min(width / 2 - 4, height / 2 - 4))
        border {
            top: handle.borderWidth
            bottom: handle.borderWidth
            left: handle.borderWidth
            right: handle.borderWidth
        }
        source: SharedAttributes.dpiAssetPrefix + "_knobImageRaster.png"
        opacity: controller.pressed ? 0:1
        Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
        Image {
            source: SharedAttributes.dpiAssetPrefix + "_knobImageRasterDot.png"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    BorderImage  {
        anchors.fill: handle
        border {
            top: handle.borderWidth
            bottom: handle.borderWidth
            left: handle.borderWidth
            right: handle.borderWidth
        }
        source: SharedAttributes.dpiAssetPrefix + "_knobImageRasterActive.png"
        opacity: controller.pressed ? 1:0
        Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
        Image {
            source: SharedAttributes.dpiAssetPrefix + "_knobImageRasterActiveDot.png"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
