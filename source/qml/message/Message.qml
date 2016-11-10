import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.XmlListModel 2.0
import QtMultimedia 5.6

import org.lt.db 1.0

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/Message.js" as MessageJS

Item {

    property XmlListModel tmpModel;

    property bool isLoad: false // 默认不加载
    property SqlContactModel chatListModel: contactListView.model
    property ListView chatListView: contactListView
    property ChatShow chatviewp: chatview
    property ChatToolBar chattoolp: chattool
    property LTextArea txtAreaInput: sendText

    Connections{ // 上传素材返回
        target: utilityControl
        onUploadMaterialRet:{
            if(filefrom == 0){
                if(retCode == 0){
                    MessageJS.saveFileMsg(retMsg)
                    console.log("成功",type,retMsg);
                }
                else{
                    console.log("失败",type,retMsg);
                }
            }
        }
        onUpdateProgress:{
            if(filefrom == 0){
//                upfilemodel.setProperty(curIdx, "percent", percent);
                console.log("percent:",percent);
            }
        }
    }
    Connections{ // 重新发送
        target: chatview
        onSignResendMsg:{
            chatview.ctype = type;
            switch(type){
            case 4: // 文字
            case 5: // 图片
                MessageJS.sendMsg(content, chatview.user_type,chatview.ctype)
                break;
            case 31: // 云
                var sendtxt = "[发送云库文件]"

                var idx = contactListView.currentIndex;
                var retStr = contactListView.model.updateContacts(idx,sendtxt);
                console.log("retStr:"+retStr)
                if(retStr !== ""){
                    var kk = retStr.split("|");
                    if(kk.length !== 3)
                        return;
                    var targetid = kk[0];
                    var recipient = kk[1];
                    var categoryId = kk[2];
                    var messgeid = utilityControl.getMessageId();
                    message.chatviewp.chatListModel.addMessage(messgeid,messgeid,targetid,settings.user_id,content,targetid,0,31,"");
                    ryControl.sendCloudMsg(messgeid,targetid,categoryId,content,31);
                }

                break;
            default:
                return;
            }
            // 删除原来的记录
            chatview.chatListModel.deleteMsgByID(msgid)
        }
    }

    Connections{ // 显示
        target: utilityControl
        onSigquit: {
            console.log("hide");
            main.hide();
            login.hide();
        }
        onSigshow:{
            console.log("show");
            if(login.visible || !login.isLogin){ // 如果是登录窗体点击显示则不显示
                return;
            }

            if(ischange){ // 需要切换到当前第一个
                contactListView.currentIndex = -1
                contactListView.currentIndex = 0
            }
            main.show();
            main.requestActivate();
            // 这里还要选中聊天人
            login.hide();
        }
    }

    Connections{ // 下载文件
        target: networkControl
        onDownProcess:{
            if(percent<100)
                tips.text = "下载进度："+percent+"%";
            else
                tips.text = "下载完成"
        }

    }

    onIsLoadChanged: {
        console.log("onIsLoadChanged,need refresh....")
        contactListView.model.refresh();
        chatviewp.converListView.model.refresh();
        contactListView.currentIndex = -1;
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

    SoundEffect {
        id: msgSound
        source: "qrc:/images/message.wav"
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
            onValueChanged: {
                chatListModel.name = search.value
            }
        }

        // 左侧会话人员列表
        ListView {
            id: contactListView
            property bool deleted: false
            currentIndex: -1
            anchors.left: parent.left
            anchors.top: search.bottom
            anchors.topMargin: UI.fMLsearch+5
            width: parent.width
            height: parent.height-search.height-2*UI.fMLsearch-10

            model: SqlContactModel {}//chatPersionmodel
            clip: true
            maximumFlickVelocity: 5000
            orientation: ListView.Vertical
            focus: true
            highlightFollowsCurrentItem: false
            spacing: 1
            delegate: msgDelegate
            ScrollIndicator.vertical: ScrollIndicator { }
            onCurrentIndexChanged: {
                console.log("changed..."+currentIndex)
                if(currentIndex>-1){
                    topTitle.text = model.get(currentIndex).user_remark == ""? model.get(currentIndex).user_name : model.get(currentIndex).user_remark
                    chatview.user_id = model.get(currentIndex).user_id
                    chatview.user_type = model.get(currentIndex).categoryId // 1:person 3:group
                    chatview.user_photo = model.get(currentIndex).categoryId==1 ? model.get(currentIndex).user_photo : ""
                    if(chatview.user_type == "1")
                        grouparea.width=0
                    else{
                        grouparea.width = UI.fWGroup
                        MessageJS.getGroupMember(chatview.user_id)
                    }
                    chatview.reload = false
                    chatview.reload = true
                    messageRect.visible = true;
                    if( model.get(currentIndex).newcount>0 && chatview.user_type == 1){ // 只有个人才发已读消息
                        MessageJS.sendNtyMsg(chatview.user_id,chatview.user_type);
                        chatListView.model.setCount(chatview.user_id, 0);
                    }
                    // 将未读的红点去掉,发送回执消息
                    if( model.get(currentIndex).newcount>0){
                        if(model.get(currentIndex).categoryId === 1){ // 只有个人才发已读消息
                            MessageJS.sendNtyMsg(model.get(currentIndex).user_id, model.get(currentIndex).categoryId);
                        }
                        console.log("newcount..."+ model.get(currentIndex).newcount)
                        chatListView.model.setCount(model.get(currentIndex).user_id, 0);
                    }
                }else{
                    topTitle.text = ""
                    grouparea.width=0
                    messageRect.visible = false;
                }
            }

            onDragEnded: {
                if (header.refresh) {
                    console.log("need refresh....")
                    contactListView.model.refresh();
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
                width: contactListView.width
                height: UI.fHItem
                color:contactListView.currentIndex==index? UI.cItemSelected:UI.cItem
                Image{
                    id: photo
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: parent.height/5
                    height: parent.height*3/5
                    width: height
                    fillMode: Image.PreserveAspectFit
                    asynchronous: true
                    source: model.user_photo === undefined ? "" : model.user_photo
                    onProgressChanged: {
                        if(progress == 1)
                            loadingpic.visible = false;
                    }
                    // 默认图片
                    Image{
                        id: loadingpic
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/images/icon/default.png"
                    }


                    Rectangle {
                        id: newTip
                        width: 10
                        height: 10
                        anchors.right: parent.right
                        anchors.rightMargin: -5
                        anchors.top: parent.top
                        anchors.topMargin: -5

                        color: UI.cTransparent
                        visible: model.newcount>0 ? true : false

                        Rectangle {
                            width: {
                                if (parent.width > parent.height) {
                                    return parent.height
                                } else {
                                    return parent.width
                                }
                            }
                            height: width
                            anchors.centerIn: parent
                            radius: {
                                if (width > height) {
                                    return height / 2
                                } else {
                                    return width / 2
                                }
                            }
                            color: UI.cRed
                        }

                    }
                }
                LText{
                    id: msgTitle
                    text: (model.user_remark === undefined || model.user_remark ==="") ?  model.user_name : model.user_remark
                    width: msgItem.width-photo.width-2*UI.fMLsearch
                    anchors.left: photo.right
                    anchors.top: parent.top
                    anchors.leftMargin: UI.fMLsearch
                    anchors.topMargin: parent.height/5
                    maximumLineCount:20
                }

                LText{
                    id: msgContent
                    text: model.last_msg === undefined ? "" : model.last_msg
                    width: msgItem.width-photo.width-2*UI.fMLsearch
                    anchors.left: photo.right
                    anchors.top: msgTitle.bottom
                    anchors.leftMargin: UI.fMLsearch
                    fcolor: UI.cFTB
                    pointSize: UI.TinyFontPointSize
                }


                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton // 激活右键
                    onClicked: {
                        if (mouse.button == Qt.RightButton) { // 右键菜单
                            var pp  = Qt.point(mouse.x,mouse.y)
                            contactMenu.x = pp.x;
                            contactMenu.y = pp.y;
                            contactMenu.open();
                            return;
                        }
                        console.log("onclicked2..."+index);
                        contactListView.currentIndex = -1
                        contactListView.currentIndex = index
                    }
                }

                LMenu {
                    id: contactMenu
                    width: 70
                    LMenuItem {
                        text: qsTr("删除")
                        onTriggered:{
                            console.log("删除");
                            if(index == chatListView.currentIndex){
                                messageRect.visible = false;
                                topTitle.text = "";
                                chatListView.currentIndex--;
                            }
                            chatListView.model.remove(index);

                        }
                    }
                }

            }
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
            id: messageRect
            color: UI.cTransparent
            anchors.top: rightTop.bottom
            anchors.right: parent.right
            width: parent.width
            height: parent.height-rightTop.height
            visible: contactListView.currentIndex >=0?true:false
            Row{
                anchors.fill: parent
                Rectangle{
                    color: UI.cTransparent
                    height: parent.height
                    width: parent.width -grouparea.width

                    Rectangle{
                        id: rightCenter
                        width: parent.width
                        height: parent.height-rightBottom.height
                        anchors.left: parent.left
                        anchors.top: parent.top
                        color:UI.cWhite
                        border.width: 1
                        border.color: UI.cTBBorder
                        z: rightbar.z+1

                        ChatShow{
                            id: chatview
                            anchors.fill: parent
                            MouseArea{
                                anchors.fill: parent
                                propagateComposedEvents: true
                                onPressed: {
                                    mouse.accepted = false
                                }
                            }
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
                        enabled: contactListView.currentIndex >=0?true:false
                        Rectangle{
                            id: toolBar
                            width: parent.width
                            height: UI.fHSendTool
                            color: UI.cMainCBg
                            ChatToolBar{
                                id: chattool
                                anchors.fill: parent
                                onSignalClick:{
                                    if(type ==1 ){    // 文字
                                        chatview.ctype = 4;
                                        console.log(type+"|"+imgName+"|"+strPath);
                                    }
                                    else if(type ==2 ){ // 图片
                                        chatview.ctype = 5;
                                        // 将图片路径转成base64发送
                                        console.log("message send image path:"+strPath)
                                        MessageJS.sendMsg(strPath,chatview.user_type,chatview.ctype);

                                    }
                                    else if(type ==3 ){ // 文档
                                        // 文档先调用上传，再发送云文件
                                        chatview.ctype = 31;
                                        // 1.上传文件
                                        var ret = utilityControl.uploadMaterial(API.api_upload_file,strPath,"mFile",0) // 0为会话上传，1为云上传，消息获取区分用
                                        if(!ret){
                                            console.log("upload file failed...");
                                        }

                                    }
                                    else if(type ==4 ){ // 云

                                    }
                                    else if(type ==5 ){ // 截屏

                                    }
                                }
                            }
                        }

                        LTextArea{
                            id: sendText
                            width: parent.width
                            height: rightBottom.height - toolBar.height - sendBtn.height
                            anchors.left: parent.left
                            anchors.top: toolBar.bottom
                            selectByMouse: true
                            Accessible.name: "document"
                            //baseUrl: "qrc:/images/yibanface"
                            text: chattool.document.text
                            textFormat: Qt.RichText
                            Component.onCompleted: forceActiveFocus()
                            property point clickPos: "0,0"
                            onLinkActivated: Qt.openUrlExternally(link)
                            persistentSelection: true
                            Shortcut {
                                sequence: "Ctrl+Return"
                                onActivated:  {
                                    console.log("发送消息");
                                    chatview.ctype = 4; // 文字和表情都是文字，图片直接发送
                                    var sendtxt = chattool.document.transferText;
                                    if(sendtxt.length==0){
                                        tips.text = qsTr("请输入要发送的内容")
                                        return
                                    }
                                    MessageJS.sendMsg(sendtxt,chatview.user_type,chatview.ctype); // user_type1为个人 3为群组
                                }
                            }
                            Shortcut {
                                sequence: "Ctrl+Enter"
                                onActivated:  {
                                    console.log("发送消息");
                                    chatview.ctype = 4; // 文字和表情都是文字，图片直接发送
                                    var sendtxt = chattool.document.transferText;
                                    if(sendtxt.length==0){
                                        tips.text = qsTr("请输入要发送的内容")
                                        return
                                    }
                                    MessageJS.sendMsg(sendtxt,chatview.user_type,chatview.ctype); // user_type1为个人 3为群组
                                }
                            }
                            Timer {
                                id: keytimer
                                interval: 5000
                                repeat: false
                                onTriggered:{
                                    tips.text = "";
                                    stop();
                                }
                            }

                            MouseArea{
                                anchors.fill: parent
                                propagateComposedEvents: true
                                cursorShape: Qt.IBeamCursor
                                acceptedButtons: Qt.LeftButton | Qt.RightButton // 激活右键
                                onPressed: {
                                    gUserView.visible = false;
                                    if (mouse.button == Qt.LeftButton) { // 右键菜单
                                        mouse.accepted = false
                                    }
                                }
                                onClicked: {
                                    gUserView.visible = false;
                                    if (mouse.button == Qt.RightButton) { // 右键菜单
                                        sendText.clickPos  = Qt.point(mouse.x,mouse.y)
                                        copyMenu.open();
                                    }
                                }
                            }

                            Keys.onReturnPressed: {
                                if( gUserView.visible && grpMemberListModelFilter.count>0){
                                    var sendtxt = chattool.document.transferText;
                                    var lastIdx = sendtxt.lastIndexOf("@");
                                    var tiptxt = sendtxt.substring(lastIdx+1);
                                    chattool.document.insertText(grpMemberListModelFilter.get(subtipslistview.currentIndex).user_name.replace(tiptxt,"")+" ")
                                    sendText.focus = true;
                                    gUserView.visible = false;
                                }else{
                                    event.accepted = false;
                                }
                            }
                            Keys.onEnterPressed: {
                                if( gUserView.visible && grpMemberListModelFilter.count>0){
                                    var sendtxt = chattool.document.transferText;
                                    var lastIdx = sendtxt.lastIndexOf("@");
                                    var tiptxt = sendtxt.substring(lastIdx+1);
                                    chattool.document.insertText(grpMemberListModelFilter.get(subtipslistview.currentIndex).user_name.replace(tiptxt,"")+" ")
                                    sendText.focus = true;
                                    gUserView.visible = false;
                                }else{
                                    event.accepted = false;
                                }
                            }
                            Keys.onDownPressed: {
                                if(gUserView.visible && subtipslistview.currentIndex < (subtipslistview.model.count-1) && (chatview.user_type+'') == "3"){
                                    subtipslistview.currentIndex++
                                    console.log("++")
                                }
                            }
                            Keys.onUpPressed: {
                                if(gUserView.visible && subtipslistview.currentIndex > 0 && (chatview.user_type+'') == "3"){
                                    subtipslistview.currentIndex--
                                    console.log("--")
                                }
                            }

                            onTextChanged: {
                                if( (chatview.user_type+'') === "3"){
                                    var sendtxt = chattool.document.transferText;
                                    // 取最后一个@
                                    if(sendtxt.length>0){
                                        var lastchar = sendtxt.charAt(sendtxt.length - 1);
                                        var lastIdx = sendtxt.lastIndexOf("@");
//                                        console.log(lastchar)
//                                        console.log(lastIdx)
                                        if(lastchar === "@"){
                                            MessageJS.search("")
                                            gUserView.x = sendText.positionToRectangle(chattool.document.cursorPosition).x+10;
                                            gUserView.y = sendText.positionToRectangle(chattool.document.cursorPosition).y - gUserView.height
                                            gUserView.visible = true;
                                        }
                                        else if(gUserView.visible && lastIdx >= 0){
                                            console.log(sendtxt.substring(lastIdx+1))
                                            MessageJS.search(sendtxt.substring(lastIdx+1))
                                            gUserView.x = sendText.positionToRectangle(chattool.document.cursorPosition).x+10;
                                            gUserView.y = sendText.positionToRectangle(chattool.document.cursorPosition).y - gUserView.height
                                            console.log(grpMemberListModelFilter.count)
                                            if(grpMemberListModelFilter.count == 0)
                                                gUserView.visible = false;
                                        }else{
                                            gUserView.visible = false;
                                        }
                                    }else{
                                        gUserView.visible = false;
                                    }

                                }
                            }

                            Rectangle{
                                id: gUserView
                                visible: false
                                border.width: 1
                                border.color: UI.cMainCBg
                                width: 180
                                height: Math.min(grpMemberListModelFilter.count*26,150)
                                ListView {
                                    id: subtipslistview
                                    currentIndex: -1
                                    anchors.left: parent.left
                                    anchors.leftMargin: 1
                                    anchors.top: parent.top
                                    anchors.topMargin: 1
                                    width: parent.width-2
                                    height: parent.height-2

                                    model: grpMemberListModelFilter
                                    clip: true
                                    maximumFlickVelocity: 5000
                                    orientation: ListView.Vertical
                                    focus: true
                                    spacing: 0
                                    delegate: popupUsrDelegate

                                    ScrollIndicator.vertical: ScrollIndicator { }
                                }
                                Component{
                                    id: popupUsrDelegate
                                    Rectangle{
                                        width: gUserView.width
                                        height: 25
                                        color: subtipslistview.currentIndex==index ? UI.cItemSelected:UI.cItem
                                        LText{
                                            anchors.fill: parent
                                            anchors.leftMargin: 10
                                            text: user_name
                                            color: subtipslistview.currentIndex == index ? "red" : "black"
                                        }
                                        MouseArea{
                                            anchors.fill: parent
                                            onClicked: {
                                                var sendtxt = chattool.document.transferText;
                                                var lastIdx = sendtxt.lastIndexOf("@");
                                                var tiptxt = sendtxt.substring(lastIdx+1);
                                                chattool.document.insertText(grpMemberListModelFilter.get(0).user_name.replace(tiptxt,"")+" ")
                                                sendText.focus = true;
                                                gUserView.visible = false;

                                            }
                                        }
                                    }
                                }
                            }

                            ListModel{
                                id: grpMemberListModelFilter
                            }

                            LMenu {
                                id: copyMenu
                                x: sendText.clickPos.x
                                y: sendText.clickPos.y
                                z: sendText.z +10
                                width: 50
                                LMenuItem {
                                    text: qsTr("复制")
                                    height: 25
                                    onTriggered:{
                                        console.log("复制");
                                        // 复制
                                        //                                        utilityControl.copy();
                                        sendText.copy();
                                    }
                                }
                                LMenuItem {
                                    text: qsTr("剪切")
                                    height: 25
                                    onTriggered:{
                                        console.log("剪切");
                                        // 剪切
                                        sendText.cut();
                                    }
                                }
                                LMenuItem {
                                    text: qsTr("粘贴")
                                    height: 25
                                    onTriggered:{
                                        console.log("粘贴");
                                        // 粘贴
                                        sendText.paste();
                                    }
                                }
                            }

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
                                chatview.ctype = 4; // 文字和表情都是文字，图片直接发送
                                var sendtxt = chattool.document.transferText;
                                if(sendtxt.length==0){
                                    tips.text = qsTr("请输入要发送的内容")
                                    return
                                }
                                MessageJS.sendMsg(sendtxt,chatview.user_type,chatview.ctype); // user_type1为个人 3为群组
                            }
                        }

                        Connections {
                            target: ryControl
                            onSendImageFailed:{
                                tips.text = "发送图片失败，请重试！"
                                // 更新数据库为-1
                                ryControl.updateMsgStatus(messageid,-1)
                            }

                            onReceivedMsg: {
                                switch(type){
                                case 0: // 其他
                                    tips.text = ""
                                case 2: // 输入
                                    tips.text = msg
                                    keytimer.start();
                                    break;
                                case 3: // 最后发送时间，已读
//                                    tips.text = msg
                                    var contentJson = JSON.parse(msg);
                                    console.log("contentJson:"+contentJson);
                                    var lasttime = contentJson.lastMessageSendTime+'';
                                    console.log("lastMessageSendTime:"+lasttime);
                                    messageid = lasttime.substr(0,10);
                                    chatview.chatListModel.updateMsgStatusByLastTime(messageid,settings.user_id,targetid,sendtime,2) // sendtime当成rcvtime 2为已读
                                    break;
                                case 4: //文字
                                case 31: //云库
                                case 5: //图片
                                case 6: // 语音
                                    // 添加到所有的聊天记录
                                    chatview.chatListModel.addMessage(msgUid,messageid,settings.user_name,senderid,msg,targetid,1,type,sendtime);
                                    // 更新左侧会话列表，将此对话置顶，可能要将联系人基本信息存本地，定时更新
                                    // 如果是图片
                                    if(type==5)
                                        msg = qsTr("[图片]");
                                    if(type==6)
                                        msg = qsTr("[语音]");
                                    if(type==31)
                                        msg = qsTr("[云库文件]");

                                    if(isMetionedMe){ //@我
                                        msg = "[有人@我] " + msg;
                                    }

                                    if(conversationType == 1) // 单人
                                    {

                                        if(senderid == chatview.user_id && main.active){ // 如果当前对话框是发消息者,且窗体显示的情况下，则直接回发已收
                                            MessageJS.sendNtyMsg(senderid,conversationType);
                                            chatListView.model.setCount(senderid, 0);
                                        }
                                        else{ // 不在当前会话列表
                                            if(!chatListView.model.addContactById(senderid, msg,1))  // 不在会话列表根据发送者获取基本信息
                                                MessageJS.getUserInfoById(senderid, msg)
                                        }
                                    }
                                    else if(conversationType == 3) // 群聊
                                    {
                                        if(!chatListView.model.addContactById(targetid, msg,1)) // 不在会话列表根据发送者获取基本信息
                                            MessageJS.getGroupInfoById(targetid, msg)

                                    }
                                    // 让选中的人还是被选中
                                    MessageJS.setCurrentIdx();
                                    tips.text = "";
                                    msgSound.play();
                                    break;
                                }
                            }
                            onReceivedException:{
                                console.log("exception code:"+ code)
                                if(code == "0")
                                    tips.text = "通讯正常";
                                else
                                    tips.text = "通讯故障"
                            }
                            onProceeFile:{
                                tips.text = "图片上传："+process+"%"
                            }
                            onUploadFileCallback:{
                                tips.text = "图片成功"
                                // 更新消息内容
                                chatview.chatListModel.updateMsgContent(msgid, content);
                            }
                            onSendMsgDealCallback:{
                                // 消息发送成功，入库
                                console.log("消息发送成功，入库")
                                chatview.chatListModel.updateMsgStatus(msgid, result);

                            }
                        }

                        Connections{
                            target: utilityControl
                            onCaptureSuccessed:{
                                console.log("screen shoot path:"+path);
                                //  发送本地图片
                                chatview.ctype = 5;
                                // 将图片路径转成base64发送
                                console.log("message send image path:"+path)
                                MessageJS.sendMsg(path,chatview.user_type,chatview.ctype);
                            }
                        }

                    }
                }
                Rectangle {
                    id: grouparea
                    color: UI.cMainCBg
                    width: UI.fWGroup;
                    height: parent.height
//                    visible: false
                    LText{
                        id: grouptitle
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.top: parent.top
                        width: parent.width
                        height: 40
                        text: qsTr("群成员（0）")
                        color: "#7E7E7E"
                    }
                    ListModel{
                        id: grpMemberListModel
                    }

                    ListView {
                        id: groupListView
                        currentIndex: -1
                        anchors.left: parent.left
                        anchors.top: grouptitle.bottom
                        width: parent.width
                        height: parent.height-grouptitle.height

                        model: grpMemberListModel//chatPersionmodel
                        clip: true
                        maximumFlickVelocity: 5000
                        orientation: ListView.Vertical
                        focus: true
                        highlightFollowsCurrentItem: false
                        spacing: 2
                        delegate: groupDelegate
                        ScrollIndicator.vertical: ScrollIndicator { }
                    }

                    Component{
                        id: groupDelegate
                        Rectangle{
                            id: groupItem
                            width: groupListView.width
                            height: gusername.contentHeight * 1.5
                            color:groupListView.currentIndex==index? UI.cRightBg : UI.cTransparent
                            Image{
                                id: guserphoto
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                anchors.top: parent.top
                                anchors.topMargin: (parent.height-height)/2
                                height: parent.height*2/3
                                width: height
                                fillMode: Image.PreserveAspectFit
                                source: user_photo
                            }
                            LText{
                                id: gusername
                                anchors.left: guserphoto.right
                                anchors.leftMargin: 5
                                anchors.top: parent.top
                                anchors.topMargin: (parent.height-height)/2
                                pointSize: UI.TinyFontPointSize
                                text: user_name
                                height: contentHeight
                            }

                            MouseArea{
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton // 激活右键（别落下这个）
                                onClicked: {
                                    groupListView.currentIndex = index
                                    if (mouse.button == Qt.RightButton) { // 右键菜单
                                        contentMenu.open()
                                    }
                                }
                                onDoubleClicked: {
                                    if (mouse.button == Qt.RightButton) // 右键双击不算
                                        return;
                                    // 如果是自己则不操作
                                    if(user_id == settings.user_id)
                                        return;

                                    // 判断是否已经存在
                                    var idx = -1;
                                    console.log(user_id)
                                    for(var i=0; i<chatListModel.rowCount(); i++){
                                        console.log(chatListModel.getId(i))
                                        if(chatListModel.getId(i) === user_id){
                                            idx = i;
                                            break;
                                        }
                                    }
                                    console.log(idx)

                                    //添加或更新
                                    if(idx>-1){
                                        chatListView.model.updateContacts(idx,"")
                                    }else{
                                        chatListView.model.addContacts(user_id,user_name,"",user_photo,"",1,0) // 单人，群成员可能不是好友，没有remark
                                    }

                                    message.chatListView.currentIndex = -1; // 引起change事件
                                    message.chatListView.currentIndex = 0;
                                    message.visible = true;
                                    friend.visible = false;
                                }
                            }
                            LMenu {
                                id: contentMenu
                                y: groupItem.height
                                x: 10
                                width: groupItem.width -10
                                LMenuItem {
                                    text: "@"+user_name
                                    onTriggered:{
                                        chattool.document.insertText(" @"+user_name+" ")
                                        sendText.focus = true;
                                    }
                                }
                                LMenuItem {
                                    text: "发消息"
                                    onTriggered:{

                                        // 判断是否已经存在
                                        var idx = -1;
                                        console.log(user_id)
                                        for(var i=0; i<chatListModel.rowCount(); i++){
                                            console.log(chatListModel.getId(i))
                                            if(chatListModel.getId(i) === user_id){
                                                idx = i;
                                                break;
                                            }
                                        }
                                        console.log(idx)

                                        //添加或更新
                                        if(idx>-1){
                                            chatListView.model.updateContacts(idx,"")
                                        }else{
                                            chatListView.model.addContacts(user_id,user_name,"",user_photo,"",1,0) // 单人，群成员可能不是好友，没有remark
                                        }

                                        message.chatListView.currentIndex = -1; // 引起change事件
                                        message.chatListView.currentIndex = 0;
                                        message.visible = true;
                                        friend.visible = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    DropArea{
        anchors.fill: parent;
        onEntered:{
            // Ensure at least one file is supported before accepted the drag
            var validFile = false;
            if(messageRect.visible){ // 显示的时候才能发送
                for(var i = 0; i < drag.urls.length; i++) {
                    if(API.validateFileExtension(drag.urls[i])) {
                        validFile = true;
                        break;
                    }
                }
            }
            if(!validFile) {
                drag.accepted = false;
                return false;
            }
            tips.text = qsTr("释放鼠标发送文件");
        }
        onExited: {
            tips.text = qsTr("");
        }

        onDropped: {
            if(drop.hasUrls){
                MessageJS.selectFiles(drop.urls)
            }
        }
    }
}
