/*
    StatusUI.qml

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
    id: root

    property StatusScreenController controller

    readonly property real batteryLife: controller.drillStatus.batteryLife
    readonly property real rpm: controller.drillStatus.rpm
    readonly property real torque: controller.drillStatus.torque
    readonly property int direction: controller.drillStatus.direction
    readonly property int mode: controller.drillStatus.mode
    readonly property real currentDraw: controller.drillStatus.currentDraw

    ToolBar {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        RowLayout {
            anchors.fill: parent
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: {
                        if (mode === DrillStatus.HammerDrill)
                            return "qrc:/icons/hammer-solid.svg"
                        if (mode === DrillStatus.ScrewDriving)
                            return "qrc:/icons/screwdriver-solid.svg"
                        // TO DO: Add Icon for regular drilling
                        return "qrc:/icons/screwdriver-solid.svg"
                    }
                }
                display: ToolButton.IconOnly
            }
            ToolSeparator {}
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: direction === DrillStatus.Clockwise ? "qrc:/icons/redo-solid.svg" : "qrc:/icons/undo-solid.svg"
                }
                display: ToolButton.IconOnly
            }
            ToolSeparator {}
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: "qrc:/icons/tachometer-alt-solid.svg"
                }
                text: "%1 RPM".arg(root.rpm.toFixed(0))
            }
            ToolSeparator {}
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: "qrc:/icons/superpowers-brands.svg"
                }
                text: "%1 N.m".arg(root.torque.toFixed(2))
            }
            ToolSeparator {}
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: "qrc:/icons/bolt-solid.svg"
                }
                text: "%1 A".arg(root.currentDraw.toFixed(2))
            }

            Item { Layout.fillWidth: true }
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: {
                        if (batteryLife > 95)
                            return "qrc:/icons/battery-full-solid.svg"
                        if (batteryLife >= 75)
                            return "qrc:/icons/battery-three-quarters-solid.svg"
                        if (batteryLife >= 50)
                            return "qrc:/icons/battery-half-solid.svg"
                        if (batteryLife >= 25)
                            return "qrc:/icons/battery-quarter-solid.svg"
                        return "qrc:/icons/battery-empty-solid.svg"
                    }
                }
                text: "%1 %".arg(root.batteryLife.toFixed(0))
            }
        }
    }

    // Labels for warnings
    Label {
        id: torqueWarningLabel
        x: controller.chuckPosition.x
        y: controller.chuckPosition.y - implicitHeight * 0.5
        text: "Elevated Torque Detected!"
        visible: controller.drillStatus.torqueWarning
        font.pointSize: 15
    }

    Label {
        id: currentWarningLabel
        x: controller.batteryPackPosition.x
        y: controller.batteryPackPosition.y - implicitHeight * 0.5
        text: "Elevated Current Draw Detected!"
        visible: controller.drillStatus.currentDrawWarning
        font.pointSize: 15
    }
}
