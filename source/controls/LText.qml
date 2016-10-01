import QtQuick 2.0
import QtQuick.Controls 1.2
import "qrc:/js/UI.js" as UI

Text {
    id: txt
    property int pointSize: UI.LittleFontPointSize
    property color fcolor: UI.cBlack
    font.family: UI.defaultFontFamily
    font.pointSize: txt.pointSize
    color: txt.fcolor
    verticalAlignment: Text.AlignVCenter

}
