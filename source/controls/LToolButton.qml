/*******************
 *
 *
 *******************/
import QtQuick 2.0
import "qrc:/js/UI.js" as UI

Rectangle{
    id:root
    property string source:""
    color: UI.cMainLBg
    Image {
        width: parent.width/2
        height: parent.height/2
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        source: parent.source
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
//            root.color = UI.cOperActBg
        }
        onExited: {
            root.color = UI.cMainLBg
        }
    }
}
