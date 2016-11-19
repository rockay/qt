import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQml.Models 2.2
import QtMultimedia 5.6
import QtQuick.Dialogs 1.2

import org.lt.db 1.0
import org.lt.controls 1.1

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/convertuni.js" as ConvertJS


Rectangle{
    id: chatroot
    color: UI.cTransparent
    property SqlConversationModel chatListModel: listView.model
    property string user_id: ""     // 当前聊天窗口的对象ID
    property int user_type: 1       // 当前聊天窗口的类别1为个人，3为群组
    property string user_photo:""
    property int ctype: 1        // 发送图片还是文字，与融云匹配
    property ListView converListView: listView
    property bool reload: false

    // type 文字 2：图片 3：文档 4：云 5截屏
    signal signResendMsg(string msgid, int type, string content);

    ListModel{
        id: imgList
    }

    onUser_idChanged: {
        // 切换聊天窗口的数据
        listView.model.targetid = user_id
    }

    Connections{ //ryControl
        target: ryControl
        onSendMsgCallback:{
            console.log("sendmsg return json:"+retJson)
        }
    }

    Connections{ // listView.model
        target: listView.model
        onCountChanged:{
            imgList.clear()
            for(var i=0; i <listView.model.count; i++){
                // 添加图片数组
                if(listView.model.get(i).ctype === 5){
                    var idx = listView.model.get(i).message.split('|')[0].lastIndexOf(".")
                    var localpath = listView.model.get(i).message.split('|')[0];
                    var file_ext = localpath.substring(idx+1);
                    var filepath = ""
                    var fileurl = "" ;
                    if(listView.model.get(i).result==0 || listView.model.get(i).result==-1){
                        fileurl = "" ;
                        filepath = listView.model.get(i).message.split('|')[0];
                    }
                    else{
                        filepath = listView.model.get(i).message.split('|')[2] ;
                        fileurl = listView.model.get(i).message.split('|')[1] ;
                    }
                    imgList.append({"path": filepath,"file_ext":file_ext,"url": fileurl})
                }
                else if(listView.model.get(i).ctype === 31 && listView.model.get(i).message.split('|')[1].toString() === "1"){
                    var filepath1 = listView.model.get(i).message.split('|')[4];
                    var file_ext1 = listView.model.get(i).message.split('|')[0];
                    imgList.append({"path": filepath1,"file_ext":file_ext1,"url": filepath1})

                }
            }
            // 滚到最新的消息处
            chatviewp.converListView.positionViewAtEnd();
        }

        onNeedRefresh:{
            chatviewp.converListView.positionViewAtEnd();
        }
    }


    FileDialog {
        id: saveFileDialog
        folder: shortcuts.pictures
        selectExisting: false
        property string httpurl: ""
        property string file_ext: "pdf"
        onAccepted: {
            var urlNoProtocol = (fileUrl+"."+file_ext).replace('file:///', '');
            if(httpurl!=""){
                networkControl.doDownload(httpurl,urlNoProtocol);
            }
        }
    }

    SoundEffect {
        id: play
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        verticalLayoutDirection: ListView.TopToBottom //ListView.BottomToTop
        spacing: 10
        model: SqlConversationModel{}//chatList
        delegate: conversationDelegate
        cacheBuffer: 50 * 20
        clip: true
        ScrollBar.vertical: ScrollBar {}
    }

    Component{
        id: conversationDelegate
        Column {
            id: messagecolumn
            property bool sentByMe: (model.senderid == API.user_id) ? true : false
            spacing: 10
            Component.onCompleted: {
                anchors.right = sentByMe ? parent.right : undefined
            }

            LText {
                id: timestampText
                text: model.timestamp.substring(0,16)
                color: UI.cTime
                horizontalAlignment: Text.Center
                width: listView.width
                anchors.right: sentByMe ? parent.right : undefined
                visible: (listView.model.get(index-1).timestamp===undefined || model.timestamp === undefined)
                         ? true : API.compareDate(listView.model.get(index-1).timestamp,model.timestamp)<5 ? false : true
            }

            Row {
                id: messageRow
                spacing: 4
                anchors.right: sentByMe ? parent.right : undefined

                LImage {
                    id: you
                    height: UI.fChatImgH
                    width: height
                    picname: model.senderid
                    source: !sentByMe ? (chatroot.user_type==1 ? chatroot.user_photo : API.photoObjMap[model.senderid] == undefined? "":API.photoObjMap[model.senderid]) : ""
                }

                Rectangle { // textarea
                    id: textarearect
                    width: Math.min(model.ctype == 4 ? (hidemessageText.implicitWidth+ 24 ) : model.ctype == 5 ? (messageImg.width + 24) : model.ctype == 6 ? 120 : model.ctype == 31 ? 250: (hidemessageText.width + 24)
                                    , listView.width - 2*UI.fChatImgH - messageRow.spacing-24)
                    height:{
                        model.ctype == 4 ? hidemessageText.implicitHeight + 14 : model.ctype == 5 ? (messageImg.height + 24) :  model.ctype == 6 ? 40 : model.ctype == 31? 70 : (hidemessageText.height + 24)
                    }
                    color: UI.cTransparent //sentByMe ? UI.cRightBg : UI.cWhite
                    border.color: UI.cLeftBorder
                    border.width: 0

                    //                        rightAligned: sentByMe ? true : false
                    anchors.leftMargin:  sentByMe ? UI.fChatImgH : 0
                    anchors.rightMargin: !sentByMe ?UI.fChatImgH : 0
                    Rectangle{
                        id: resultRect
                        anchors.left: sentByMe ? parent.left : undefined
                        anchors.right: !sentByMe ? parent.right : undefined
                        anchors.leftMargin:  sentByMe ? -UI.fChatImgH+10 : 0
                        anchors.rightMargin: !sentByMe ? -UI.fChatImgH+10 : 0
                        anchors.top: parent.top
                        anchors.topMargin: (parent.height-height)/2
                        color: model.result === -1 ? UI.cRed : UI.cTransparent
                        width: 20
                        height: width
                        radius: model.result === -1 ? parent.width/2 : 0
                        visible: (model.result === -1 || (model.result === 2 && user_type===1))? true : false
                        LText{
                            anchors.centerIn: parent
                            text: model.result === -1 ? qsTr("!") : model.result === 2 ? "已读":""
                            color:  model.result === -1 ? UI.cWhite : UI.cBlue
                            pointSize: model.result === -1 ? UI.SmallFontPointSize : UI.TinyFontPointSize
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                // 重新发送，判断消息类型
                                switch(model.ctype){
                                case 4: // 文字
                                    signResendMsg(model.messageid, 4,model.message)
                                    break;
                                case 5: //  图片
                                    signResendMsg(model.messageid, 5,model.message.split('|')[0])
                                    break;
                                case 6: //  语音
                                    break;
                                case 31: //  云库
                                    signResendMsg(model.messageid, 31,model.message)
                                    break;
                                }

                            }
                        }

                    }

                    TextBalloon {
                        id: txtBallon
                        anchors.fill: parent
                        rightAligned: sentByMe ? true : false
                        LText {
                            id: hidemessageText
                            text: model.message
                            color: sentByMe ? UI.cChatFont : UI.cChatFont
                            anchors.fill: parent
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                            textFormat: Text.RichText
                            visible: false
                        }
                        LTextArea {
                            id: messageText
                            text: model.message
                            color: sentByMe ? UI.cChatFont : UI.cChatFont
                            anchors.fill: parent
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                            visible: model.ctype == 4 ? true : false
                            selectByMouse: true
                            textFormat: Text.RichText
                            onLinkActivated: Qt.openUrlExternally(link)
                            readOnly : true
                            persistentSelection: true
                            background: Rectangle {
                                anchors.fill: parent
                                color: UI.cTransparent
                            }
                            MouseArea{
                                anchors.fill: parent
                                cursorShape: Qt.ArrowCursor;
                                propagateComposedEvents: true
                                acceptedButtons: Qt.LeftButton | Qt.RightButton // 激活右键
                                onPressed: {
                                    if (mouse.button == Qt.LeftButton) { // 右键菜单
                                        mouse.accepted = false
                                    }
                                    else if (mouse.button == Qt.RightButton && messageText.selectedText !="")
                                    { // 右键菜单
                                        var pp  = Qt.point(mouse.x,mouse.y)
                                        saveMenu.x = pp.x;
                                        saveMenu.y = pp.y;
                                        saveMenu.ctype = 4
                                        saveMenu.httpurl = "";
                                        saveMenu.open()
                                    }
                                }
                            }

                            Menu {
                                id: saveMenu
                                width: 60
                                property int ctype : -1
                                property string httpurl: ""
                                background: Item{
                                    Rectangle{
                                        anchors.fill: parent
                                        color: UI.cWhite
                                        border.width: 1
                                        border.color: UI.cMainCBg
                                    }
                                }

                                MenuItem {
                                    id: copyitem
                                    text: qsTr("复制")
                                    height: saveMenu.ctype == 4 ? 25 : 0
                                    visible: saveMenu.ctype == 4 ? true : false
                                    onTriggered:{
                                        console.log("复制");
                                        // 复制
                                        messageText.copy();
                                    }
                                }
                                MenuItem {
                                    id: saveitem
                                    text: "保存"
                                    height: (saveMenu.ctype == 5 || saveMenu.ctype == 31) ? 25 : 0
                                    visible: (saveMenu.ctype == 5 || saveMenu.ctype == 31) ? true : false
                                    onTriggered:{
                                        console.log("保存")
                                        saveFileDialog.httpurl = saveMenu.httpurl
                                        saveFileDialog.open();
                                    }
                                }
                            }

                            Component.onCompleted: {
                                var transferText = model.message.replace(/\ud83c[\udf00-\udfff]|\ud83d[\udc00-\ude4f]|\ud83d[\ude80-\udeff]/g, function(match){
                                    return "<img src='qrc:/images/emoji/drawable-xhdpi/u"+ConvertJS.getCPfromChar(match).toLowerCase().trim()+".png' width=25 height=25/>";
                                 });
                                messageText.text= transferText ;
                                hidemessageText.text= transferText ;
                            }
                        }
                    }
//                    Image{
//                        id: imgOrignal
//                        visible: false
//                        asynchronous: true
//                        source: model.ctype === 5? ((model.result==-1 || model.result==0) ? "file:///"+model.message.split('|')[0] : "file:///"+model.message.split('|')[2]) : ""
//                    }

                    Image {
                        id: messageImg
                        anchors.margins: 12
                        anchors.centerIn: parent
                        source: model.ctype === 5? "file:///"+model.message.split('|')[0] : ""
                        visible: model.ctype === 5 ? true : false
                        width: sourceSize.width>200 ? 200 : sourceSize.width
                        height: sourceSize.height>200 ? 200 : sourceSize.height
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.LeftButton | Qt.RightButton // 激活右键
                            onClicked: { // 右键菜单
                                if ( mouse.button == Qt.RightButton )
                                { // 右键菜单
                                    console.log("右键菜单保存")
                                    var pp  = Qt.point(mouse.x,mouse.y)
                                    saveMenu.x = pp.x;
                                    saveMenu.y = pp.y;
                                    saveMenu.httpurl = model.message.split('|')[1];

                                    var idx = model.message.split('|')[0].lastIndexOf(".")
                                    var localpath = model.message.split('|')[0];
                                    var file_ext = localpath.substring(idx+1);
                                    saveFileDialog.file_ext =  file_ext;
                                    saveMenu.ctype = model.ctype
                                    saveMenu.open()
                                }
                            }
                            onDoubleClicked: { // 双击预览图片
                                console.log("预览图片:"+model.message.split('|')[1])

                                // 获取当前图片的index
                                var imgIndx = 0;
                                for(var i=0; i<index; i++){
                                    if((listView.model.get(i).ctype === 5) || (listView.model.get(i).ctype === 31 && listView.model.get(i).message.split('|')[1].toString() === "1"))
                                        imgIndx++;
                                }
                                imageshow.curIdx = -1; // 引起事件
                                imageshow.curIdx = imgIndx;

                                imageshow.imgSrc = ""
                                imageshow.imgSrc = (model.result==-1 || model.result==0) ? "file:///"+model.message.split('|')[0] : "file:///"+model.message.split('|')[2];
                                imageshow.imgshowList = imgList
                                imageshow.show();
                                imageshow.requestActivate();
                            }
                        }
                    }

                    Rectangle{
                        id: voiceRect
                        anchors.fill: parent
                        color: UI.cTransparent
                        visible: model.ctype === 6
                        Image {
                            id:  voiceImg
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.top: parent.top
                            anchors.topMargin: (parent.height-height)/2
                            source: model.ctype === 6 ? "qrc:/images/icon/voice.png" : ""
                            visible: model.ctype === 6 ? true : false
                        }

                        LText{
                            id: voice_len
                            anchors.left: voiceImg.right
                            anchors.leftMargin: 10
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            text: model.ctype===6 ? model.message.split('|')[0]+"''" : ""
                        }

                        MouseArea{
                            anchors.fill: parent
                            propagateComposedEvents: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                console.log("语音：" + "file:///"+model.message.split('|')[1])
                                acceptedButtons: Qt.LeftButton // 激活右键
                                if(play.playing)
                                    play.stop();
                                else{
                                    play.source = model.ctype === 6 ? "file:///"+model.message.split('|')[1] : ""
                                    play.play()
                                }
                            }
                        }
                    }

                    Rectangle{ // 云文件发送
                        anchors.fill: parent
                        color: UI.cTransparent
                        visible: model.ctype===31
                        LText{
                            id: file_name
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            width: parent.width - iconrect.width- 20
                            text: model.ctype===31 ? model.message.split('|')[3] : ""
                        }
                        LText{
                            id: file_size
                            width: parent.width - iconrect.width-4
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.top: file_name.bottom
                            anchors.topMargin: 10
                            text: model.ctype===31 ? model.message.split('|')[2]>(1024*1024) ? (model.message.split('|')[2]/1024/1024).toFixed(0)+"M" : (model.message.split('|')[2]/1024).toFixed(0) +"kb"  : ""
                            color: UI.cMainCBg
                        }
                        Rectangle{
                            id: iconrect
                            height: parent.height-10
                            width: height
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            anchors.top: parent.top
                            anchors.topMargin: 5
                            color: UI.cRed
                            radius: 5
                            LText{
                                id: file_ext
                                anchors.centerIn: parent
                                color: UI.cWhite
                                pointSize: UI.BigFontPointSize
                                text: model.ctype===31 ? model.message.split('|')[0].toString().toUpperCase() : ""
                            }
                        }
                        MouseArea{
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton // 激活右键
                            onClicked: { // 右键菜单
                                if (mouse.button == Qt.RightButton) { // 右键菜单
                                    var pp  = Qt.point(mouse.x,mouse.y)
                                    saveMenu.x = pp.x;
                                    saveMenu.y = pp.y;
                                    saveMenu.ctype = model.ctype
                                    saveFileDialog.file_ext =  model.message.split('|')[0].toString().toLowerCase()
                                    saveMenu.httpurl = model.message.split('|')[4]
                                    saveMenu.open()
                                }
                            }
                            onDoubleClicked: {
                                if( model.message.split('|')[1].toString() === "1") //file_mold：文件类型，1 表示图片类型，2 表示 PDF 类型
                                {
                                    imageshow.imgSrc = ""
                                    imageshow.imgSrc = model.ctype===31 ? model.message.split('|')[4] : ""

                                    // 获取当前图片的index
                                    var imgIndx = 0;
                                    for(var i=0; i<index; i++){
                                        if((listView.model.get(i).ctype === 5) || (listView.model.get(i).ctype === 31 && listView.model.get(i).message.split('|')[1].toString() === "1"))
                                            imgIndx++;
                                    }

                                    imageshow.curIdx = -1; // 引起事件
                                    imageshow.curIdx = imgIndx;

                                    imageshow.imgshowList = imgList
                                    imageshow.show()
                                    imageshow.requestActivate();
                                }else if( model.message.split('|')[1].toString() === "2"){
//                                    fileshow.fileUrl = ""
//                                    fileshow.fileUrl = model.message.split('|')[4];
//                                    fileshow.show()
//                                    imageshow.requestActivate();
                                    Qt.openUrlExternally( model.message.split('|')[4]);
                                }else{
                                    console.log("==unsupport file...")
                                }
                            }
                        }

                    }


                    Rectangle{
                        id: msgMask
                        color: UI.cBlack
                        opacity: 0.5
                        visible: false
                        anchors.fill: parent

                        Connections{ // 上传素材返回
                            target: utilityControl
                            onUpdateProgress:{
                                if(filefrom == 0){
                                    if(messageid == model.messageid){
                                        if(percent==100){
                                            msgMask.visible = false;
                                        }
                                        else if(model.ctype===31){
                                            msgMask.visible = true
                                            uploadprocess.text = percent+"%";
                                            console.log("percent:",percent);
                                        }
                                    }
                                }
                            }
                        }

                        Connections {
                            target: ryControl
                            onProceeFile:{
                                if(messageid == model.messageid){
                                    if(percent==100){
                                        msgMask.visible = false;
                                    }
                                    else if(model.ctype===5){
                                        msgMask.visible = true
                                        uploadprocess.text = percent+"%";
//                                            console.log(uploadprocess.text)
                                    }
                                }
//                                tips.text = "图片上传："+process+"%"
                            }
                        }
                    }

                    LText{
                        id: uploadprocess
                        pointSize: UI.BigFontPointSize
                        color: UI.cWhite
                        anchors.centerIn: parent
                        visible: msgMask.visible
                    }
                }

                Rectangle{
                    height: UI.fChatImgH
                    width: height+UI.fChatMargin
                    color: UI.cTransparent
                    LImage {
                        id: me
                        picname: API.user_id
                        width: parent.height
                        height: parent.height
                        source: sentByMe ? API.user_photo : ""
                    }
                }

            }

        }
    }

}
