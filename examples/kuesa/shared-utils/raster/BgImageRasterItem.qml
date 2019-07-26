/*
    KnobImageRasterItemActiveDot.qml

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Nuno Pinheiro <nuno.pinheiro@kdab.com>

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

Item {
    id: root
    property real dpi: 160
    property string dpiName

    function grabImage() {
        root.grabToImage(function(result) {
            result.saveToFile("bgImageraster" + dpiName + ".png")
        })
    }

    width: height + 2
    height: Math.ceil(dpi / 2.8)

    Rectangle {
        id: mask
        anchors.fill: parent
        anchors.margins: 1
        radius: height / 2
        color: "#33000000"

        // visual items shadows and edges
        Item {
            anchors.fill: parent
            Repeater {
                model: [ "#65000000", "#20000000", "#10000000" ]
                delegate: Rectangle {
                    anchors.fill: parent
                    anchors.margins: (model.index + 1) * Math.ceil(dpi / 150)
                    radius: height
                    border.color: modelData
                    color: "transparent"
                    border.width: Math.ceil(dpi / 150)
                }
            }
            Rectangle {
                anchors.fill: parent
                border.color: "#65ffffff"
                color: "#00000000"
                radius: height
                border.width: Math.ceil(dpi / 150)
            }
        }
    }
}
