import QtQuick 2.0

import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import "qrc:/js/UI.js" as UI
TabButton{
    id: btn

    property int radius: 2//最大输入长度
    property string bdcolor: UI.cTBBorder
    property string bgcolor: UI.cMainBg
    property string color: UI.cBlack
    property int fontSize: UI.LittleFontPointSize
//    contentItem: Text {
//        text: btn.text
//        font: btn.font
//        opacity: enabled ? 1.0 : 0.3
//        color: btn.down ? UI.cBlack : UI.cLightBlue
//        horizontalAlignment: Text.AlignHCenter
//        verticalAlignment: Text.AlignVCenter
//        elide: Text.ElideRight
//    }

//    background: Rectangle {
//        implicitWidth: 100
//        implicitHeight: 40
//        opacity: enabled ? 1 : 0.3
//        color: UI.cTransparent
//        border.color: btn.down ? "#17a81a" : "#21be2b"
//        border.width: 0
//        radius: 0
//    }
}
