import QtQuick 2.12

Row {
    spacing: SharedAttributes.defaultSpacing

    property alias text: label.text
    property alias checked: control.checked
    property alias exclusiveGroup: control.exclusiveGroup

    StyledSwitch {
        id: control
        width: height
        radio: true
    }

    StyledLabel {
        id: label
        text: "Reinhard"
        anchors.verticalCenter: parent.verticalCenter
    }
}
