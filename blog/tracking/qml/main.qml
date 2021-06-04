/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Andrew Hayzen <abdrew.hayzen@kdab.com>

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
import Qt3D.Render 2.12
import Kuesa 1.3 as Kuesa
import Kuesa.Utils 1.3 as KuesaUtils

Item {
    KuesaUtils.View3D {
        id: scene3D

        anchors.fill: parent
        camera: "Cam"
        source: "qrc:/assets/tracking.gltf"
        transformTrackers: [
            Kuesa.TransformTracker {
                id: tracker1
                name: "Center.Cube1"
            },
            Kuesa.TransformTracker {
                id: tracker2
                name: "Center.Cube2"
            },
            Kuesa.TransformTracker {
                id: tracker3
                name: "Center.Cube3"
            }
        ]

        Kuesa.AnimationPlayer {
            clip: "AnimRotate"
            loops: Kuesa.AnimationPlayer.Infinite
            running: true
            sceneEntity: scene3D.scene
        }
    }

    Text {
        readonly property point position: tracker1.screenPosition

        text: qsTr("Cube1")
        x: position.x - (implicitWidth * 0.5)
        y: position.y - implicitHeight
    }

    Text {
        readonly property point position: tracker2.screenPosition

        text: qsTr("Cube2")
        x: position.x - (implicitWidth * 0.5)
        y: position.y - implicitHeight
    }

    Text {
        readonly property point position: tracker3.screenPosition

        text: qsTr("Cube3")
        x: position.x - (implicitWidth * 0.5)
        y: position.y - implicitHeight
    }
}

