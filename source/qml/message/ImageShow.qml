import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API

Window {
    id: imgWindow
    title: qsTr("圈图")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
//    modality:  Qt.ApplicationModal
    property string imgSrc: ""
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
    FileDialog {
        id: saveDialog
        folder: shortcuts.pictures
        selectExisting: false
        property string httpurl: ""
        property string file_ext: "pdf"
        onAccepted: {
            var urlNoProtocol = (fileUrl+"."+file_ext).replace('file:///', '');
            if(httpurl!=""){
                networkControl.doDownload(httpurl,urlNoProtocol);
            }
        }
    }

    onCurIdxChanged: {
        btnPre.enabled = true;
        btnNext.enabled = true;
        if(curIdx==(imgshowList.count-1)){
            btnNext.enabled = false;
        }
        if(curIdx==0)
            btnPre.enabled = false;
    }

    onImgshowListChanged: {
        btnPre.enabled = true;
        btnNext.enabled = true;
        if(curIdx==imgshowList.count-1){
            btnNext.enabled = false;
        }
        if(curIdx==0)
            btnPre.enabled = false;
    }

//    width: Math.min(Math.max(showimage.sourceSize.width,400),Screen.desktopAvailableWidth)
//    height: Math.min(Math.max(showimage.sourceSize.height,400),Screen.desktopAvailableHeight)
    width: Screen.desktopAvailableWidth/2
    height: Screen.desktopAvailableHeight*3/5
    x: (Screen.desktopAvailableWidth-width)/2
    y: (Screen.desktopAvailableHeight-height)/2

    Rectangle{
        anchors.fill: parent
        color: UI.cBlack
        opacity: 0.5
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                btnPre.visible = true;
                btnNext.visible = true;
                bottomTool.visible = true
            }

            onExited: {
                btnPre.visible = false;
                btnNext.visible = false;
                bottomTool.visible = false
            }
        }
    }

    Rectangle{
        id: imgrect
        color: UI.cTransparent
        width: imgWindow.width
        height: imgWindow.height
        anchors.centerIn: parent
        Flickable {
            id: flickable
            width: parent.width
            height: parent.height
            anchors.centerIn: parent
            contentWidth: showimage.width; contentHeight: showimage.height

            Image{
                id: showimage
                width:  Math.min(flickable.width,showimage.sourceSize.width)
                height:  Math.min(flickable.height,showimage.sourceSize.height)
                source: imgWindow.imgSrc
                fillMode: Image.PreserveAspectFit
                y: (parent.height-height)/2
                x: (parent.width-width)/2
                rotation: 0
                onProgressChanged: {
                    var proc = (progress + "").substring(0,3) * 100;
                    console.log("proc:"+proc);
                    imgProcess.text = qsTr(proc+"%");
                    imgProcess.visible = true;
                    if(progress == 1 || progress == 0)
                        imgProcess.visible = false;
                }

                onSourceSizeChanged:  {
                    showimage.width = Math.min(flickable.width,showimage.sourceSize.width)
                    showimage.height = Math.min(flickable.height,showimage.sourceSize.height)
                    showimage.y = (flickable.height-showimage.height)/2
                    showimage.x = (flickable.width-showimage.width)/2
                }

            }

        }

        LText{
            id: imgProcess
            color: UI.cWhite
            pointSize: UI.HugeFontPointSize
            anchors.centerIn: parent
        }


        MouseArea{
            anchors.fill: parent
            onWheel: {
//                console.log("onWheel:"+wheel.angleDelta.y / 120); // now it works
                //                        if (wheel.modifiers & Qt.ControlModifier) {
                var percent = (wheel.angleDelta.y / 120)/10;
                showimage.width *= (1+percent);
                showimage.height *= (1+percent);
                showimage.y = Math.max((flickable.height-showimage.height)/2,0) // 防止负数
                showimage.x =  Math.max((flickable.width-showimage.width)/2,0)
                //                        }
            }
            propagateComposedEvents: true
            onClicked: { mouse.accepted = false }
            onPressed: { mouse.accepted = false }
        }
    }

    Image{
        id: btnPre
        width: UI.fWNextBtn
        height: width
        source: "qrc:/images/icon/pre.png"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: (parent.height-height)/2
        fillMode: Image.PreserveAspectFit
        visible: false
        opacity: enabled ? 1.0 : 0.5
        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(imgWindow.curIdx>0){
                    imgWindow.curIdx--;
                    console.log("上一张"+imgshowList.get(imgWindow.curIdx).path)
                    imgWindow.imgSrc = imgshowList.get(imgWindow.curIdx).path.indexOf("http://")>=0 ? imgshowList.get(imgWindow.curIdx).path : "file:///"+imgshowList.get(imgWindow.curIdx).path;
                }
            }
        }
    }
    Image{
        id: btnNext
        width: UI.fWNextBtn
        height: width
        source: "qrc:/images/icon/next.png"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: (parent.height-height)/2
        fillMode: Image.PreserveAspectFit
        visible: false
        opacity: enabled ? 1.0 : 0.5
        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(imgWindow.curIdx<imgshowList.count-1){
                    imgWindow.curIdx++;
                    console.log("下一张"+imgshowList.get(imgWindow.curIdx).path)
                    imgWindow.imgSrc = imgshowList.get(imgWindow.curIdx).path.indexOf("http://")>=0 ? imgshowList.get(imgWindow.curIdx).path : "file:///"+imgshowList.get(imgWindow.curIdx).path;
                }
            }
        }
    }

    Rectangle{
        id: bottomTool
        width: UI.fWBottomTool
        height: UI.fHBottomTool
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 10
        color: UI.cBlack
        opacity: 0.6
        LText{
            anchors.centerIn: parent
            pointSize: UI.LargeFontPointSize
            text: (curIdx+1) + "/" + imgshowList.count
            color: UI.cMainCBg
        }
        Image{
            id: fullScreen
            source: "qrc:/images/icon/fullscreen.png"
            anchors.left: parent.left
            anchors.leftMargin: UI.fItemMargin/2
            anchors.top: parent.top
            anchors.topMargin: (parent.height-height)/2

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if (imgWindow.visibility === Window.Maximized){
                        fullScreen.source = "qrc:/images/icon/fullscreen.png"
                        imgWindow.visibility = Window.AutomaticVisibility
                    }
                    else{
                        imgWindow.visibility = Window.Maximized
                        fullScreen.source = "qrc:/images/icon/small.png"
                    }

                    showimage.width = Math.min(flickable.width,showimage.sourceSize.width)
                    showimage.height = Math.min(flickable.height,showimage.sourceSize.height)
                    showimage.y = (flickable.height-showimage.height)/2
                    showimage.x = (flickable.width-showimage.width)/2
                }
            }
        }

        Image{
            id: zoomin
            source: "qrc:/images/icon/zoomin.png"
            anchors.left: fullScreen.right
            anchors.leftMargin: UI.fItemMargin
            anchors.top: parent.top
            anchors.topMargin: (parent.height-height)/2
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("放大")
                    var percent = 0.1
                    showimage.width *= (1+percent);
                    showimage.height *= (1+percent);
                    showimage.y = Math.max((flickable.height-showimage.height)/2,0) // 防止负数
                    showimage.x =  Math.max((flickable.width-showimage.width)/2,0)

                }
            }
        }
        Image{
            id: zoomout
            source: "qrc:/images/icon/zoomout.png"
            anchors.left: zoomin.right
            anchors.leftMargin: UI.fItemMargin
            anchors.top: parent.top
            anchors.topMargin: (parent.height-height)/2
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("缩小")
                    var percent = -0.1
                    showimage.width *= (1+percent);
                    showimage.height *= (1+percent);
                    showimage.y = Math.max((flickable.height-showimage.height)/2,0) // 防止负数
                    showimage.x =  Math.max((flickable.width-showimage.width)/2,0)
                }
            }
        }
        Image{
            id: shareimg
            source: "qrc:/images/icon/share.png"
            anchors.right: parent.right
            anchors.rightMargin: UI.fItemMargin
            anchors.top: parent.top
            anchors.topMargin: (parent.height-height)/2
            visible : false
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("分享")
                }
            }
        }

        Image{
            id: saveimg
            source: "qrc:/images/icon/save.png"
            anchors.right: shareimg.left
            anchors.rightMargin: UI.fItemMargin/2
            anchors.top: parent.top
            anchors.topMargin: (parent.height-height)/2
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("保存")
                    saveDialog.file_ext = imgshowList.get(imgWindow.curIdx).file_ext
                    saveDialog.httpurl = imgshowList.get(imgWindow.curIdx).url
                    saveDialog.open();
                }
            }

        }
        Image{
            id: rotateimg
            source: "qrc:/images/icon/refresh.png"
            anchors.right: saveimg.left
            anchors.rightMargin: UI.fItemMargin
            anchors.top: parent.top
            anchors.topMargin: (parent.height-height)/2
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("旋转")
                    if(showimage.rotation < 360)
                        showimage.rotation += 90;
                    else
                        showimage.rotation = 0;

                }
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
                source: "qrc:/images/icon/close_loginp.png"
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked:
                {
                    imageshow.hide()
                }
                onEntered: {
                    close.source = "qrc:/images/icon/close_login.png"
                }
                onExited: {
                    close.source = "qrc:/images/icon/close_loginp.png"
                }
            }
        }
    }
}
