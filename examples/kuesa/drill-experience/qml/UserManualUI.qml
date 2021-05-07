/*
    UserManualUI.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Drill 1.0

Item {
    property ScreenController controller
      // 2D Text Labels

    Repeater {
        id: labelsRepeater

        model: enabled ? controller.partLabels : 0
        enabled: controller.mode === ScreenController.UserManualMode

        Item {
            Layout.alignment: Qt.AlignHCenter
            x: model.modelData.position.x
            y: model.modelData.position.y
            scale: mouseArea.pressed ? 0.9 : 1
            Behavior on scale { ScaleAnimator { duration: 300 } }

            width: label.implicitWidth * 2
            height: label.implicitHeight

            readonly property bool isClickable:  model.modelData.part !== ScreenController.NoPartSelected
            visible: controller.selectedPart === ScreenController.NoPartSelected ||
                     model.modelData.part === controller.selectedPart

            // Left Side Handle
            Rectangle {
                visible: isClickable
                id: handle
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: 2
                }
                height: label.fontInfo.pixelSize * 1.5
                width: height
                radius: width * 0.5
                color: "transparent"
                border {
                    width: 1
                    color: "white"
                }

                Rectangle {
                    anchors {
                        fill: parent
                        margins: 5
                    }
                    radius: width * 0.5
                    color: "white"
                    opacity: mouseArea.containsMouse ? 1 : 0
                    Behavior on opacity { OpacityAnimator { duration: 300 } }
                }
            }

            // Text Label
            Label {
                id: label
                text: model.modelData.labelName
                anchors {
                    left: handle.right
                    leftMargin: 15
                    verticalCenter: parent.verticalCenter
                }
                font.pointSize: 15
                font.bold: mouseArea.containsMouse
            }

            MouseArea {
                id: mouseArea
                enabled: isClickable
                anchors.fill: parent
                onClicked: controller.selectedPart = model.modelData.part
                hoverEnabled: true
            }
        }
    }
}
