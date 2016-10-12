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
        height: UI.fChatToolH
        width: height
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: (parent.height-UI.fChatToolH)/2
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon/face.png"
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked:{
                biaoq.visible = !biaoq.visible;
                animation2.stop();
                animation2.start();
            }
            onEntered: {
                face.source = "qrc:/images/icon/facep.png"
            }
            onExited: {
                face.source = "qrc:/images/icon/face.png"
            }
        }
    }
    Image{
        id: img
        height: UI.fChatToolH
        width: height
        anchors.left: face.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: (parent.height-UI.fChatToolH)/2
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon/image.png"
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked:{
                fileDialog.folder = fileDialog.shortcuts.pictures
                fileDialog.nameFilters = [ "图片文件 (*.jpg *.png)", "All files (*)" ]
                fileDialog.open();
            }
            onEntered: {
                img.source = "qrc:/images/icon/imagep.png"
            }
            onExited: {
                img.source = "qrc:/images/icon/image.png"
            }
        }
    }
    Image{
        id: folder
        height: UI.fChatToolH
        width: height
        anchors.left: img.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: (parent.height-UI.fChatToolH)/2
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon/folder.png"
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked:{
                fileDialog.folder = fileDialog.shortcuts.documents
                fileDialog.nameFilters = [ "PDF文件 (*.pdf)", "All files (*)" ]
                fileDialog.open();
            }
            onEntered: {
                folder.source = "qrc:/images/icon/folderp.png"
            }
            onExited: {
                folder.source = "qrc:/images/icon/folder.png"
            }
        }
    }
    Image{
        id: cloud
        height: UI.fChatToolH
        width: height
        anchors.left: folder.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: (parent.height-UI.fChatToolH)/2
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon/cloudt.png"
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onEntered: {
                cloud.source = "qrc:/images/icon/cloudtp.png"
            }
            onExited: {
                cloud.source = "qrc:/images/icon/cloudt.png"
            }
        }
    }
    Image{
        id: screenshot
        height: UI.fChatToolH
        width: height
        anchors.left: cloud.right
        anchors.top: parent.top
        anchors.leftMargin: height
        anchors.topMargin: (parent.height-UI.fChatToolH)/2
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon/screenshot.png"
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onEntered: {
                screenshot.source = "qrc:/images/icon/screenshotp.png"
            }
            onExited: {
                screenshot.source = "qrc:/images/icon/screenshot.png"
            }
        }
    }
}
