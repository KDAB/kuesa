/*
    StyledPanel.qml

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
import "."

Column {
    id: root
    property alias title: titleLabel.text
    property alias interactive: dragMa.enabled
    z: 1
    visible: opacity > 0

    Behavior on opacity { animation: SharedAttributes.opacityAnimation }

    property bool closed: false
    property bool hasCloseButton: true

    onClosedChanged: {
        if (closed)
            hide()
        else
            show()
    }

    SequentialAnimation {
        id: shakeAnimation
        RotationAnimation {
            targets: [root, panelBackground]
            from: -5
            to: 5
            property: "rotation"
            easing.type: Easing.OutInBounce
            duration: 250
        }
        PropertyAction {
            targets: [root, panelBackground]
            property: "rotation"
            value: 0
        }
        alwaysRunToEnd: true
    }

    function show() {
        shakeAnimation.start()
        root.opacity = 1
    }

    function hide() { root.opacity = 0 }

    Image {
        id: panelBackground
        source: "../menu_gradient.png"
        opacity: root.opacity * SharedAttributes.panelOpacity
        anchors.fill: root
        anchors.margins: -10
        parent: root.parent
        visible: root.visible

        MouseArea {
            id: dragMa
            anchors.fill: parent
            drag.target: root
            drag.axis: Drag.XAndYAxis
            drag.filterChildren: true
            // Panels are not draggable by default
            enabled: false
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border {
                width: 1
                color: SharedAttributes.controlsBorderColor
            }
            radius: 1
        }
    }

    Item {
        id: panelBar
        z: 1
        height: hideButtonRect.height
        width: Math.max(root.width, 1.2 * (titleLabel.width + hideButtonRect.width))

        StyledLabel {
            id: titleLabel
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10
            }
            font.bold: true
        }

        Rectangle {
            id: hideButtonRect
            height: SharedAttributes.controlsHeight
            width: height
            radius: width * 0.5
            anchors {
                right: parent.right
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            visible: hasCloseButton
            scale: hideButtonMa.pressed ? 0.9 : 1.0
            color: "transparent"
            Behavior on scale { animation: SharedAttributes.switchXAnimation }
            border {
                width: 1
                color: SharedAttributes.controlsBorderColor
            }
            Text {
                anchors.centerIn: parent
                text: "X"
                font.family: SharedAttributes.roboto.name
                color: SharedAttributes.labelTextColor
                font.bold: true
            }
            MouseArea {
                id: hideButtonMa
                anchors.fill: parent
                onClicked: root.hide()
            }
        }
    }

    Item {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 10
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 1
        color: SharedAttributes.controlsBorderColor
    }

    Item {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 5
    }
}

