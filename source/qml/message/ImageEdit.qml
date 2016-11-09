import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.0

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API

Window {
    id: imgWindow
    title: qsTr("圈图")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
    property string imgSrc: ""
    property ListModel imgshowList: imgList // 图片数组列表
    property int curIdx: -1
    width: 800
    height: 600

    ListModel{
        id: imgList
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        property real lastX
        property real lastY
        property color color: UI.cLightBlue
        onPaint: {
            var ctx = getContext('2d')
            ctx.drawImage("file:///C:\\Users\\Administrator\\Pictures\\1012332\\1.jpg",0,0)
            ctx.lineWidth = 1.5
            ctx.strokeStyle = canvas.color
            ctx.beginPath()
            ctx.moveTo(lastX, lastY)
            lastX = area.mouseX
            lastY = area.mouseY
            ctx.lineTo(lastX, lastY)
            ctx.stroke()
        }
        MouseArea {
            id: area
            anchors.fill: parent
            onPressed: {
                canvas.lastX = mouseX
                canvas.lastY = mouseY
            }
            onPositionChanged: {
                canvas.requestPaint()
            }
        }
    }

    Rectangle{
        id: headRect
        width: parent.width
        height: closeBtn.height
        anchors.left: parent.left
        anchors.top: parent.top
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
                imgWindow.x = imgWindow.x+delta.x
                imgWindow.y = imgWindow.y+delta.y
            }
        }
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
                width: 40
                height: width
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/icon/close_login.png"
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked:
                {
                    imageshow.hide()
                }
                onEntered: {
                    close.source = "qrc:/images/icon/close_loginp.png"
                }
                onExited: {
                    close.source = "qrc:/images/icon/close_login.png"
                }
            }
        }
    }
}
