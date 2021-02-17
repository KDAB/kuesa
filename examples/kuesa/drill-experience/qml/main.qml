/*
    main.qml

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
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Drill 1.0

//! [0]
ApplicationWindow {
    id: mainRoot
    title: "Drill-Experience"
    visible: true
    visibility: ApplicationWindow.Maximized

    readonly property int _DRILL_STATUS_SCREEN: 0
    readonly property int _GUIDED_DRILLING_SCREEN: 1
    readonly property int _USER_MANUAL_SCREEN: 2

    Material.theme: Material.Dark
    Material.accent: Material.Blue

    menuBar: ToolBar {
        Label {
            text: mainRoot.title
            font.pixelSize: 20
            anchors.centerIn: parent
        }
    }

    header: TabBar {
        TabButton { text: "Drill Status" }
        TabButton { text: "Guided Drilling" }
        TabButton { text: "User Manual" }
    }

    //! [1]
    Item {
        id: contentItem
        anchors.fill: parent


        //! [2]
        DrillScene {
            id: view3D
            anchors.fill: parent
            screen: header.currentIndex
            onScreenChanged: opacityAnimation.restart()

            SequentialAnimation on opacity {
                id: opacityAnimation
                running: false
                OpacityAnimator { from: 1; to: 0; duration: 450; easing.type: Easing.OutQuad }
                OpacityAnimator { from: 0; to: 1; duration: 450; easing.type: Easing.InQuad }
            }
        }
        //! [2]

        //! [3.1]
        Component {
            id: statusUI
            StatusUI {
                batteryLife: view3D.statusScreenController.drillStatus.batteryLife
                torque: view3D.statusScreenController.drillStatus.torque
                rpm: view3D.statusScreenController.drillStatus.rpm
                mode: view3D.statusScreenController.drillStatus.mode
                direction: view3D.statusScreenController.drillStatus.direction
            }
        }
        Component {
            id: guidedDrillingUI
            GuidedDrillingUI {

            }
        }
        Component {
            id: userManualUI
            UserManualUI {

            }
        }
        //! [3.1]

        //! [3.2]
        Loader {
            id: uiLoader
            anchors.fill: parent
            readonly property var sources: [statusUI, guidedDrillingUI, userManualUI]
            sourceComponent: sources[header.currentIndex]
        }
        //! [3.2]
    }
    //! [1]
}
//! [0]
