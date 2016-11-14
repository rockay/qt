import QtQuick 2.0
import QtQuick.Controls 2.0
import "qrc:/js/UI.js" as UI

Menu {
    id: item
    focus: true
    background: Rectangle{
        color: UI.cWhite
        border.width: 1
        border.color: UI.cRightBg
        radius: 2
    }
}
