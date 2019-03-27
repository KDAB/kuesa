/*
    main.qml

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

import QtQuick.Scene3D 2.0
import QtQuick 2.11
import QtQuick.Controls 1.4
import "controls" as Controls

Item {
    id: mainRoot

    // 3D Content

    Item {
        id: baseUI
        anchors.fill: parent

        Wind {
            id: wind
            anchors.fill: parent
            visible: menu.useOpacityMask
        }

        Scene3D {
            id: scene3D
            anchors.fill: parent
            focus: true
            multisample: true
            aspects: ["input", "animation", "logic"]

            // Root Scene Entity
            MainScene {
                id: sceneContent
                screenWidth: scene3D.width
                screenHeight: scene3D.height
                animated: menu.idleAnimationRunning
                showSkybox: menu.showSkybox
                environmentMap: menu.environmentMap
                environmentExposure: menu.environmentExposure
                exposure: menu.exposure
                carSpeed: menu.carSpeed
                useOpacityMask: menu.useOpacityMask
                openLeftDoor: menu.openLeftDoor
                openRightDoor: menu.openRightDoor
                openHood: menu.openHood
            }

            Keys.onPressed: {
                if (event.modifiers & Qt.ControlModifier) {
                    // Ctrl+F to toggle fullscreen
                    if (event.key === Qt.Key_F) {
                        _isFullScreen = !_isFullScreen

                        if (_isFullScreen)
                            _view.showFullScreen()
                        else
                            _view.show()
                    } else if (event.key === Qt.Key_C) {
                        // Ctrl+C to toggle cursor
                        mainMouseArea.cursorShape = mainMouseArea.cursorShape === Qt.BlankCursor ? Qt.ArrowCursor : Qt.BlankCursor
                    }
                }
            }
        }
    }

    // UI menu container
    MainMenu {
        id: menu
        anchors.fill: parent
    }

    // Logos
    Image {
        id: kdabLogoBottomRight
        width: parent.width/13
        smooth: true
        antialiasing: true
        source: "kdab_logo_white.png"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: width/4
        anchors.rightMargin: width/4
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onPressAndHold: menu.idleAnimationRunning = true
        }
    }

    Image {
        id: qtLogo
        property bool kuesaLogo: true
        anchors.verticalCenter: kdabLogoBottomRight.verticalCenter
        anchors.right: kdabLogoBottomRight.left
        anchors.rightMargin: kdabLogoBottomRight.anchors.rightMargin/1.3
        width: parent.width/15* (kuesaLogo ? 2.3 : 1.8)
        smooth: true
        antialiasing: true
        source: kuesaLogo ? "Kuesa-logo-black.png" : "Qt-logo.png"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onPressAndHold: qtLogo.kuesaLogo = !qtLogo.kuesaLogo
        }
    }
}
