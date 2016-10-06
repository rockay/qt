import QtQuick 2.7
import QtQuick.Window 2.2

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
        color: UI.cLoginBg
    }

    //要置于MouseArea之后，否则无法响应鼠标点击
    LOperButton {
        id:closeBtn
        height: UI.fWCloseButton+5
        width: UI.fWCloseButton
        anchors.right: parent.right
        anchors.top: parent.top
        source: "qrc:/images/close.png";
        MouseArea{
            anchors.fill: parent
            onClicked: {
                Qt.quit()
            }
        }
    }
//    Rectangle{
//        id:closeBtn
//        height: 43
//        width: 43
//        anchors.right: parent.right
//        anchors.top: parent.top
//        color: UI.cTransparent
//        Image {
//            id: close
//            anchors.centerIn: parent
//            source: "qrc:/images/close.png"
//        }
//        MouseArea{
//            anchors.fill: parent
//            onClicked:
//            {
//                Qt.quit()
//            }
//        }
//    }

    Rectangle{
        id:photo
        width:parent.width/3
        height:width
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        Image {
            id: photop
            anchors.fill: parent
            source: "qrc:/images/photo.png"
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
            text:qsTr("郭思佳")
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