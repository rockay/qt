import QtQuick 2.0
import "qrc:/js/UI.js" as UI

Rectangle{
    id:root
    property string source:""
    color: UI.cMainBg
    Image {
        width: 9
        height: 10
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        source: parent.source
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            root.color = UI.cOperActBg
        }
        onExited: {
            root.color = UI.cMainBg
        }
    }
}
