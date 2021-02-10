/*
    DrillScene.qml

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

import Kuesa 1.3
import Kuesa.Utils 1.3
import Drill 1.0

//! [0]
View3D {
    id: view3D
    focus: true
    source: "qrc:/drill/drill.gltf"
    camera: "|CamCenter|OrbitCam"

    //! [1]
    StatusScreenController {
        id: statusScreenController

        drillStatus.onRpmChanged: console.log("RPM " + drillStatus.rpm)
        drillStatus.onTorqueChanged: console.log("Torque " + drillStatus.torque)
        drillStatus.onCurrentDrawChanged: console.log("Current Draw " + drillStatus.currentDraw)
        drillStatus.onBatteryLifeChanged: console.log("BatteryLife " + drillStatus.batteryLife)
    }

    UserManualScreenController {
        id: userManualScreenController
    }

    GuidedDrillingScreenController {
        id: guidedDrillingScreenController
    }
    //! [1]
}
//! [0]
