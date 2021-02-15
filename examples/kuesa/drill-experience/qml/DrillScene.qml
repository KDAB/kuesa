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
    asynchronous: true
    backgroundColor: "#666"

    property int screen: 0

    //! [1]
    readonly property AbstractScreenController controller: {
        if (screen === _DRILL_STATUS_SCREEN)
            return statusScreenController
        if (screen === _GUIDED_DRILLING_SCREEN)
            return guidedDrillingScreenController
        // _USER_MANUAL_SCREEN
        return userManualScreenController
    }

    // We rely on each controller providing the scene configuration
    // This provides the source, camera, trackers, animation players...
    activeScene: controller.sceneConfiguration

    // Controllers
    // Properties to expose controllers for external access
    readonly property StatusScreenController statusScreenController: _statusScreenController
    readonly property UserManualScreenController userManualScreenController: _userManualScreenController
    readonly property GuidedDrillingScreenController guidedDrillingScreenController: _guidedDrillingScreenController

    StatusScreenController {
        id: _statusScreenController
        isActive: screen === _DRILL_STATUS_SCREEN
    }

    UserManualScreenController {
        id: _userManualScreenController
        isActive: screen === _USER_MANUAL_SCREEN
    }

    GuidedDrillingScreenController {
        id: _guidedDrillingScreenController
        isActive: screen === _GUIDED_DRILLING_SCREEN
    }
    //! [1]
}
//! [0]
