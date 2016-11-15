import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import Qt.labs.settings 1.0


import "qrc:/controls/"
import "qrc:/qml/login"
import "qrc:/qml/message"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/login.js" as LoginJS

QtObject {
    property real defaultSpacing: 10
    property SystemPalette palette: SystemPalette { }
    property var set: Settings{
        id: settings
        property string token: ""
        property string rong_token: ""
        property string weix_token: ""
        property string user_id: ""
        property string user_mobile: ""
        property string user_name: ""
        property string user_wx_name: ""
        property string user_photo: ""
        property string is_vip_user: ""
        property string is_company_user: ""
        property string code: ""
    }

    property var lWindow:Login {
        id: login
        visible: false
        width: 400 //280
        height: 560 //400
    }
    property var mWindow:MainWindow {
        id: main
        width: UI.fWMain
        height: UI.fHMain
        minimumWidth: UI.fWMain
        minimumHeight: UI.fHMain
        visible: false
   }
    property var iWindow:ImageShow {
        id: imageshow
        visible: false
   }

    property var twindow: Window{
        id: ppFace
        visible: false
        flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
               | Qt.WindowMinimizeButtonHint| Qt.Popup
        property LFace biaoq: rootface
        width: 35*13+5;
        height: 35*10
        onActiveChanged: {
            if (!active) {
                ppFace.hide();
            }
        }
        LFace{
            id: rootface
            visible: true
            anchors.fill: parent
        }
    }

//    property var iWindow:ImageEdit {
//        id: imageshow
//        visible: false
//        Component.onCompleted: {
//            console.log("MainWindow loaded...")
//        }
//   }

    property var splashWindow: Splash {
        onTimeout: {
            lWindow.requestActivate();
            lWindow.visible = true
        }
    }
}
