/****************************************************
 * 名称：登录透明的文本输入框
 * 作者：LT
 * 日期：2016.10.3
 * 修改记录：
 *
 ****************************************************/

import QtQuick 2.6
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4

import "qrc:/js/UI.js" as UI
TextField{
    id: txtField
    font.pointSize: UI.LittleFontPointSize
    font.family: UI.defaultFontFamily
    property int borderwidth: 1
    property int radius: 2
    property string bordercolor: UI.cTBBorder
    style: TextFieldStyle {
        background: Rectangle {
            border.width: 0
            color: UI.cTransparent
        }
        placeholderTextColor:UI.cFTB
    }
}
