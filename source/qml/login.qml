import QtQuick 2.7
import QtQuick.Window 2.2

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/login.js" as LoginJS

Window {
    id:root
    visible: true
    width: 472
    height: 623
    title: qsTr("Hello World")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
    color:"#00000000"


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
            root.x = root.x+delta.x
            root.y = root.y+delta.y
        }
    }

    Rectangle{
        anchors.fill: parent;
        radius: 10;
        color: UI.cWhite
    }

    //要置于MouseArea之后，否则无法响应鼠标点击
    Rectangle{
        id:closeBtn
        height: 43
        width: 43
        anchors.right: parent.right
//        anchors.rightMargin: 5
        anchors.top: parent.top
//        anchors.topMargin: 5
        opacity: 1
        Image {
            id: close
            anchors.fill: parent
            source: "qrc:/images/close.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:
            {
                //Qt.quit()无法关闭窗口
                Qt.quit()
            }
        }
    }

    Rectangle{
        id:photo
        width:150
        height:150
//        color: UI.cLightBlue
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        Image {
            id: photop
            anchors.fill: parent
            source: "qrc:/images/logo.png"
        }
    }

    Rectangle{
        id:name
        width:photo.width
        height:txtName.contentHeight
        anchors.top: photo.bottom
        anchors.topMargin: 55
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        LText{
            id:txtName
            text:qsTr("郭思佳")
            anchors.centerIn: parent
        }
    }

    Rectangle{
        id:btnlogin
        color: UI.cLoginBg
        width:293
        height:57
        anchors.top: name.bottom
        anchors.topMargin: 55
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
        anchors.bottomMargin: 60
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        LText{
            id:title
            text:qsTr("切换账号")
            anchors.centerIn: parent
            color: UI.cExchange
        }
    }

}
