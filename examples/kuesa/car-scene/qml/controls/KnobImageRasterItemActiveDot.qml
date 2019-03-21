
import QtQuick 2.11

Item {
    id: root
    visible: false
    width: height
    height:  Math.ceil( SharedAttributes.ldpi / 2.8 - ( SharedAttributes.ldpi / 10) )

    Rectangle {
        width: parent.height/2.5
        height: width
        radius: width/2
        anchors.centerIn: parent
        color: "#90000000"
        border.color:  "#3996ff"
        border.width:  SharedAttributes.ldpi / 90

        Rectangle {
            anchors.fill: parent
            anchors.margins: SharedAttributes.ldpi /150
            radius: height
            border.color: "#55000000"
            color: "transparent"
            border.width:  SharedAttributes.ldpi /150
        }

        Rectangle {
            anchors.fill: parent
            anchors.margins: SharedAttributes.ldpi /75
            radius: height
            border.color: "#15000000"
            color: "transparent"
            border.width:  SharedAttributes.ldpi /150
        }
    }

        Component.onCompleted: frrt.start()
        
        Timer {
            id: frrt
            interval: 50
            onTriggered: {root.grabToImage(function(result) {
                result.saveToFile("/tmp/knobImageRasterActiveDot.png") } )
            }
        }
    }

