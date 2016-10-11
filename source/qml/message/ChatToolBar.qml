import QtQuick 2.0
import QtQuick.Controls 2.0
import org.lt.controls 1.0
import QtQuick.Dialogs 1.2


import "qrc:/controls/"
import "qrc:/js/UI.js" as UI

Rectangle {
    // type 1：表情 2：图片 3：文档 4：云 5截屏
    signal signalClick(int type, string imgName,string strPath);

    property DocumentHandler document: biaoq.document
    property LFace face: biaoq
    color: UI.cTransparent
    LFace{
        id:biaoq
        anchors.bottom: parent.top
        anchors.left: parent.left
        opacity: 0
        visible: false
        onSignalClickCurrentImg: {
            console.log(imgName+" "+strPath);
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("打开")
        onAccepted: {
            console.log("You chose: " + fileUrls)
            for (var i = 0; i < fileUrls.length; i++)
                sendText.text += fileUrls[i].toString();
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    SequentialAnimation {
        id:animation2
        NumberAnimation { target: biaoq; property: "opacity"; to: 0.8; duration: 500 }
        PauseAnimation { duration: 2000 }
    }
    SequentialAnimation {
        id:animation3
        NumberAnimation { target: biaoq; property: "opacity"; to: 0.0; duration: 500 }
        PauseAnimation { duration: 2000 }
    }

    Image{
        id: face
        height: parent.height/2
        width: height
        anchors.left: parent.left
        anchors.top: parent.top
//        anchors.leftMargin: height
        anchors.topMargin: parent.height/4
        source: "qrc:/images/icon/face.png"
        MouseArea{
            anchors.fill: parent
            onClicked:{
                biaoq.visible = !biaoq.visible;
                animation2.stop();
                animation2.start();
            }
        }
    }
    Image{
        id: img
        height: parent.height/2
        width: height
        anchors.left: face.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: parent.height/4
        source: "qrc:/images/icon/image.png"
        MouseArea{
            anchors.fill: parent
            onClicked:{
                fileDialog.folder = fileDialog.shortcuts.pictures
                fileDialog.nameFilters = [ "图片文件 (*.jpg *.png)", "All files (*)" ]
                fileDialog.open();
            }
        }
    }
    Image{
        id: folder
        height: parent.height/2
        width: height
        anchors.left: img.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: parent.height/4
        source: "qrc:/images/icon/folder.png"
        MouseArea{
            anchors.fill: parent
            onClicked:{
                fileDialog.folder = fileDialog.shortcuts.documents
                fileDialog.nameFilters = [ "PDF文件 (*.pdf)", "All files (*)" ]
                fileDialog.open();
            }
        }
    }
    Image{
        id: cloud
        height: parent.height/2
        width: height
        anchors.left: folder.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: parent.height/4
        source: "qrc:/images/icon/cloud.png"
    }
    Image{
        id: screenshot
        height: parent.height/2
        width: height
        anchors.left: cloud.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: parent.height/4
        source: "qrc:/images/icon/screenshot.png"
    }
}
