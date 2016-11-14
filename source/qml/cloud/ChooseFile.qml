/****************************************************
 * 名称：选择云库文件
 * 作者：LT
 * 日期：2016.10.26
 * 修改记录：
 *
 ****************************************************/
import QtQuick 2.0
import QtQuick.Controls 2.0

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/Cloud.js" as CloudJS

Rectangle {
    id: choosefile
    radius: 5
    color: UI.cMainBg //"#d2d3d4"
    border.width: 1
    border.color: UI.cUploadTittleBD
    property bool reload: false

    // type 1：表情 2：图片 3：文档 4：云 5截屏
    signal chooseFileClick(int type, string file_name, string file_url, int file_size, string file_ext, int file_mold);

    onReloadChanged: {
        CloudJS.getCloudsByMsg();
    }

    property int curIdx: -1

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
            text: qsTr("云库-选择文件(双击选择发送文件)")
            pointSize: UI.LittleFontPointSize
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

        LOperButton { source: "qrc:/images/icon/close.png"; width: headbar.height; height: headbar.height-2
            anchors.right: parent.right
            anchors.top: parent.top
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    choosefile.visible = false;
                    choosefile.curIdx = -1;
                }
            }
        }
    }

    Rectangle{
        id: centerarea
        width: parent.width -4
        height: parent.height - headbar.height - 4
        anchors.left: parent.left
        anchors.top: headbar.bottom
        color: UI.cWhite

        Component {
            id: upfileDelegate
            Rectangle{
                id: wrapper
                width: choosefilegrid.cellWidth //UI.fWFile;
                height: choosefilegrid.cellHeight
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
                    color: UI.cUploadTip //choosefilegrid.currentIndex==index ? "red" : UI.cUploadTip
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
                        choosefilegrid.currentIndex = index;
                    }
                    onDoubleClicked: {
                        chooseFileClick(4, file_name, file_url, file_size, file_ext, file_mold);
                    }
                }

            }
        }

        GridView {
            id: choosefilegrid
            anchors.fill: parent
            cellWidth: UI.fWCloudItem;
            cellHeight: UI.fHCloudItem;
            currentIndex: -1


            model: choosefilemodel
            delegate: upfileDelegate
//            highlight: Rectangle { color: UI.cLightBlue; radius: 2 }
            clip: true
            maximumFlickVelocity: 10000
            focus: true
            ScrollBar.vertical: ScrollBar { }
        }
        ListModel{
            id: choosefilemodel
        }

    }
}
