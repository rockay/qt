import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
//import QtWebView 1.1

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API

Window {
    id: imgWindow
    title: qsTr("圈图")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
    property string fileUrl: ""
    property ListModel imgshowList: imgList // 图片数组列表
    property int curIdx: -1
    color: UI.cTransparent
    onVisibleChanged: {
        if(imgWindow.visibility === Window.Maximized)
            fullScreen.source = "qrc:/images/icon/small.png"
        else
            fullScreen.source = "qrc:/images/icon/fullscreen.png"
    }

    ListModel{
        id: imgList
    }


    width: Screen.desktopAvailableWidth/2
    height: Screen.desktopAvailableHeight*3/5
    x: (Screen.desktopAvailableWidth-width)/2
    y: (Screen.desktopAvailableHeight-height)/2

    Rectangle{
        anchors.fill: parent
        color: UI.cBlack
        opacity: 0.5
    }

//    WebView {
//        id: webView
//        anchors.fill: parent
//        url: fileUrl
//    }
}
