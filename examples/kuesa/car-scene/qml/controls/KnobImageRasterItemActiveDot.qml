
import QtQuick 2.11

Item {
    id: root
    visible: false
    width: height
    height:  Math.ceil( (SharedAttributes.ldpi / 2.8 - ( SharedAttributes.ldpi / 10)) / sFC )
    
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
            anchors.margins: Math.ceil( (SharedAttributes.ldpi /170) / sFC )
            radius: height
            border.color: "#55000000"
            color: "transparent"
            border.width:  Math.ceil(SharedAttributes.ldpi /170)
        }
        
        Rectangle {
            anchors.fill: parent
            anchors.margins:  Math.ceil((SharedAttributes.ldpi /85)/ sFC)
            radius: height
            border.color: "#15000000"
            color: "transparent"
            border.width:  Math.ceil(SharedAttributes.ldpi /150)
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

