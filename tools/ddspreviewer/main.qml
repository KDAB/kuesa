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
