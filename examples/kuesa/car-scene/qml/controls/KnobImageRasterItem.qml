
import QtQuick 2.11

Item {
    id: knobImageRasterItem
    visible: false
    width: height
    height:  Math.ceil(( SharedAttributes.ldpi / 2.8 - ( SharedAttributes.ldpi / 10)) / sFC )
    property var storedImage: undefined //now if I could tell image to use this
    Rectangle {
        id: handle
        anchors.verticalCenter: parent.verticalCenter
        anchors.fill: parent
        radius: height/2
        
        border.color: "#65ffffff"
        color:  "#16ffffff"
        border.width: Math.ceil( SharedAttributes.ldpi / 170)
    }
    
    Component.onCompleted: frrt.start()
    
    Timer {
        id: frrt
        interval: 50
        onTriggered: {knobImageRasterItem.grabToImage(function(result) {
            result.saveToFile("/tmp/knobImageRaster.png") ;
            storedImage = result
        } )
       }
    }
}

