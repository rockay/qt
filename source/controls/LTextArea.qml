import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import "qrc:/js/UI.js" as UI
TextArea{
    id:txtControl
    font.family: UI.emojiFont
    font.pointSize: UI.LittleFontPointSize
    wrapMode: TextEdit.Wrap
//    property int maxLength: 200//最大输入长度
//    onLengthChanged:
//    {
//        if(text.length > maxLength)
//        {
//            var prePosition = cursorPosition;
//            text = text.substring(0, maxLength);
//            cursorPosition = Math.min(prePosition, maxLength);
//        }
//    }
    background: Rectangle {
        anchors.centerIn: parent
        color: txtControl.enabled ? "transparent" : "#353637"
        border.color: txtControl.enabled ?  UI.cTBBorder : "transparent"
    }
}
