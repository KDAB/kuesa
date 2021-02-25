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

    //! [0.1]
    header: TabBar {
        TabButton { text: "Drill Status" }
        TabButton { text: "Guided Drilling" }
        TabButton { text: "User Manual" }
    }
    //! [0.1]

    //! [1]
    Item {
        id: contentItem
        anchors.fill: parent

        MouseArea {
            enabled: !view3D.activeFocus
            anchors.fill: parent
            onClicked: view3D.forceActiveFocus();
        }

        readonly property int headerIndex: header.currentIndex
        onHeaderIndexChanged: screenSwitchAnimation.restart()

        SequentialAnimation {
            id: screenSwitchAnimation
            running: false
            OpacityAnimator { target: contentItem; from: 1; to: 0; duration: 450; easing.type: Easing.OutQuad }
            ScriptAction { script: view3D.screen = contentItem.headerIndex }
            OpacityAnimator { target: contentItem; from: 0; to: 1; duration: 850; easing.type: Easing.InQuad }
        }

        //! [2]
        DrillScene {
            id: view3D
        }
        //! [2]

        //! [3.1]
        Component {
            id: statusUI
            StatusUI {
                controller: view3D.statusScreenController
            }
        }
        Component {
            id: guidedDrillingUI
            GuidedDrillingUI {
                controller: view3D.guidedDrillingScreenController
            }
        }
        Component {
            id: userManualUI
            UserManualUI {
                controller: view3D.userManualScreenController
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

        //! [3.3]
        states: [
            State {
                when: view3D.screen === _DRILL_STATUS_SCREEN
                AnchorChanges {
                    target: view3D
                    anchors {
                        top: contentItem.top; bottom: contentItem.bottom
                        left:contentItem.left; right: contentItem.right
                    }
                }
            },
            State {
                when: view3D.screen === _GUIDED_DRILLING_SCREEN
                AnchorChanges {
                    target: view3D
                    anchors {
                        top: contentItem.top; bottom: contentItem.bottom
                        left:contentItem.horizontalCenter; right: contentItem.right
                    }
                }
            },
            State {
                when: view3D.screen === _USER_MANUAL_SCREEN
                AnchorChanges {
                    target: view3D
                    anchors {
                        top: contentItem.top; bottom: contentItem.bottom
                        left:contentItem.left; right: contentItem.right
                    }
                }
            }
        ]
        //! [3.3]
    }
    //! [1]
}
//! [0]
