import QtQuick 2.0
import QtQuick.Controls 2.0
import org.lt.controls 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2


import "qrc:/controls/"
import "qrc:/js/API.js" as API
import "qrc:/js/UI.js" as UI

Rectangle {
    id: chattoolroot
    // type 1：表情 2：图片 3：文档 4：云 5截屏
    signal signalClick(int type, string imgName,string strPath);

    property DocumentHandler document: document
    color: UI.cTransparent

    Connections{
        target: ppFace.biaoq
        onSigFaceClicked: {
            console.log("text:"+text);
            document.insertText(text);
        }
    }

    DocumentHandler {
        id: document
        target: sendText
        cursorPosition: sendText.cursorPosition
        selectionStart: sendText.selectionStart
        selectionEnd: sendText.selectionEnd
        Component.onCompleted: {
            document.fileUrl = "qrc:/qml/textarea.html"
        }
        onError: {
            errorDialog.text = message
            errorDialog.visible = true
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("打开")
        property int fileType: 2 // 2：图片 3：文档（如PDF等）
        onAccepted: {
            console.log("You chose: " + fileUrls)
            for (var i = 0; i < fileUrls.length; i++){
                if(API.validateFileExtension(fileUrls[i].toString())) // 只能选择PDF和图片
                    signalClick(fileType,fileUrls[i].toString(),fileUrls[i].toString());
            }
        }
        onRejected: {
            console.log("Canceled")
        }
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
        ToolTip {
            id: facetip
            parent: face
            visible: false
            text: qsTr("表情")
        }
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked:{
                var mainformPos = API.getAbsolutePosition(mWindow)
                var facePos = API.getAbsolutePosition(face)
                ppFace.x = mainformPos.x + facePos.x -face.x //chattoolroot.x
                ppFace.y = mainformPos.y + facePos.y - ppFace.height - face.y //chattoolroot.y - ppFace.height
                ppFace.show();
                ppFace.requestActivate();
            }
            onEntered: {
                facetip.visible = true
                face.source = "qrc:/images/icon/facep.png"
            }
            onExited: {
                facetip.visible = false
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
        ToolTip {
            id: imagetip
            parent: img
            visible: false
            text: qsTr("图片")
        }
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked:{
                fileDialog.fileType = 2
                fileDialog.folder = fileDialog.shortcuts.pictures
                fileDialog.nameFilters = [ "图片文件 (*.jpg *.png)", "All files (*)" ]
                fileDialog.open();
            }
            onEntered: {
                imagetip.visible = true
                img.source = "qrc:/images/icon/imagep.png"
            }
            onExited: {
                imagetip.visible = false
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
        ToolTip {
            id: filetip
            parent: folder
            visible: false
            text: qsTr("文档")
        }
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked:{
                fileDialog.fileType = 3
                fileDialog.folder = fileDialog.shortcuts.documents
                fileDialog.nameFilters = [ "PDF文件 (*.pdf)", "All files (*)" ]
                fileDialog.open();
            }
            onEntered: {
                filetip.visible = true
                folder.source = "qrc:/images/icon/folderp.png"
            }
            onExited: {
                filetip.visible = false
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
        ToolTip {
            id: cloudtip
            parent: cloud
            visible: false
            text: qsTr("云库")
        }
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                chooseCloudFile.reload = false;
                chooseCloudFile.reload = true;
                chooseCloudFile.visible = true;
            }

            onEntered: {
                cloudtip.visible = true
                cloud.source = "qrc:/images/icon/cloudtp.png"
            }
            onExited: {
                cloudtip.visible = false
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
        ToolTip.text: qsTr("Alt+Q")
        ToolTip {
            id: tttip
            parent: screenshot
            visible: false
            text: qsTr("截屏并发送(Alt+Q)")
        }
        MouseArea{
            id: ssmouse
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                utilityControl.shootScreen();
//                screenControl.show();
            }

            onEntered: {
                tttip.visible = true
                screenshot.source = "qrc:/images/icon/screenshotp.png"
            }
            onExited: {
                tttip.visible = false
                screenshot.source = "qrc:/images/icon/screenshot.png"
            }
        }
    }
}
