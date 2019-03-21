
import QtQuick 2.11

Item {
    id: knobImageRasterItemActive
    visible: false
    width: height
    height:  Math.ceil( SharedAttributes.ldpi / 2.8 - ( SharedAttributes.ldpi / 10) )

    Rectangle {
        id: handle
        anchors.verticalCenter: parent.verticalCenter
        anchors.fill: parent
        radius: height/2

        border.color: "#3996ff"
        color: "#603996ff"
        border.width:  SharedAttributes.ldpi / 90
    }

        Component.onCompleted: frrt.start()
        
        Timer {
            id: frrt
            interval: 50
            onTriggered: {knobImageRasterItemActive.grabToImage(function(result) {
                result.saveToFile("/tmp/knobImageRasterActive.png") } )
            }
        }
    }

