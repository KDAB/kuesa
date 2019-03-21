
import QtQuick 2.11

Item {
    id: bgImageRasterItem
    visible: false
    width: height+2
    height: Math.ceil( SharedAttributes.ldpi / 2.8)
    Rectangle {
        id: mask
        anchors.fill: parent
        anchors.margins: 1
        radius: height/2
        color: "#33000000"

        
        // visual items shadows and edges
        Item {
            anchors.fill: parent
            
            Repeater {
                model: [ "#65000000", "#20000000", "#10000000" ]
                delegate: Rectangle {
                    anchors.fill: parent
                    anchors.margins: (model.index + 1)*Math.ceil( SharedAttributes.ldpi / 150)
                    radius: height
                    border.color: modelData
                    color: "transparent"
                    border.width: Math.ceil( SharedAttributes.ldpi / 150)
                }
            }
            
            Rectangle {
                anchors.fill: parent
                border.color: "#65ffffff"
                color: "#00000000"
                radius: height
                border.width: Math.ceil( SharedAttributes.ldpi / 150)
            }
        }
        
        Component.onCompleted: frrt.start()
        
        Timer {
            id: frrt
            interval: 50
            onTriggered: {bgImageRasterItem.grabToImage(function(result) {
                result.saveToFile("/tmp/bgImageraster.png") } )
            }
        }
    }
}
