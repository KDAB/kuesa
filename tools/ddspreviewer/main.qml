/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Scene3D 2.0
import QtQuick.Dialogs 1.2
import DDSPreviewer 1.0

Page {
    id: mainRoot
    property string filePath: _ddsFilePath

    header: ToolBar {
        Flow {
            ToolButton {
                text: "Open"
                onClicked: fileDialog.open()
                focusPolicy: Qt.NoFocus
            }
            ToolSeparator {}
            ButtonGroup { id: modeButtonGroup }
            ToolButton {
                text: "Plane"
                onToggled: texturePreview.mode = texturePreview._PLANE_MODE
                focusPolicy: Qt.NoFocus
                ButtonGroup.group: modeButtonGroup
                checkable: true
                checked: texturePreview.mode === texturePreview._PLANE_MODE
            }
            ToolButton {
                text: "SkyBox"
                onToggled: texturePreview.mode = texturePreview._SKYBOX_MODE
                focusPolicy: Qt.NoFocus
                ButtonGroup.group: modeButtonGroup
                checkable: true
                checked: texturePreview.mode === texturePreview._SKYBOX_MODE
            }
        }
    }

    DDSHeaderParser {
        id: ddsParser
        filePath: mainRoot.filePath
    }

    Scene3D {
        anchors.fill: parent
        focus: true
        aspects: ["render", "input", "logic"]
        TexturePreview {
            id: texturePreview
            filePath: mainRoot.filePath
        }
    }

    FileDialog {
        id: fileDialog
        onAccepted: mainRoot.filePath = fileUrl
    }

    Column {
        Row {
            Label {
                text: "Exposure: "
                anchors.verticalCenter: slider.verticalCenter
            }
            Slider {
                id: slider
                from: -5
                to: 5
                value: 2.5
                onMoved: texturePreview.exposure = value
            }
        }
        Label {
            text: ddsParser.ddsHeader
        }
    }
}
