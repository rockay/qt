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
import "qrc:/js/Friend.js" as FriendJS

Item {
    property bool isLoad: false // 默认不加载
    signal sendMsgClick(string userid,string namec, string nameremark, string photoc,string msgc,int conversationType);
    onIsLoadChanged: {
        if(isLoad){
            FriendJS.getContacts();
            FriendJS.getCompany();
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
            dispTxt: qsTr("好友名称")
            svgsrc: "qrc:/images/icon/find.png"
            svgcsrc: "qrc:/images/icon/close.png"
            onValueChanged: {
                FriendJS.search(category.currentIndex,search.value)
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
                    FriendJS.search(category.currentIndex,search.value)
                }
            }
            LTabButton {
                text: qsTr("企业")
                onClicked:{
                    search.defaultValue = ""
                    FriendJS.search(category.currentIndex,search.value)
                }
            }
          }
        StackLayout {
            width: parent.width
            height: parent.height-search.height-category.height-2*UI.fMLsearch-10
            currentIndex: category.currentIndex
            anchors.left: parent.left
            anchors.top: category.bottom

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
                onDragEnded: {
                    if (header.refresh) {
                        console.log("need refresh....")
                        FriendJS.getContacts();
                    }
                }

                ListHeader {
                    id: header
                    mainListView: parent
                    y: -parent.contentY - height
                }
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
                onDragEnded: {
                    if (companyheader.refresh) {
                        console.log("need refresh....")
                        FriendJS.getCompany();
                    }
                }

                ListHeader {
                    id: companyheader
                    mainListView: parent
                    y: -parent.contentY - height
                }

                section.delegate: sectionDelegate
                section.property: "company_name"
                section.criteria: ViewSection.FullString
            }

        }
        //企业组
        Component{
            id:sectionDelegate
            Rectangle {
                id:dataGroupContainer
                width: busiView.width
                height: UI.fHFItem
                color: UI.cTransparent
                Image{
                    id: lefticon
                    source: sectionName.selected ? "qrc:/images/icon/arr_down.png" : "qrc:/images/icon/arr_right.png"
                    width: 21
                    height: 21
                    fillMode: Image.PreserveAspectFit
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: (parent.height-height)/2
                }
                LText{
                    id: sectionName
                    width: parent.width
                    height: parent.height
                    font.pointSize: UI.LittleFontPointSize
                    text: section
                    color: selected ? UI.cBlack : "#818181"
                    anchors.left: lefticon.right
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: (parent.height-height)/2
                    property bool selected: false
                }

                Rectangle{
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    border.width: 1
                    border.color: UI.cWhite
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("点击分组-展开/关闭...");
                        if( sectionName.selected == true)
                            sectionName.selected = false;
                        else
                            sectionName.selected = true;

                        for(var k=0;k<busimodel.rowCount();k++){
                            if(busimodel.get(k).company_name != sectionName.text)continue;
                            busimodel.get(k).visibled = !busimodel.get(k).visibled;
                        }
                    }
                }
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
                    text: friend_remark_name =="" ? friend_name : friend_remark_name
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
                        nameremark.text = friend_remark_name;
                        typec.text = category.currentIndex==0 ? "个人用户" : "企业用户" //type
                        targetid.text = friend_id;
                    }
                    onDoubleClicked: {
                        sendMsgClick(friend_id, friend_name, friend_remark_name, friend_photo,"",1);
                    }
                }


                states: [
                    State {
                        name: "showItem"
                        when: visibled
                        PropertyChanges { target: msgItem; height: UI.fHFItem ;opacity:1}
                    },
                    State {
                        name: "hideItem"
                        when: !visibled
                        PropertyChanges { target: msgItem; height: 0;opacity:0}
                    }
                ]
                transitions: [
                    Transition {
                        from: "showItem"
                        to: "hideItem"
                        NumberAnimation { properties: "opacity"; duration: 400 }
                        NumberAnimation { properties: "height"; duration: 400 }
                    },
                    Transition {
                        from: "hideItem"
                        to: "showItem"
                        NumberAnimation { properties: "opacity"; duration: 400 }
                        NumberAnimation { properties: "height"; duration: 400 }
                    }
                ]
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
                Item{
                    id: nameitem
                    width: (card.width-photoc.width-UI.fMLsearch)/2
                    height: namec.contentHeight
                    anchors.left: photoc.right
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: UI.fMLsearch
                    LText{
                        id: namec
                        pointSize: UI.StandardFontPointSize
                        font.bold: true
                        width: parent.width
                        visible: nameremark.text == "" ? true : false;
                    }
                    LText{
                        id: nameremark
                        pointSize: UI.StandardFontPointSize
                        font.bold: true
                        width: parent.width
                        visible: text == "" ? false : true;
                    }
                }

                Rectangle{
                    width: nameitem.width
                    height: nameitem.height
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
                    source: "qrc:/images/icon/contacts.png"
                    anchors.left: photoc.right
                    anchors.leftMargin: UI.fMLsearch
                    anchors.top: nameitem.bottom
                    anchors.topMargin: UI.fMLsearch
                    height: nameitem.height
                    width: height
                }
                LText{
                    id: phonec
                    anchors.right: parent.right
                    anchors.top: nameitem.bottom
                    anchors.leftMargin: UI.fMLsearch
                    pointSize: UI.SmallFontPointSize
                    anchors.topMargin: UI.fMLsearch
                    maximumLineCount:20
                    height: contacts.height
                    verticalAlignment:  Text.AlignVCenter
                }
                LText{
                    id: targetid
                    visible: false
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
                    onClicked: {
                        sendMsgClick(targetid.text,namec.text,nameremark.text,photoc.source,"",1);
                    }
                }
            }
        }
    }

//    Component.onCompleted: {
//        Friend.getContacts();
//        Friend.getCompany();
//    }
}
