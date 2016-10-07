import QtQuick 2.7
import QtQuick.Window 2.2

import "qrc:/controls/"
import "qrc:/qml/login"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/login.js" as LoginJS

QtObject {
    property real defaultSpacing: 10
    property SystemPalette palette: SystemPalette { }

    property var lWindow:Login {
        id:login
        visible: true
        width: 280
        height: 400
    }
    property var mWindow:MainWindow {
        id:main
        width: UI.fWMain
        height: UI.fHMain
        minimumWidth: UI.fWMain
        minimumHeight: UI.fHMain
        visible: false
   }
}
