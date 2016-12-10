import QtQuick 2.7
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API
import "qrc:/js/login.js" as LoginJS
import "qrc:/js/Cloud.js" as CloudJS

Window {
    id:login
    visible: true
    width: 472
    height: 623
    title: qsTr("登录")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;

    color: UI.cWhite    // XP
    // color: UI.cTransparent
    property bool isCodeLogin: false    // 是否验证码登录
    property bool isLogin: false
    property bool autoLogin: false

    Component.onCompleted: {
        LoginJS.getConfig();
        if(settings.token==""){
            isCodeLogin = true;
        }
    }

    onAutoLoginChanged: {
        if(autoLogin && !utilityControl.checkIdIsLogin("9527198609")){
            // 自动登录,还要是否已经有登录过账号, 9527198609这个账号专门用来判断是否有账号登录了
            LoginJS.loginLocal();
        }
    }

    onClosing:{
        ryControl.disconnect();
    }

    LMessageDialog{
        id: smsTipDialog
        visible: false
        msg: qsTr("收到验证码了吗？")
        flag: 3
        okTitle: "是"
        cancelTitle: "否"
        onCancelClicked:{
            // 则调用发送语音验证码接口并提示“将通过语音电话为你播报验证码，请注意“010”等区号开头的来电号码”，如果点击收到了则恢复正常状态。
            LoginJS.sndVoiceCode();
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
            login.x = login.x+delta.x
            login.y = login.y+delta.y
        }
    }

    Rectangle{
        anchors.fill: parent;
        radius: 4;
        color: UI.cTransparent
        Image{
            anchors.fill: parent
            source: "qrc:/images/icon/login_bg.png"
        }

        LText{
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 10
            anchors.topMargin: 10
            text: qsTr("圈")
            font.pointSize: UI.HugeFontPointSize
            color: UI.cLoginQ
        }
    }

    Rectangle{
        id:closeBtn
        height: UI.fHLoginClose
        width: height
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 10
        anchors.topMargin: 10
        color: UI.cTransparent
        Image {
            id: close
            anchors.centerIn: parent
            width: 20
            height: width
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/icon/close_login.png"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onClicked:
            {
//                utilityControl.quit()
                Qt.quit();
            }
            onEntered: {
                close.source = "qrc:/images/icon/close_loginp.png"
            }
            onExited: {
                close.source = "qrc:/images/icon/close_login.png"
            }
        }
    }

    Rectangle{
        id:photo
        width:parent.width/3
        height:width
        anchors.top: parent.top
        anchors.topMargin: UI.fHLoginM
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        LImage {
            id: photop
            anchors.fill: parent
            source: settings.user_photo == "" ? "qrc:/images/qt-logo.png" : settings.user_photo
            picname: settings.user_id
        }
    }

    Rectangle{
        id:name
        width:photo.width
        height:txtName.contentHeight
        anchors.top: photo.bottom
        anchors.topMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        color: UI.cTransparent
        visible:  !isCodeLogin
        LText{
            id:txtName
            text: settings.user_name //qsTr("刘其超")
            anchors.centerIn: parent
        }
    }

    Rectangle{
        id:btnlogin
        color: UI.cLoginBtnBg
        width: parent.width*5/7
        height: UI.fHLoginBtn
        anchors.top: name.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        radius: 5
        visible: !isCodeLogin
        enabled: settings.token == "" ? false : true
        LText{
            id:txtLogin
            text:qsTr("登录")
            anchors.centerIn: parent
            color: UI.cWhite
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                LoginJS.loginLocal();
            }
            onEntered: {
                btnlogin.color = UI.cLoginQP
            }
            onExited: {
                btnlogin.color = UI.cLoginBtnBg
            }
        }
    }

    Rectangle{
        id:exchange
        width:photo.width
        height:title.contentHeight
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        anchors.left: parent.left
        anchors.leftMargin: (parent.width-width)/2
        color: UI.cTransparent
        LText{
            id:title
            text: isCodeLogin ? qsTr("返回快捷登录") : qsTr("切换账号")
            anchors.centerIn: parent
            color: UI.cExchange
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if(title.text=="切换账号"){
                        isCodeLogin = true;
                    }else{
                        isCodeLogin = false;
                    }
                }

                onEntered: {
                    title.color = UI.cLightBlue
                }
                onExited: {
                    title.color = UI.cExchange
                }
            }
        }
    }

    Rectangle{
        id: codeArea
        color: UI.cTransparent
        width: parent.width
        height: parent.height/2
        anchors.bottom: parent.bottom
        visible: isCodeLogin
        Item{
            width: parent.width*5/7
            height: UI.fHLoginBtn
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: (parent.width-width)/2
            Column{
                spacing: 5
                Row {
                    spacing: 2
                    LText{
                        id: phonel
                        height: UI.fHLoginBtn
                        text: qsTr("手机号")
                    }
                    LTextInput{
                        id: phone
                        width: btncodelogin.width-codebtn.width-phonel.width-2*2;
                        height: UI.fHLoginBtn
                        maximumLength: 11
                        placeholderText: qsTr("请输入手机号")
                        text: settings.user_mobile

                    }
                    LButton{
                        id: codebtn
                        width: 100;
                        height: UI.fHLoginBtn
                        text: qsTr("获取验证码");
                        fontSize: UI.TinyFontPointSize
                        onClicked: {
                            LoginJS.sndVCode();
                        }
                    }
                    Timer{
                        id: counttimer;
                        property int count: 60
                        interval: 1000;
                        running: false;
                        repeat: true
                        property bool isVoice: false
                        onTriggered: {
                            count--;
                            if(count==0){
                                count = 60;
                                counttimer.stop();
                                codebtn.enabled = true;
                                codebtn.text = qsTr("获取验证码");
                                tips.text = "";
                                if(!isVoice) // 是否当前是语音在倒计时
                                {
                                    smsTipDialog.msg = qsTr("收到验证码了吗？")
                                    smsTipDialog.flag = 3
                                    smsTipDialog.okTitle = "是"
                                    smsTipDialog.cancelTitle = "否"
                                    if(smsTipDialog.visible)
                                        smsTipDialog.requestActivate();
                                    else
                                        smsTipDialog.show();
                                }
                                isVoice = false;

                            }
                            else{
                                codebtn.text = qsTr(count+"秒后可重新获取");
                            }
                        }
                    }
                }
                Row{
                    Rectangle{
                        width: btncodelogin.width;
                        height: 1
                        color: UI.cLightBlue
                        border.width: 1
                        border.color: UI.cLightBlue
                    }
                }
                Row {
                    LText{
                        id: codel
                        height: UI.fHLoginBtn
                        text: qsTr("验证码")
                    }
                    LTextInput{
                        id: code
                        width: btncodelogin.width-codel.width-2;
                        height: UI.fHLoginBtn
                        maximumLength: 6
                        placeholderText: qsTr("请输入验证码")
                    }
                }
                Row{
                    Rectangle{
                        width: btncodelogin.width
                        height: 1
                        color: UI.cLightBlue
                        border.width: 1
                        border.color: UI.cLightBlue
                    }
                }
                Row{
                    LText{
                        id: tips
                        width: btncodelogin.width
                        height: UI.fHLoginBtn
                        color: UI.cRed
                    }
                }
            }
        }

        Rectangle{
            id:btncodelogin
            color: UI.cLoginBtnBg
            width: parent.width*5/7
            height: UI.fHLoginBtn
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 35+UI.fHLoginBtn
            anchors.left: parent.left
            anchors.leftMargin: (parent.width-width)/2
            radius: 5
            LText{
                text:qsTr("登录")
                anchors.centerIn: parent
                color: UI.cWhite
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    LoginJS.loginfuc(phone.text,code.text);
                }
                onEntered: {
                    btncodelogin.color = UI.cLoginQP
                }
                onExited: {
                    btncodelogin.color = UI.cLoginBtnBg
                }
            }
        }

    }

}
