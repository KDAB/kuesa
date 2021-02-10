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

Item {
    id: root
    property real batteryLife
    property real rpm
    property real torque

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
                    source: "icons/tachometer-alt-solid.svg"
                }
                text: "%1 RPM".arg(root.rpm.toFixed(0))
            }
            ToolSeparator {}
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: "icons/superpowers-brands.svg"
                }
                text: "%1 N.m".arg(root.torque.toFixed(2))
            }

            Item { Layout.fillWidth: true }
            ToolButton {
                icon {
                    width: 32
                    height: 32
                    source: {
                        if (batteryLife > 95)
                            return "icons/battery-full-solid.svg"
                        if (batteryLife >= 75)
                            return "icons/battery-three-quarters-solid.svg"
                        if (batteryLife >= 50)
                            return "icons/battery-half-solid.svg"
                        if (batteryLife >= 25)
                            return "icons/battery-quarter-solid.svg"
                        return "icons/battery-empty-solid.svg"
                    }
                }
                text: "%1 %".arg(root.batteryLife.toFixed(0))
            }
        }
    }
}
