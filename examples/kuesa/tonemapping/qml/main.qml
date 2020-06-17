/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Robert Brock <robert.brock@kdab.com>

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

import QtQuick 2.12
import QtQuick.Scene3D 2.12
import QtQuick.Controls.Material 2.12

//! [0]
Item {
    id: root

    Item {
        id: baseUI
        anchors.fill: parent

        Scene3D {
            id: scene3d
            anchors.fill: parent
            focus: true
            multisample: true
            aspects: ["render", "input", "logic", "animation"]

            MainScene {
                id: sceneEntity
                screenWidth: scene3d.width
                screenHeight: scene3d.height
                exposure: menu.exposure
                rotating: menu.toggleRotation
                lightRotating: menu.toggleLightRotation
                toneMappingAlgorithmName: menu.toneMappingAlgorithmName
            }
        }
    }
//! [0]

    // UI menu container
    MainMenu {
        id: menu
        anchors.fill: parent
    }

    // Logos
    Image {
        id: kdabLogoBottomRight
        width: parent.width/13
        mipmap: true
        smooth: true
        antialiasing: true

        source: "/kdab_logo_white.png"
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
        mipmap: true
        smooth: true
        antialiasing: true
        source: kuesaLogo ? "/Kuesa-logo-black.png" : "/Qt-logo.png"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onPressAndHold: qtLogo.kuesaLogo = !qtLogo.kuesaLogo
        }
    }

}
