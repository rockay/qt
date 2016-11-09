import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "qrc:/js/UI.js" as UI
Button{
    id: btn
    property int radius: 5
    property string bdcolor: UI.cTBBorder
    property string bgcolor: UI.cMainBg
    property string color: UI.cBlack
    property int fontSize: UI.LittleFontPointSize
    style: ButtonStyle {
        background: Rectangle {
            implicitWidth: 100
            implicitHeight: 25
            border.width: control.activeFocus ? 2 : 1
            border.color: btn.bdcolor
            radius: btn.radius
            color:  control.pressed ? btn.bdcolor : btn.bgcolor
        }
        label: LText{
            text: btn.text
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            color: btn.color
            font.family: UI.defaultFontFamily
            font.pointSize: btn.fontSize
        }
    }
}
