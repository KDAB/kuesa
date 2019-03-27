/*
    StyledSwitch.qml

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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2

BorderImage {
    id: sliderN

    width: SharedAttributes.ldpi
    source: SharedAttributes.dpiAssetPrefix + "_bgImageraster.png"
    border.left:height / 2 - 1; border.top: border.left
    border.right: border.left; border.bottom: border.left

    property bool radio: false
    property bool checked: false
    property ExclusiveGroup exclusiveGroup: null
    enabled: !(radio && checked)

    onExclusiveGroupChanged: {
        if (exclusiveGroup)
            exclusiveGroup.bindCheckable(sliderN)
    }

    readonly property bool interactive: width >= height + 4

    MouseArea {
        id: controller
        anchors.fill: parent
        onClicked: checked=!checked
    }

    // visual items shadows and edges
    Image {
        id: handle
        source: SharedAttributes.dpiAssetPrefix + "_knobImageRaster.png"
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: Math.ceil((SharedAttributes.ldpi * 0.09) / 2)
        anchors.leftMargin: anchors.rightMargin
        width: Math.ceil(parent.height - SharedAttributes.ldpi * 0.1)
        height: width
        scale: radio && !enabled ? 0.8 : 1
        opacity: controller.pressed||checked ? 0:1
        Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
        Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
        states: [
            State {
                when: interactive && checked
                AnchorChanges {
                    target: handle
                    anchors.right: sliderN.right
                    anchors.left: undefined
                }
            },
            State {
                when: interactive && !checked
                AnchorChanges {
                    target: handle
                    anchors.left: sliderN.left
                    anchors.right: undefined
                }
            },
            State {
                when: !interactive
                AnchorChanges {
                    target: handle
                    anchors.horizontalCenter: sliderN.horizontalCenter
                }
            }
        ]
        transitions: Transition {
            from: "*"
            to: "*"
            AnchorAnimation {
                duration: 200
            }
        }
    }
    Image {
        source: SharedAttributes.dpiAssetPrefix + "_knobImageRasterActive.png"
        anchors.fill: handle
        opacity: controller.pressed||checked ? 1:0
        Behavior on opacity { NumberAnimation { easing.type: Easing.OutQuad; duration: 400 } }
        scale: handle.scale
    }
}
