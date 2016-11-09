/****************************************************
 * 名称：普通文字显示控件
 * 作者：LT
 * 日期：2016.10.1
 * 修改记录：
 *
 ****************************************************/

import QtQuick 2.0
import QtQuick.Controls 1.4
import "qrc:/js/UI.js" as UI

Text {
    id: txt
    property int pointSize: UI.LittleFontPointSize
    property color fcolor: UI.cBlack
    elide: Text.ElideRight
    font.family: UI.defaultFontFamily
    font.pointSize: txt.pointSize
    color: txt.fcolor
    verticalAlignment: Text.AlignVCenter
//    horizontalAlignment: Text.AlignHCenter

}
