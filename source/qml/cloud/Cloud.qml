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
import "qrc:/js/Cloud.js" as Cloud

Item {
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
                Cloud.filter(categoryView.currentIndex,search.value);
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
                Image{
                    id: photo
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: parent.height/5
                    height: parent.height*3/5
                    width: height
                    source: cover
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
                        Cloud.filter(index,search.value);
                    }
                }
            }
        }
        ListModel{
            id: filetype
            ListElement{
                name:qsTr("全部文档")
                cover:"qrc:/images/icon/allfile.png"
            }
            ListElement{
                name:qsTr("图片文档")
                cover:"qrc:/images/icon/picfile.png"
            }
            ListElement{
                name:qsTr("Office文档")
                cover:"qrc:/images/icon/officefile.png"
            }
            ListElement{
                name:qsTr("其他文档")
                cover:"qrc:/images/icon/otherfile.png"
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
        }

        Rectangle{
            id: rightBottom
            width: parent.width
            height: parent.height-rightTop.height
            anchors.left: parent.left
            anchors.leftMargin: UI.fMLCloud
            anchors.top: rightTop.bottom
            color: UI.cTransparent

            Component {
                id: contactDelegate
                Rectangle{
                    id: wrapper
                    width: UI.fWFile;
                    height: filegrid.cellHeight-20
                    color: UI.cTransparent
                    Rectangle{
                        id: filecover
                        width: parent.width
                        height: width
                        anchors.left: parent.left
                        anchors.top: parent.top
                        border.width: 1
                        border.color: UI.cLightBlue
                        radius: 1
                        Image{
                            width: parent.width-2
                            height: width
                            anchors.centerIn: parent
                            source: file_view_url
                        }
                    }
                    LText {
                        id: contactInfo
                        text: file_name
                        width: parent.width
                        height: parent.height-filecover.height
                        anchors.left: parent.left
                        anchors.top: filecover.bottom
                        horizontalAlignment: Text.AlignHCenter
                        color: filegrid.currentIndex==index ? "red" : "black"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onEntered: {
                            wrapper.color = UI.cLightBlue
                        }
                        onExited: {
                            wrapper.color = UI.cTransparent
                        }

                        onClicked: {
                            filegrid.currentIndex = index;
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
            }
        }
    }

    Component.onCompleted: {
        Cloud.getClouds();
    }
}

