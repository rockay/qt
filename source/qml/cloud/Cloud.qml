/****************************************************
 * 名称：云主窗体
 * 作者：LT
 * 日期：2016.10.6
 * 修改记录：
 *
 ****************************************************/
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/Cloud.js" as CloudJS

Item {
    property bool isLoad: false // 默认不加载
    onIsLoadChanged: {
        if(isLoad)
            CloudJS.getClouds();
    }


    ListModel{
        id: imgList
    }

    Connections{
        target: filegrid.model
        onCountChanged:{
            imgList.clear()
            for(var i=0; i <allmodel.count; i++){
                // 添加图片数组
                if(allmodel.get(i).file_mold === 1){
                    imgList.append({"path": allmodel.get(i).file_url,"file_ext": allmodel.get(i).file_ext})
                }
            }
        }
    }


    Rectangle{
        id:leftarea
        width: UI.fWMainC
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        color: UI.cMainCBg

        LIconTextField {
            id: search
            width: parent.width-2*UI.fMLsearch
            height: UI.fHsearch
            anchors.left: parent.left
            anchors.leftMargin: UI.fMLsearch
            anchors.top: parent.top
            anchors.topMargin: UI.fMLsearch+5
            dispTxt: qsTr("文件名称")
            svgsrc: "qrc:/images/icon/find.png"
            svgcsrc: "qrc:/images/icon/close.png"
            onValueChanged: {
                CloudJS.searchFile(categoryView.currentIndex,search.value);
            }
        }


        // 文件分类列表
        ListView {
            id: categoryView
            currentIndex: 0

            width: parent.width
            height: parent.height-search.height-2*UI.fMLsearch-10
            anchors.left: parent.left
            anchors.top: search.bottom
            anchors.topMargin: UI.fMLsearch+5

            model: filetype
            clip: true
            maximumFlickVelocity: 5000
            orientation: ListView.Vertical
            focus: true
            highlightFollowsCurrentItem: false
            spacing: 1
            delegate: msgDelegate
            ScrollIndicator.vertical: ScrollIndicator { }

        }

        Component{
            id: msgDelegate
            Rectangle{
                id: msgItem
                width:  categoryView.width
                height: UI.fHFItem
                color: categoryView.currentIndex==index? UI.cItemSelected:UI.cItem
                Rectangle{
                    id: photo
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: parent.height/5
                    height: parent.height*3/5
                    width: height
                    color: bcolor
                    Image{
                        width: parent.width*2/3
                        height: width
                        anchors.centerIn: parent
                        source: cover
                    }
                }
                LText{
                    id: msgTitle
                    text: name
                    width: msgItem.width-photo.width-2*UI.fMLsearch
                    anchors.left: photo.right
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: (parent.height-height)/2
                    maximumLineCount:20
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        topTitle.text = name
                        search.defaultValue = "";
                        categoryView.currentIndex = index;
                        CloudJS.searchFile(index,search.value);
                    }
                }
            }
        }
        ListModel{
            id: filetype
            ListElement{
                name:qsTr("全部文档")
                cover:"qrc:/images/icon/allfile.png"
                bcolor:"#F6C8AB"
            }
            ListElement{
                name:qsTr("图片文档")
                cover:"qrc:/images/icon/picfile.png"
                bcolor:"#EAF0A3"
            }
            ListElement{
                name:qsTr("Office文档")
                cover:"qrc:/images/icon/officefile.png"
                bcolor:"#B3DEEA"
            }
            ListElement{
                name:qsTr("其他文档")
                cover:"qrc:/images/icon/otherfile.png"
                bcolor:"#C8E2D0"
            }
        }

        ListModel{
            id: allmodel
        }

    }
    Rectangle{
        id:rightbar
        width: parent.width-leftarea.width
        height: parent.height
        anchors.left: leftarea.right
        anchors.top: parent.top
        color: UI.cTransparent
        Rectangle{
            id:rightTop
            width: parent.width
            height: UI.fHRithTop
            color: UI.cTransparent

            LText{
                id: topTitle
                text:qsTr("")
                anchors.centerIn: parent

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor;
                }
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
                    mainform.x = mainform.x+delta.x
                    mainform.y = mainform.y+delta.y
                }
            }

            Image{
                id: addBtn
                height: parent.height/2
                width: height
                source: "qrc:/images/icon/add.png"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: (parent.height-height)/2
                anchors.leftMargin: (parent.height-height)/2
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        uploadFileDialog.visible = true;
                    }
                    onEntered: {
                        addBtn.source ="qrc:/images/icon/addp.png"
                    }
                    onExited: {
                        addBtn.source ="qrc:/images/icon/add.png"
                    }
                }
            }
        }

        Rectangle{
            id: rightBottom
            width: parent.width - 2*UI.fMLCloud
            height: parent.height-rightTop.height-10
            anchors.left: parent.left
            anchors.leftMargin: UI.fMLCloud
            anchors.top: rightTop.bottom
            color: UI.cTransparent

            Component {
                id: contactDelegate
                Rectangle{
                    id: wrapper
                    width: filegrid.cellWidth //UI.fWFile;
                    height: filegrid.cellHeight
                    color: UI.cTransparent
                    Rectangle{
                        id: filecover
                        width: parent.width-40
                        height: width
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        anchors.left: parent.left
                        anchors.leftMargin: (parent.width-width)/2
                        border.width: 1
                        border.color: UI.cLightBlue
                        radius: 1
                        Image{
                            width: parent.width-2
                            height: width
                            anchors.centerIn: parent
                            fillMode: Image.PreserveAspectFit
                            source: file_view_url
                        }
                    }
                    LText {
                        id: contactInfo
                        text: file_name
                        width: parent.width
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        horizontalAlignment: Text.AlignHCenter
                        color: filegrid.currentIndex==index ? "red" : "black"
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
                            filegrid.currentIndex = index;
                            if (mouse.button == Qt.RightButton) { // 右键菜单
                                var pp  = Qt.point(mouse.x,mouse.y)
                                operMenu.x = pp.x;
                                operMenu.y = pp.y;
                                operMenu.open();
                                return;
                            }
                        }
                        onDoubleClicked: {
                            if( file_mold === 1) //file_mold：文件类型，1 表示图片类型，2 表示 PDF 类型
                            {
                                imageshow.imgSrc = ""
                                imageshow.imgSrc = file_url

                                // 获取当前图片的index
                                var imgIndx = 0;
                                for(var i=0; i<index; i++){
                                    if(filegrid.model.get(i).file_mold === 1)
                                        imgIndx++;
                                }

                                imageshow.curIdx = imgIndx;
                                imageshow.imgshowList = imgList
                                if(imageshow.visible)
                                    imageshow.requestActivate();
                                else
                                    imageshow.show();
                            }else if( file_mold === 2){
                                Qt.openUrlExternally(file_url);
                            }else{
                                console.log("==unsupport file...")
                            }
                        }
                    }

                    LMenu {
                        id: operMenu
                        width: 70
                        LMenuItem {
                            text: qsTr("删除")
                            onTriggered:{
                                console.log("删除");
                                CloudJS.deleteFile(cloud_id)
                            }
                        }
                    }

                }
            }

            GridView {
                id: filegrid
                anchors.fill: parent
                cellWidth: UI.fWCloudItem;
                cellHeight: UI.fHCloudItem;
                currentIndex: -1
                model: allmodel
                delegate: contactDelegate
                highlight: Rectangle { color: UI.cLightBlue; radius: 2 }
                clip: true
                maximumFlickVelocity: 10000
                focus: true
                ScrollBar.vertical: ScrollBar { }

                onDragEnded: {
                    if (header.refresh) {
                        console.log("need refresh....")
                        CloudJS.getClouds();
                    }
                }

                GridHeader {
                    id: header
                    mainListView: parent
                    y: -parent.contentY - height
                }
            }
        }
    }

}

