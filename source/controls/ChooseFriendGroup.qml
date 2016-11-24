/****************************************************
 * 名称：选择好友或群组文件
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
import "qrc:/js/Friend.js" as FriendJS

Rectangle {
    id: choosefriendgroup
    radius: 5
    color: UI.cMainBg //"#d2d3d4"
    border.width: 1
    border.color: UI.cUploadTittleBD
    property bool isLoad: false


    // ctype 1：个人 3：群组
    signal chooseFriendGroupClick(string ctype, string id, string name,string photo);
    signal chooseFriendGroupAfter(string ctype, string id, string name,string photo, var msgtype,string msgcontent);

    onIsLoadChanged: {
        if(isLoad)
            FriendJS.getFriendGroup();
    }

    property int curIdx: -1

    Rectangle{
        id: headbar
        height: UI.fHsearch+10
        width: parent.width - 4
        color: UI.cTransparent
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.top: parent.top
        anchors.topMargin: 2

        LIconTextField {
            id: search
            width: parent.width-2*UI.fMLsearch - closeBtn.width
            height: UI.fHsearch
            anchors.left: parent.left
            anchors.leftMargin: UI.fMLsearch
            y: (parent.height- height)/2
            dispTxt: qsTr("搜索")
            svgsrc: "qrc:/images/icon/find.png"
            svgcsrc: "qrc:/images/icon/close.png"
            onValueChanged: {
                FriendJS.searchChoose(search.value)
            }
        }

        LOperButton {
            id: closeBtn; source: "qrc:/images/icon/close.png"; width: headbar.height; height: headbar.height-2
            anchors.right: parent.right
            anchors.top: parent.top
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    choosefriendgroup.visible = false;
                    choosefriendgroup.curIdx = -1;
                }
            }
        }
    }

    Rectangle{
        id: centerarea
        width: parent.width -2*UI.fMLsearch
        height: parent.height - headbar.height - 4
        anchors.left: parent.left
        anchors.leftMargin: UI.fMLsearch
        anchors.top: headbar.bottom
        color: UI.cTransparent

        // 群组列表
        ListView {
            id: groupView
            currentIndex: -1

            anchors.fill: parent
            model: friendgroupmodel
            clip: true
            maximumFlickVelocity: 5000
            orientation: ListView.Vertical
            focus: true
            highlightFollowsCurrentItem: false
            spacing: 0
            delegate: msgDelegate
            ScrollIndicator.vertical: ScrollIndicator { }

            onDragEnded: {
                if (header.refresh) {
                    console.log("need refresh....")
                    FriendJS.getFriendGroup();
                }
            }

            ListHeader {
                id: header
                mainListView: parent
                y: -parent.contentY - height
            }
        }

        Component{
            id: msgDelegate
            Rectangle{
                id: msgItem
                width: groupView.width
                height: UI.fHFItem*2/3
                color:UI.cTransparent //groupView.currentIndex==index? UI.cItemSelected:UI.cTransparent
                LImage{
                    id: photoimg
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 0
                    anchors.topMargin: parent.height/5
                    height: parent.height*3/5
                    width: height
                    picname: id
                    source: photo
                }
                LText{
                    id: msgTitle
                    text: remark_name == "" ? name : remark_name
                    width: msgItem.width-photo.width-2*UI.fMLsearch
                    anchors.left: photoimg.right
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: (parent.height-height)/2
                    maximumLineCount:20
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        msgItem.color = UI.cItemSelected
                    }
                    onExited: {
                        msgItem.color= UI.cTransparent // groupView.currentIndex==index? UI.cItemSelected:UI.cTransparent
                    }

                    onClicked: {
                        messagebox.name = msgTitle.text
                        groupView.currentIndex = index
                        if(messagebox.visible)
                            messagebox.requestActivate();
                        else
                            messagebox.show();
                    }
                }
            }
        }
        ListModel{
            id: friendgroupmodel
        }
        LMessageDialog{
            id: messagebox
            property string name: ""
            visible: false
            msg: qsTr("是否转发给："+name)
            flag: 3
            okTitle: "确定"
            cancelTitle: "取消"
            onOkClicked:{
                chooseFriendGroupClick(groupView.model.get(groupView.currentIndex).type,
                                       groupView.model.get(groupView.currentIndex).id,
                                       groupView.model.get(groupView.currentIndex).name,
                                       groupView.model.get(groupView.currentIndex).photo);
                choosefriendgroup.visible = false
            }
        }

    }
}
