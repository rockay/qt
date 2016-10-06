/****************************************************
 * 名称：单行文本输入框
 * 作者：LT
 * 日期：2016.10.3
 * 修改记录：
 *
 ****************************************************/

import QtQuick 2.6
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4

import "qrc:/js/UI.js" as UI
//Rectangle {
//     FocusScope {
//         id: focusScope
//         focus: true

//         TextInput {
//             id: input
//             focus: true
//         }
//     }
// }
//TextField {
//    id: firstName
//    placeholderText: qsTr("first name")
//}

//TextField {
//    id: control

//    background: Rectangle {
//        anchors.centerIn: parent
//        color: control.enabled ? "transparent" : "#353637"
//        border.color: control.enabled ?  UI.cTBBorder : "transparent"
//    }
//}
TextField{
    id: txtField
    font.pointSize: UI.TinyFontPointSize
    font.family: UI.defaultFontFamily
    property int borderwidth: 1
    property int radius: 2
    property string bordercolor: UI.cTBBorder
    style: TextFieldStyle {
        background: Rectangle {
            radius: txtField.radius
            border.color: txtField.bordercolor
            border.width: txtField.borderwidth
            color: UI.cTBBg
        }
        placeholderTextColor:UI.cFTB
    }
}
