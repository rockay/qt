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
    property int fontSize: UI.SmallFontPointSize
    contentItem: LText {
        text: btn.text
        font.family: UI.defaultFontFamily
        font.pointSize: UI.LittleFontPointSize
        color: btn.checked ? UI.cTabCheckedF : UI.cBlack
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        color: UI.cMainCBg
        border.color: btn.checked ? UI.cTBBorder : UI.cTransparent
        border.width: 0
        radius: 0
        Rectangle{
            width: parent.width
            height:2
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            color: btn.checked ? UI.cTabCheckedF : UI.cTabUncheckedB
        }
    }
}
