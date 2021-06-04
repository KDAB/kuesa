/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Andrew Hayzen <andrew.hayzen@kdab.com>

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
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Kuesa.Utils 1.3 as KuesaUtils

Item {
    KuesaUtils.View3D {
        id: scene3D

        readonly property QtObject picker: QtObject {
            property Entity entity: null
            readonly property string entityName: entity && entity.parent ? entity.parent.objectName : null
            readonly property var groups: ["Cube1", "Cube2"]
            readonly property string groupName: {
                for (let entity = scene3D.picker.entity; entity !== null; entity = entity.parent) {
                    if (entity.objectName !== "") {
                        if (groups.indexOf(entity.objectName) !== -1) {
                            return entity.objectName;
                        }
                    }
                }

                return "";
            }
        }

        anchors.fill: parent
        camera: "Cam"
        components: [
            ObjectPicker {
                onClicked: scene3D.picker.entity = pick.entity
            }
        ]
        source: "qrc:/assets/picking.gltf"
    }

    Column {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 10
        anchors.top: parent.top

        Text {
            text: qsTr("Picked: %1").arg(scene3D.picker.entityName || qsTr("None"))
        }

        Text {
            text: qsTr("Group: %1").arg(scene3D.picker.groupName || qsTr("None"))
        }
    }
}
