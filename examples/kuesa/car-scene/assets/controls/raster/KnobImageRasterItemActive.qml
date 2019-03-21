/*
    KnobImageRasterItemActive.qml

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
            result.saveToFile(dpiName + "_knobImageRasterActive.png")
        })
    }

    width: height
    height:  Math.ceil(dpi / 2.8 - (dpi / 10))

    Rectangle {
        id: handle
        anchors.verticalCenter: parent.verticalCenter
        anchors.fill: parent
        radius: height / 2
        border.color: "#3996ff"
        color: "#603996ff"
        border.width: Math.ceil(dpi / 90)
    }
}

