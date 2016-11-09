import QtQuick 2.0
import QtQuick.Controls 2.0

import "qrc:/js/UI.js" as UI

MenuItem {
    id: mitemroot
    font.family: UI.defaultFontFamily
    property string bgcolor : UI.cTransparent
    property bool trigger: false
    background: Rectangle{
        focus: false
        x: 2
        y: 2
        width: parent.width-4
        height: parent.height-4
        anchors.centerIn: parent
        color: mitemroot.down ? UI.cLightBlue : UI.cTransparent
//        color: mitemroot.hovered ? UI.cLightBlue : UI.cTransparent
    }
    leftPadding: 10
}

