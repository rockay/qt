import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.XmlListModel 2.0

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/Message.js" as MessageJS

Item {

    property XmlListModel tmpModel;
    property string atContents: "@zhuditingyu"

    property bool isLoad: false // 默认不加载
    property ListModel chatList: chatPersionmodel
    property ListView chatView: msgView

    onIsLoadChanged: {
//        if(isLoad)
//            Cloud.getClouds();
        // 获取会话列表
        ryControl.GetChatList();
    }
    XmlListModel {
        id: xmlModel
        source:"qrc:/ybemxml_new.xml"
        query: "/ybdb/img/item"
        XmlRole { name: "title"; query: "title/string()" }
        XmlRole { name: "path"; query: "path/string()" }
        XmlRole { name: "title2"; query: "title2/string()" }
        Component.onCompleted: {
            tmpModel = xmlModel;
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            chattool.face.visible = false;
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
            dispTxt: qsTr("搜索")
            svgsrc: "qrc:/images/icon/find.png"
            svgcsrc: "qrc:/images/icon/close.png"
        }

        // 左侧会话人员列表
        ListView {
            id: msgView
            property bool deleted: false
            currentIndex: -1
            anchors.left: parent.left
            anchors.top: search.bottom
            anchors.topMargin: UI.fMLsearch+5
            width: parent.width
            height: parent.height-search.height-2*UI.fMLsearch-10

            model: chatPersionmodel
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
                width: msgView.width
                height: UI.fHItem
                color:msgView.currentIndex==index? UI.cItemSelected:UI.cItem
                Image{
                    id: photo
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: parent.height/5
                    height: parent.height*3/5
                    width: height
                    source: photosrc
                }
                LText{
                    id: msgTitle
                    text: name
                    width: msgItem.width-photo.width-2*UI.fMLsearch
                    anchors.left: photo.right
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: parent.height/5
                    maximumLineCount:20
                }

                LText{
                    id: msgContent
                    text: msg
                    width: msgItem.width-photo.width-2*UI.fMLsearch
                    anchors.left: photo.right
                    anchors.top: msgTitle.bottom
                    anchors.leftMargin: UI.fMLsearch
                    fcolor: UI.cFTB
                    pointSize: UI.TinyFontPointSize
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        msgView.currentIndex = index;
                        topTitle.text = name;

                        // 切换聊天对象的记录
                        chatview.chatLists.clear();  // 清除
                        tempdata = MessageJS.allMessage.filter(function(item){
                            return (item.author == targetid || item.targetid == targetid)
                        });
//                        chatview.append(tempdata);
                        chatview.chatLists = tempdata;
                    }
                }
            }
        }
        ListModel{
            id: chatPersionmodel
        }
    }
    Rectangle{
        id:rightbar
        width: parent.width-leftarea.width
        height: parent.height
        anchors.left: leftarea.right
        anchors.top: parent.top
        color: UI.cMainBg
        Rectangle{
            id:rightTop
            width: parent.width
            height: UI.fHRithTop
            color: UI.cMainBg
            z: rightCenter.z+1
            LText{
                id: topTitle
                text:qsTr("消息")
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
            id: rightCenter
            width: parent.width
            height: parent.height-rightBottom.height-rightTop.height
            anchors.left: parent.left
            anchors.top: rightTop.bottom
            color:UI.cWhite
            border.width: 1
            border.color: UI.cTBBorder
            z: rightbar.z+1

            ChatShow{
                id: chatview
                anchors.fill: parent
            }

        }
        Rectangle{
            id: rightBottom
            width: parent.width
            height: UI.fHSendMsg
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            border.width: 1
            border.color: UI.cTBBorder
            z: rightCenter.z+1
            enabled: msgView.currentIndex >=0?true:false
            Rectangle{
                id: toolBar
                width: parent.width
                height: UI.fHSendTool
                color: UI.cMainCBg
                ChatToolBar{
                    id: chattool
                    anchors.fill: parent
                    onSignalClick:{
                        console.log(type+"|"+imgName+"|"+strPath);
                    }
                }
            }

            LTextArea{
                id: sendText
                width: rightbar.width
                height: rightBottom.height - toolBar.height - sendBtn.height
                anchors.left: parent.left
                anchors.top: toolBar.bottom
                selectByMouse: true
                Accessible.name: "document"
                //baseUrl: "qrc:/images/yibanface"
                text: chattool.document.text
                textFormat: Qt.RichText
                enabled: msgView.currentIndex >=0?true:false
                Component.onCompleted: forceActiveFocus()
            }

            LText{
                id: tips
                width: parent.width-sendBtn.width
                height: UI.fHNormalBtn
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
            }

            LButton{
                id: sendBtn
                width: UI.fWNormalBtn
                height: UI.fHNormalBtn
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                text: qsTr("发送(S)")
                onClicked: {
                    var sendtxt = chattool.document.transferText;

                    // 获取发送的客户信息
                    var idx = msgView.currentIndex;
                    var curitem = chatPersionmodel.get(idx);
                    var msgid = ryControl.sendMsg(curitem.targetid,curitem.categoryId,sendtxt);

                    var item={
                        "messageid": msgid,
                        "recipient": "Me",
                        "author": API.user_id,
                        "message": sendtxt,
                        "targetid": curitem.targetid,
                        "timestamp": MessageJS.currentDateTime(),
                        "ctype": 1
                    }
                    chatview.chatLists.insert(0,item);
                    // 添加到所有的聊天记录
//                    MessageJS.allMessage.push(0,item);


                    chattool.document.setText("");
                    sendText.clear();

                }
            }

            Connections {
                  target: ryControl
                  onReceivedMsg: {
                      switch(type){
                      case 0: // 其他
                          tips.text = ""
                      case 2: // 输入
                      case 3: // 最后发送时间
                          tips.text = msg
                          break;
                      case 1: // 消息
                          // 根据ID获取头像
                          var item={
                              "messageid": messageid,
                              "recipient": senderid,
                              "author": senderid,
                              "message": msg,
                              "targetid": API.user_id,
                              "timestamp": sendtime,
                              "ctype": 1
                          }
                          chatview.chatLists.insert(0,item);
                          // 添加到所有的聊天记录
//                          MessageJS.allMessage.push(0,item);
                          tips.text = "";
                          break;
                      }
                  }
                  onReceivedException:{
                      tips.text = "通讯异常："+data;
                  }
              }
        }
    }
}
