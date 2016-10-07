/****************************************************
 * 名称：联系人主窗体
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
import "qrc:/js/Friend.js" as Friend

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
            dispTxt: qsTr("好友名称")
            svgsrc: "qrc:/images/find.png"
            svgcsrc: "qrc:/images/close.png"
            onValueChanged: {
                Friend.search(category.currentIndex,search.value)
            }
        }
        Rectangle{
            id: line
            width: parent.width
            height: 1
            color: UI.cTBBorder
            anchors.left: parent.left
            anchors.top: search.bottom
            anchors.topMargin: UI.fMLsearch
        }

        LTabBar {
            id: category
            width: parent.width
            anchors.top: line.bottom
            anchors.left: parent.left
            LTabButton {
                text: qsTr("通讯录")
                onClicked:{
                    search.defaultValue = ""
                    Friend.search(category.currentIndex,search.value)
                }
            }
            LTabButton {
                text: qsTr("企业")
                onClicked:{
                    search.defaultValue = ""
                    Friend.search(category.currentIndex,search.value)
                }
            }
          }
        StackLayout {
            width: parent.width
            height: parent.height-search.height-category.height-2*UI.fMLsearch-10
            currentIndex: category.currentIndex
            anchors.left: parent.left
            anchors.top: category.bottom
//            anchors.topMargin: UI.fMLsearch+5

            // 通讯录列表
            ListView {
                id: msgView
                property bool deleted: false
                currentIndex: -1

                model: allmodel
                clip: true
                maximumFlickVelocity: 5000
                orientation: ListView.Vertical
                focus: true
                highlightFollowsCurrentItem: false
                spacing: 1
                delegate: msgDelegate
                ScrollIndicator.vertical: ScrollIndicator { }
            }

            // 企业好友列表
            ListView {
                id: busiView
                property bool deleted: false
                currentIndex: -1

                model: busimodel
                clip: true
                maximumFlickVelocity: 5000
                orientation: ListView.Vertical
                focus: true
                highlightFollowsCurrentItem: false
                spacing: 1
                delegate: msgDelegate
                ScrollIndicator.vertical: ScrollIndicator { }
            }

        }

        Component{
            id: msgDelegate
            Rectangle{
                id: msgItem
                width: msgView.width
                height: UI.fHFItem
                color:msgView.currentIndex==index? UI.cItemSelected:UI.cItem
                Image{
                    id: photo
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: parent.height/5
                    height: parent.height*3/5
                    width: height
                    source: friend_photo
                }
                LText{
                    id: msgTitle
                    text: friend_name
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
                        card.visible = true
                        msgView.currentIndex = index;
                        photoc.source = friend_photo
                        namec.text = friend_name;
                        phonec.text = friend_mobile;
                        typec.text = "个人用户"//type;
                    }
                }
            }
        }
        ListModel{
            id: allmodel
        }

        ListModel{
            id: busimodel
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
            anchors.top: rightTop.bottom
            color: UI.cTransparent
            Rectangle{
                id: card
                width: UI.fWCard
                height: UI.fHCard
                color: UI.cTransparent
                anchors.centerIn: parent
                visible: false
                Image{
                    id: photoc
                    anchors.left: parent.left
                    anchors.top: parent.top
                    height: parent.height/2
                    width: height
                }
                LText{
                    id: namec
                    width: (card.width-photoc.width-UI.fMLsearch)/2
                    anchors.left: photoc.right
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    pointSize: UI.StandardFontPointSize
                    anchors.topMargin: UI.fMLsearch
                    font.bold: true
                    maximumLineCount:20
                }
                Rectangle{
                    width: namec.width
                    height: namec.height
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: UI.fMLsearch
                    color: UI.cTypeBG
                    radius: 2
                    LText{
                        id: typec
                        anchors.centerIn: parent
                        color: UI.cWhite
                    }
                }
                Image{
                    id: contacts
                    source: "qrc:/images/contacts.png"
                    anchors.left: photoc.right
                    anchors.leftMargin: UI.fMLsearch
                    anchors.top: namec.bottom
                    anchors.topMargin: UI.fMLsearch
                    height: namec.height
                    width: height
                }
                LText{
                    id: phonec
                    anchors.right: parent.right
                    anchors.top: namec.bottom
                    anchors.leftMargin: UI.fMLsearch
                    pointSize: UI.SmallFontPointSize
                    anchors.topMargin: UI.fMLsearch
                    maximumLineCount:20
                    height: contacts.height
                    verticalAlignment:  Text.AlignVCenter
                }

                Rectangle{
                    border.width: 1
                    border.color: UI.cTBBorder
                    height: 2
                    width: parent.width
                    anchors.left: parent.left
                    anchors.top: photoc.bottom
                    anchors.topMargin: 20
                }

                LButton{
                    text: qsTr("发送消息")
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: UI.fHNormalBtn*2
                    bgcolor: UI.cButtonBG
                    color: UI.cWhite
                    radius: 4
                    fontSize: UI.StandardFontPointSize
                }
            }
        }
    }

    Component.onCompleted: {
        Friend.getContacts();
        Friend.getCompany();
    }
}
