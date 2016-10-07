import QtQuick 2.0

import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import "qrc:/js/UI.js" as UI

TabBar {
    id:tabbar
    spacing:1
    background: Rectangle {
        implicitWidth: 150
        implicitHeight: 40
        color: UI.cTabBg
//        border.width: 1
//        border.color: UI.cTabBg
    }
}
