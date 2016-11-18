import QtQuick 2.0
import QtQml.Models 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/Cloud.js" as CloudJS

Rectangle {
    id: uploadfile
    radius: 5
    color: UI.cMainBg //"#d2d3d4"
    border.width: 1
    border.color: UI.cUploadTittleBD
    property int curIdx: 0
    property int allSize: 0
    signal uploadFileSuccessed();

    FileDialog {
        id: fileDialog
        title: qsTr("上传文件")
        selectMultiple: true
        nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + fileUrls)
            CloudJS.selectFiles(fileUrls)
        }
        onRejected: {
            console.log("Canceled")
        }
    }


    Connections{
        target: utilityControl
        onUploadMaterialRet:{
            if(filefrom == 1){
                if(retCode == 0){
                    CloudJS.saveFile(retMsg)
                    console.log("成功",type,retMsg);
                }
                else{
                    console.log("失败",type,retMsg);
                    upfilemodel.setProperty(messageid, "percent", -1);
                    curIdx++
                    CloudJS.uploadFie();
                }
            }
        }
        onUpdateProgress:{
            if(filefrom == 1){
                upfilemodel.setProperty(messageid, "percent", percent);
                console.log("percent:",percent);
            }
        }
    }

    LMessageDialog{
        id: upFinishDialog
        visible: false
        x: main.x + (main.width-width)/2
        y: main.y + (main.height-height)/2
        msg: qsTr("文件上传完成")
        flag: 1
        okTitle: "确定"
        cancelTitle: "取消"
    }
//    MessageDialog {
//        id: upFinishDialog
//        title: "温馨提示"
//        text: "文件上传完成"
//        icon: StandardIcon.NoIcon
//        standardButtons:StandardButton.Ok
//    }

    Rectangle{
        id: headbar
        height: uptitle.contentHeight*1.8
        width: parent.width - 4
        color: UI.cTransparent
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.top: parent.top
        anchors.topMargin: 2
        LText{
            id: uptitle
            color: UI.cUploadTitle
            text: qsTr("上传文件")
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.top: parent.top
            anchors.topMargin: (parent.height - height-1)/2
        }

        Rectangle{
            width: parent.width
            height: 1
            color: UI.cUploadTittleBD
            anchors.bottom: parent.bottom
        }

//        MouseArea {
//            id: dragRegion
//            anchors.fill: parent
//            property point clickPos: "0,0"
//            onPressed: {
//                clickPos  = Qt.point(mouse.x,mouse.y)
//            }

//            onPositionChanged: {
//                //鼠标偏移量
//                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
//                //如果mainwindow继承自QWidget,用setPos
//                uploadfile.x = uploadfile.x+delta.x
//                uploadfile.y = uploadfile.y+delta.y
//            }
//        }

        LOperButton { source: "qrc:/images/icon/close.png"; width: headbar.height; height: headbar.height-2
//            z: dragRegion.z+1
            anchors.right: parent.right
            anchors.top: parent.top
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    uploadfile.visible = false;
                    uploadfile.allSize = 0;
                    uploadfile.curIdx = 0;
                    upfilemodel.clear();
                }
            }
        }
    }

    Rectangle{
        id: centerarea
        width: parent.width -4
        height: parent.height - headbar.height - bottombar.height - 4
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.top: headbar.bottom
        Component {
            id: upfileDelegate
            Rectangle{
                id: wrapper
                width: upfilegrid.cellWidth //UI.fWFile;
                height: upfilegrid.cellHeight
                color: UI.cTransparent
                Rectangle{
                    id: filecover
                    width: parent.width-40
                    height: width
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.left: parent.left
                    anchors.leftMargin: (parent.width-width)/2
                    border.color: UI.cLightBlue
                    radius: 1
                    Image{
                        width: parent.width-2
                        height: width
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit
                        source: file_mold == 1 ? file_url : ""
                        visible: file_mold == 1 ? true : false
                    }
                    Rectangle{
                        anchors.fill: parent
                        radius: 5
                        color: UI.cRed
                        LText{
                            anchors.centerIn: parent
                            color: UI.cWhite
                            text: file_ext
                            pointSize: UI.HugeFontPointSize
                        }
                        visible: file_mold != 1 ? true : false
                    }

                    Rectangle{
                        id: mask
                        opacity: 0.5
                        visible: percent==0 ? false : true
                        anchors.fill: parent
                        color: UI.cBlack
                    }
                    LText{
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.leftMargin:(parent.width-width)/2
                        anchors.bottomMargin: 10
                        color: UI.cWhite
                        visible: mask.visible
                        pointSize: UI.StandardFontPointSize
                        text: percent<101 ? percent==-1 ? "失败" : percent+"%" : qsTr("成功")
                    }
                }

                LText {
                    id: contactInfo
                    text: file_name
                    width: parent.width
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    horizontalAlignment: Text.AlignHCenter
                    color: UI.cUploadTip //upfilegrid.currentIndex==index ? "red" : UI.cUploadTip
                }
                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton // 激活右键
                    onEntered: {
                        wrapper.color = UI.cLightBlue
                    }
                    onExited: {
                        wrapper.color = UI.cTransparent
                    }

                    onClicked: {
                        upfilegrid.currentIndex = index;
                        if (mouse.button == Qt.RightButton) { // 右键菜单
                            var pp  = Qt.point(mouse.x,mouse.y)
                            fileMenu.x = pp.x;
                            fileMenu.y = pp.y;
                            fileMenu.open();
                            return;
                        }
                    }
                }
                LMenu {
                    id: fileMenu
                    width: 70
                    LMenuItem {
                        text: qsTr("删除")
                        onTriggered:{
                            console.log("删除");
                            upfilemodel.remove(index);
                            CloudJS.reCalculate();

                        }
                    }
                }

            }
        }

        GridView {
            id: upfilegrid
            anchors.fill: parent
            cellWidth: UI.fWCloudItem;
            cellHeight: UI.fHCloudItem;
            currentIndex: -1


            model: upfilemodel
            delegate: upfileDelegate
//            highlight: Rectangle { color: UI.cLightBlue; radius: 2 }
            clip: true
            maximumFlickVelocity: 10000
            focus: true
            ScrollBar.vertical: ScrollBar { }
        }
        ListModel{
            id: upfilemodel
        }
    }

    Rectangle{
        id: bottombar
        height: 60
        width: parent.width - 4
        color: UI.cTransparent
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        LButton{
            id: uploadBtn
            text: qsTr("开始上传")
            width: 100
            height: 40
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.top: parent.top
            color: UI.cWhite
            bgcolor: UI.cUploadBtnBG
            bdcolor: UI.cUploadBtnBD
            anchors.topMargin: (parent.height - height)/2
            onClicked: {
                // 开始上传
                // 从第1个开始
                if(upfilemodel.count==0)
                {
                    bottomtips.text = qsTr("请先选择要上传的文件");
                    bottomtips.visible = true;
                    return;
                }

                curIdx = 0;
                CloudJS.uploadFie();
            }
        }
        LButton{
            id: addFileBtn
            text: qsTr("添加文件")
            width: 100
            height: 40
            anchors.right: uploadBtn.left
            anchors.rightMargin: 10
            anchors.top: parent.top
            color: UI.cWhite
            bgcolor: UI.cUploadBtnBG
            bdcolor: UI.cUploadBtnBD
            anchors.topMargin: (parent.height - height)/2
            onClicked: {
                fileDialog.folder = fileDialog.shortcuts.pictures
                fileDialog.open();
            }
        }
        LText{
            id: bottomtips
            text: qsTr("选择或拖拽图片与PDF文档上传")
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.top: parent.top
            anchors.topMargin: (parent.height - height)/2
            visible: true
            color: UI.cUploadTip
        }
    }


    DropArea{
        anchors.fill: parent;
        onEntered:{
            // Ensure at least one file is supported before accepted the drag
            var validFile = false;
            for(var i = 0; i < drag.urls.length; i++) {
              if(API.validateFileExtension(drag.urls[i])) {
                validFile = true;
                break;
              }
            }

            if(!validFile) {
              drag.accepted = false;
              return false;
            }
        }

        onDropped: {
            if(drop.hasUrls){
                CloudJS.selectFiles(drop.urls)
            }
        }
    }

}
