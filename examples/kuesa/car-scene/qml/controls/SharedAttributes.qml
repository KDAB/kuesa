/*
    SharedAttributes.qml

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

pragma Singleton
import QtQuick 2.11
import QtQuick.Window 2.3

Item {
    readonly property real ldpi: _screenHeightScale || _isFullScreen
                                 ? 0.085 * Screen.height                // Window is full screen
                                 : Screen.pixelDensity * 25.4 * .7      // Variable size window
    readonly property alias regularFontName: openSansFontRegular.name
    readonly property real regularFontSize: ldpi / 7
    readonly property real largeFontSize: ldpi / 5.5
    readonly property real defaultSpacing: Math.ceil(ldpi / 10)

    function returnX(reference, target) {
        var globalCoordinares = reference.mapToItem(target, x, y)
        return globalCoordinares.x
    }

    function returnY(reference, target) {
        var globalCoordinares = reference.mapToItem(target, x, y)
        return globalCoordinares.y
    }

    FontLoader {
        id: openSansFontRegular
        source: "/OpenSans_Regular.ttf"
    }
}
