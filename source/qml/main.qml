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

    property var controlWindow:Login {
        id:login
        visible: true
        width: 280
        height: 400
    }
    property var testWindow:MainWindow {
        id:main
        width: 850
        height: 600
        visible: false
   }
}
