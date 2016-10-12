import QtQuick 2.7
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/login.js" as LoginJS

Window {
    id:login
    visible: true
    width: 472
    height: 623
    title: qsTr("登录")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
    color: UI.cTransparent


    Settings {
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
    }
    MouseArea {
        id: dragRegion
        anchors.fill: parent
        property point clickPos: "0,0"
        onPressed: {
            clickPos  = Qt.point(mouse.x,mouse.y)
        }

        onPositionChanged: {
            //鼠标偏移量
            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
            //如果mainwindow继承自QWidget,用setPos
            login.x = login.x+delta.x
            login.y = login.y+delta.y
        }
    }

    Rectangle{
        anchors.fill: parent;
        radius: 4;
        color: UI.cTransparent
        Image{
            anchors.fill: parent
            source: "qrc:/images/icon/login_bg.png"
        }

        LText{
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 10
            anchors.topMargin: 10
            text: qsTr("圈")
            font.pointSize: UI.LargeFontPointSize
            color: UI.cLoginQ
        }
    }

    //要置于MouseArea之后，否则无法响应鼠标点击
//    LOperButton {
//        id:closeBtn
//        height: UI.fWCloseButton+5
//        width: UI.fWCloseButton
//        anchors.right: parent.right
//        anchors.top: parent.top
//        source: "qrc:/images/icon/close.png";
//        MouseArea{
//            anchors.fill: parent
//            onClicked: {
//                Qt.quit()
//            }
//        }
//    }
    Rectangle{
        id:closeBtn
        height: UI.fHLoginClose
        width: height
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 10
        anchors.topMargin: 10
        color: UI.cTransparent
        Image {
            id: close
            anchors.centerIn: parent
            width: 20
            height: width
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/icon/close_login.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:
            {
                Qt.quit()
            }
            onEntered: {
                close.source = "qrc:/images/icon/close_loginp.png"
            }
            onExited: {
                close.source = "qrc:/images/icon/close_login.png"
            }
        }
    }

    Rectangle{
        anchors.left: parent.left
        anchors.top: parent.top
        width: UI.fHLoginM
        height: UI.fHLoginM
        radius: 4
        color: UI.cTransparent
//        Image{
//            anchors.fill: parent
//            fillMode: Image.PreserveAspectFit
//            source: "qrc:/images/icon/left.png"
//        }

//        LinearGradient {
//            anchors.fill: parent
//            start: Qt.point(4, 4)
//            end: Qt.point(100, 100)
//            gradient: Gradient {
//                GradientStop { position: 0.0; color: UI.cLoginLBg }
//                GradientStop { position: 1.0; color: UI.cLoginLBD }
//            }
//        }
//        LText{
//            anchors.left: parent.left
//            anchors.top: parent.top
//            text: qsTr("圈")
//            font.pointSize: UI.LargeFontPointSize
//            color: UI.cWhite
//        }

//        Canvas{
//            anchors.fill: parent
//            contextType: "2d";
//            onPaint: {
//                context.lineWidth = 2;
//                context.strokeStyle = UI.cLoginLBD;
//                context.fillStyle = UI.cLoginLBg;
//                context.beginPath();
//                context.moveTo(0 ,0);
//                context.lineTo(0 , UI.fHLoginM);
//                context.lineTo(UI.fHLoginM ,0);
//                context.closePath();
//                context.stroke();

//            }
//        }
    }

    Rectangle{
        id:photo
        width:parent.width/3
        height:width
        anchors.top: parent.top
        anchors.topMargin: UI.fHLoginM
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        Image {
            id: photop
            anchors.fill: parent
            source: API.user_photo
        }
    }

    Rectangle{
        id:name
        width:photo.width
        height:txtName.contentHeight
        anchors.top: photo.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        color: UI.cTransparent
        LText{
            id:txtName
            text: API.user_name//qsTr("郭思佳")
            anchors.centerIn: parent
        }
    }

    Rectangle{
        id:btnlogin
        color: UI.cLoginBtnBg
        width: parent.width*5/7
        height: UI.fHLoginBtn
        anchors.top: name.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        LText{
            id:txtLogin
            text:qsTr("登录")
            anchors.centerIn: parent
            color: UI.cWhite
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                LoginJS.sndVCode();
            }
        }
    }

    Rectangle{
        id:exchange
        width:photo.width
        height:title.contentHeight
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        color: UI.cTransparent
        LText{
            id:title
            text:qsTr("切换账号")
            anchors.centerIn: parent
            color: UI.cExchange
        }
    }
}
