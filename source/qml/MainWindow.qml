import QtQuick 2.7
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import org.lt.db 1.0
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2

import "qrc:/controls/"
import "qrc:/qml/message/"
import "qrc:/qml/friend/"
import "qrc:/qml/group/"
import "qrc:/qml/cloud/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/login.js" as LoginJS
Window {
    id:mainform
    title: qsTr("圈图")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
    color: UI.cTransparent
    property bool reLogin: false
    property string pphoto: ""

    onPphotoChanged: {
        console.log("onPphotoChanged:"+pphoto)
        leftPhoto.source = main.pphoto;
        leftPhoto.picname = API.user_id;
    }

    onClosing:{
        ryControl.disconnect();
    }

    onReLoginChanged: {
        message.visible= true
        friend.visible= false
        group.visible = false
        cloud.visible = false

        message.isLoad = false
        friend.isLoad = false
        group.isLoad = false
        cloud.isLoad = false
        msgbtn.source = "qrc:/images/icon/messagep.png"
        frdbtn.source = "qrc:/images/icon/friend.png"
        grpbtn.source = "qrc:/images/icon/group.png"
        cldbtn.source = "qrc:/images/icon/cloud.png"
        message.chatListView.currentIndex = -1;
        message.isLoad = false;
        message.isLoad = true
    }

    UploadFile{
        id: uploadFileDialog
        width: mainform.width-120
        height: mainform.height-80
        x: 60
        y: 40
        visible: false
        z: operWinArea.z + 10
        onUploadFileSuccessed:{
            cloud.isLoad = false;
            cloud.isLoad = true; // 重新加载
        }
    }
    ChooseFile{
        id: chooseCloudFile
        width: mainform.width-140
        height: mainform.height-80
        x: 60
        y: 40
        visible: false
        z: operWinArea.z + 10
        onChooseFileClick:{
            console.log("file_name:"+file_name);
            console.log("message send clould file path:"+file_url)

            //云库文件
            message.chatviewp.ctype = 31;
            var sendtxt = "[发送云库文件]"

            var idx = message.chatListView.currentIndex;
            var retStr = message.chatListView.model.updateContacts(idx,sendtxt);
            console.log("retStr:"+retStr)
            if(retStr !== ""){
                var kk = retStr.split("|");
                if(kk.length !== 3)
                    return;
                var targetid = kk[0];
                var recipient = kk[1];
                var categoryId = kk[2];

                sendtxt = file_ext+"|"+file_mold+"|"+file_size+"|"+file_name+"|"+file_url
                var messgeid = utilityControl.getMessageId();
                message.chatviewp.chatListModel.addMessage(messgeid,messgeid,targetid,API.user_id,sendtxt,targetid,0,31,"");
                message.chatviewp.converListView.positionViewAtEnd();
                var msgid = ryControl.sendCloudMsg(messgeid,targetid,categoryId,sendtxt,31);

                chooseCloudFile.visible = false;
            }
        }
    }

    Rectangle{
        anchors.fill: parent;
        color: UI.cMainBg

        // 左侧
        Rectangle{
            id:leftbar
            width: UI.fWMainL
            height: mainform.height-50
            anchors.left: parent.left
            anchors.top: parent.top
            color: UI.cMainLBg
            Grid {
                width: parent.width
                height:parent.height-exit.height
                anchors.left: parent.left
                anchors.top: parent.top

                columns: 1
                Rectangle{
                    width: parent.width;
                    height: UI.fHToolButton
                    color: UI.cMainLBg
                    LImage {
                        id: leftPhoto
                        source: ""
                        width: parent.width/2
                        height: width
                        fillMode: Image.PreserveAspectFit
                        picname: ""
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                        }
                    }
                }
                LToolButton { id:msgbtn; source: "qrc:/images/icon/message.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("会话")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= true
                            friend.visible= false
                            group.visible = false
                            cloud.visible = false
                            msgbtn.source = "qrc:/images/icon/messagep.png"
                            frdbtn.source = "qrc:/images/icon/friend.png"
                            grpbtn.source = "qrc:/images/icon/group.png"
                            cldbtn.source = "qrc:/images/icon/cloud.png"
                            if(!message.isLoad)
                                message.isLoad = true
                        }
                    }
                }
                LToolButton {id:frdbtn; source: "qrc:/images/icon/friend.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("好友")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= true
                            group.visible = false
                            cloud.visible = false
                            msgbtn.source = "qrc:/images/icon/message.png"
                            frdbtn.source = "qrc:/images/icon/friendp.png"
                            grpbtn.source = "qrc:/images/icon/group.png"
                            cldbtn.source = "qrc:/images/icon/cloud.png"
                            if(!friend.isLoad)
                                friend.isLoad = true
                        }
                    }
                }
                LToolButton {id:grpbtn; source: "qrc:/images/icon/group.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("群组")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= false
                            group.visible = true
                            cloud.visible = false
                            msgbtn.source = "qrc:/images/icon/message.png"
                            frdbtn.source = "qrc:/images/icon/friend.png"
                            grpbtn.source = "qrc:/images/icon/groupp.png"
                            cldbtn.source = "qrc:/images/icon/cloud.png"
                            if(!group.isLoad)
                                group.isLoad = true
                        }
                    }
                }
                LToolButton {id:cldbtn; source: "qrc:/images/icon/cloud.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("云库")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= false
                            group.visible = false
                            cloud.visible = true
                            msgbtn.source = "qrc:/images/icon/message.png"
                            frdbtn.source = "qrc:/images/icon/friend.png"
                            grpbtn.source = "qrc:/images/icon/group.png"
                            cldbtn.source = "qrc:/images/icon/cloudp.png"
                            if(!cloud.isLoad)
                                cloud.isLoad = true
                        }
                    }
                }
            }

            Rectangle{
                id: exit
                width: UI.fWMainL
                height: height
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                LToolButton {
                    id:exitbtn;
                    source: "qrc:/images/icon/exit.png";
                    width: parent.width;
                    height: UI.fHToolButton
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
//                            messageDialog.open();
                            if(messagebox.visible)
                                messagebox.requestActivate();
                            else
                                messagebox.show();
                        }
                        onEntered: {
                            exitbtn.source ="qrc:/images/icon/exitp.png"
                        }
                        onExited: {
                            exitbtn.source ="qrc:/images/icon/exit.png"
                        }
                    }
                }
            }
        }

        LMessageDialog{
            id: messagebox
            visible: false
            msg: qsTr("确定要退出当前帐号？")
            flag: 3
            okTitle: "确定"
            cancelTitle: "取消"
            onOkClicked:{
                login.isCodeLogin = true;
                ryControl.disconnect();
                main.hide();
                login.show();
            }
            onCancelClicked:{
            }
        }

//        MessageDialog {
//            id: messageDialog
//            title: "温馨提示"
//            text: "确定要退出当前帐号？"
//            icon: StandardIcon.NoIcon
//            standardButtons:StandardButton.Yes | StandardButton.No
//            onYes: {
//                login.isCodeLogin = true;
//                ryControl.disconnect();
//                main.hide();
//                login.show();
//            }
//        }

        Rectangle{
            id:subview
            anchors.left: leftbar.right
            anchors.top: parent.top
            width: parent.width-leftbar.width
            height: parent.height
            color: UI.cTransparent
            Message{
                id: message
                anchors.fill: parent
                visible: true
            }
            Friend{
                id: friend
                anchors.fill: parent
                visible: false
                onSendMsgClick:{
                    console.log("photoc:"+photoc)

                    // 判断是否已经存在
                    var idx = -1;
                    for(var i=0; i<message.chatListModel.rowCount(); i++){
                        if(message.chatListModel.getId(i)==userid){
                            idx = i;
                            break;
                        }
                    }

                    //添加或更新
                    if(idx>-1){
                        message.chatListView.model.updateContacts(idx,msgc)
                    }else{
                        message.chatListView.model.addContacts(userid,namec,nameremark,photoc,msgc,conversationType,0) // 单人
                    }

                    message.chatListView.currentIndex = -1;
                    message.chatListView.currentIndex = 0;
                    message.visible = true;
                    friend.visible = false;
                    msgbtn.source = "qrc:/images/icon/messagep.png"
                    frdbtn.source = "qrc:/images/icon/friend.png"
                }
            }
            Group{
                id:group
                anchors.fill: parent
                visible: false

                onSendGroupMsgClick:{
                    console.log("photoc:"+photoc)
                    var obj = {
                        name: namec,
                        photosrc: photoc,
                        msg: msgc,
                        categoryId: 3,   // 3 - GROUP 群组
                        targetid: userid
                    }

                    var idx = -1;
                    for(var i=0; i<message.chatListModel.rowCount(); i++){
                        if(message.chatListModel.getId(i)==userid){
                            idx = i;
                            break;
                        }
                    }

                    //添加或更新
                    if(idx>-1){
                        message.chatListView.model.updateContacts(idx,msgc)
                    }else{
                        message.chatListView.model.addContacts(userid,namec,"",photoc,msgc,conversationType,0) // 群组,空字符是因为群组没有备注
                    }

                    message.chatListView.currentIndex = -1; // 引起change事件
                    message.chatListView.currentIndex = 0;
                    message.visible = true;
                    group.visible = false;
                    msgbtn.source = "qrc:/images/icon/messagep.png"
                    grpbtn.source = "qrc:/images/icon/group.png"
                }
            }
            Cloud{
                id:cloud
                anchors.fill: parent
                visible: false
            }

        }

        Rectangle{
            id:operWinArea
            width: UI.fWOperWin
            height: UI.fHOperWin
            anchors.right: parent.right
            anchors.top: parent.top
            color: parent.color
            Grid {
                columns: 3
                spacing: 0
                LOperButton { source: "qrc:/images/icon/min.png"; width: operWinArea.width*3/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            mainform.visibility = Window.Minimized
                            main.hide();
                        }
                    }
                }
                LOperButton { source: "qrc:/images/icon/max.png"; width: operWinArea.width*3/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if (mainform.visibility === Window.Maximized){
                                parent.source = "qrc:/images/icon/max.png";
                                mainform.visibility = Window.AutomaticVisibility
                            }
                            else{
                                mainform.visibility = Window.Maximized
                                parent.source = "qrc:/images/icon/max.png";
                            }
                        }
                    }
                }
                LOperButton { source: "qrc:/images/icon/close.png"; width: operWinArea.width*4/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            utilityControl.quit();
                        }
                    }
                }
            }
        }
    }


}
