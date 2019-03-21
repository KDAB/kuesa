/*
    generate_raster_assets.qml

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtQuick 2.11

Item {
    id: main

    property int currentLoop: 0
    readonly property var targetDPIs: [
        {"name": "ldpi", "value": 120 },
        {"name": "mdpi", "value" : 160},
        {"name": "hdpi", "value" : 240},
        {"name": "xhdpi", "value" : 320},
        {"name": "xxhdpi", "value" : 480},
        {"name": "xxxhdpi", "value" : 640}
    ]
    readonly property real dpi: targetDPIs[currentLoop].value
    readonly property string dpiName: targetDPIs[currentLoop].name

    BgImageRasterItem {
        id: bgImageRasterItem
        dpi: main.dpi
        dpiName: main.dpiName
    }

    KnobImageRasterItem {
        id: knobImageRasterItem
        dpi: main.dpi
        dpiName: main.dpiName
    }

    KnobImageRasterItemActive {
        id: knobImageRasterItemActive
        dpi: main.dpi
        dpiName: main.dpiName
    }

    KnobImageRasterItemDot {
        id: knobImageRasterItemDot
        dpi: main.dpi
        dpiName: main.dpiName
    }

    KnobImageRasterItemActiveDot {
        id: knobImageRasterItemActiveDot
        dpi: main.dpi
        dpiName: main.dpiName
    }

    SequentialAnimation {
        id: grabAnimation

        PauseAnimation { duration: 1000 }
        ScriptAction {
            script: {
                bgImageRasterItem.grabImage()
                knobImageRasterItem.grabImage()
                knobImageRasterItemActive.grabImage()
                knobImageRasterItemDot.grabImage()
                knobImageRasterItemActiveDot.grabImage()
            }
        }
        PauseAnimation { duration: 1000 }
        ScriptAction {
            script: {
                if (currentLoop < targetDPIs.length - 1)
                    currentLoop += 1
            }
        }

        loops: targetDPIs.length
        onStopped: Qt.quit();
    }

    Component.onCompleted: {
        grabAnimation.start()
    }
}
