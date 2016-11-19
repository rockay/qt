// 设置登录者的基本信息
function copySettings2API(){
    API.token = settings.token;
    API.rong_token = settings.rong_token;
    API.weix_token = settings.weix_token;
    API.user_id = settings.user_id;
    API.user_mobile = settings.user_mobile;
    API.user_name = settings.user_name;
    API.user_wx_name = settings.user_wx_name;
    API.user_photo = settings.user_photo;
    API.is_vip_user = settings.is_vip_user;
    API.is_company_user = settings.is_company_user;
}

// 获取验证码
function sndVCode(){
    if(phone.text=="" || phone.text.length!=11){
        tips.text = ("请输入正确手机号")
        return;
    }

    counttimer.start();
    codebtn.enabled = false;
    counttimer.isVoice = false;
    // 提交接口修改
    var url = API.api_root+API.api_sndVcode;
    var obj = "user_mobile="+phone.text;
    var verb = "POST";
    API.httpRequest(verb, url, obj, sndVCodeCB);
}

function clearCodeLogin(){
    counttimer.stop();
    counttimer.count = 60;
    codebtn.enabled = true;
    codebtn.text = qsTr("获取验证码");
}

// 获取验证码回调
function sndVCodeCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
//        settings.code = data.rnd_code;
        tips.text = qsTr("验证码短信已发送到手机，请注意查收");
    }else{
        clearCodeLogin();
        var tipmsg = "";
        switch(data.errorcode){
        case 10:
            tipmsg = qsTr("手机号不正确");
            break;
        default:
            tipmsg = qsTr("获取验证码失败");
            break;
        }
        tips.text = tipmsg;
    }
}

// 获取语音验证码
function sndVoiceCode(){
    if(phone.text=="" || phone.text.length!=11){
        tips.text = ("请输入正确手机号")
        return;
    }

    codebtn.enabled = false;
    counttimer.isVoice = true;
    counttimer.start();
    // 提交接口修改
    var url = API.api_root+API.api_voiceCode;
    var obj = "user_mobile="+phone.text;
    var verb = "POST";
    API.httpRequest(verb, url, obj, sndVoiceCodeCB);
}

// 获取语音验证码回调
function sndVoiceCodeCB(data){
//    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
//        settings.code = data.rnd_code;
        tips.text = ("将通过语音电话为你播报验证码，请注意来电");
    }else{
        clearCodeLogin();
        tips.text = ("获取语音验证码失败");
    }
}

// 快速登录
function loginLocal(){
    if(settings.token!=""){
        // 判断验证码是否过期就行了。
        getUserInfoById(settings.user_id);
    }
}

// 根据用户ID获取基本信息
function getUserInfoById(user_id){
    var url = API.api_root+API.api_frienddetail;
    var obj = "token="+settings.token+"&search_key="+user_id;
    var verb = "POST";
    console.log("obj:"+obj)
    API.httpRequest(verb, url, obj, getUserInfoByIdCB);
}

// 获取好友信息回调
function getUserInfoByIdCB(data){
//    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        if(utilityControl.checkIdIsLogin(settings.user_id)){
            smsTipDialog.msg = qsTr("账户已经登录!")
            smsTipDialog.flag = 1;
            smsTipDialog.okTitle = qsTr("确定")
            smsTipDialog.show();
            return ;
        }
        console.log("快捷登录成功");
        copySettings2API();
        ryControl.initLib(API.rong_token, API.user_id);
        ryControl.connect();
        main.pphoto = API.user_photo;
        main.show();
        login.hide();
        main.reLogin = false;
        main.reLogin = true;
        clearCodeLogin();
        isLogin = true;
    }else if(data.errorcode === 10){
        isCodeLogin = false;
        isCodeLogin = true;
        isLogin = false;
        tips.text = "帐号在其他设备登录，请用验证码重新登录";
        ryControl.disconnect();
        utilityControl.releseAccount();
    }
    else{
        smsTipDialog.msg = qsTr("登录失败，未获取到该账户信息，请重试!")
        smsTipDialog.flag = 1;
        smsTipDialog.okTitle = qsTr("确定")
        smsTipDialog.show();
        tips.text = "登录失败，未获取到该账户信息";
        ryControl.disconnect();
        utilityControl.releseAccount();
    }
}

// 验证码登录
function loginfuc(phone,scode){
    if(phone=="" || phone.length!=11 || scode=="" || scode.length!=6){
        tips.text = ("请输入正确的手机号和验证码")
        return;
    }
    tips.text = "正在登录...";
    btncodelogin.enabled = false;

    var url = API.api_root+API.api_login;
    var obj = "user_mobile="+phone+"&rnd_code="+scode+"&user_device=3"; // 1 表示 IOS,2 表示 Android 3 表示windows
    var verb = "POST";
    API.httpRequest(verb, url, obj, loginCB);

}

// 验证码登录回调
function loginCB(data){
    btncodelogin.enabled = true;
//    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        clearCodeLogin();
        code.text = "";
        isLogin = true;
        console.log("登录成功");
        settings.token = data.token;
        settings.rong_token = data.rong_token;
        settings.weix_token = data.weix_token;
        settings.user_id = data.user_info.user_id;
        settings.user_mobile = data.user_info.user_mobile;
        settings.user_name = data.user_info.user_name;
        settings.user_wx_name = data.user_info.user_wx_name;
        settings.user_photo = data.user_info.user_photo;
        settings.is_vip_user = data.user_info.is_vip_user;
        settings.is_company_user = data.user_info.is_company_user;

        copySettings2API();
        tips.text = "";
        dbControl.initDB(API.user_id)
        ryControl.initLib(API.rong_token, API.user_id);
        ryControl.connect();
        main.pphoto = API.user_photo;
        main.reLogin = false;
        main.reLogin = true;
        main.show();
        login.hide();


    }else{
        var tipmsg = "";
        isLogin = false;
        switch(data.errorcode){
        case 10:
            tipmsg = qsTr("手机号不正确");
            break;
        case 20:
            tipmsg = qsTr("验证码无效");
            break;
        case 21:
            tipmsg = qsTr("验证码过期");
            break;
        case 30:
            tipmsg = qsTr("表示帐号不存在");
            break;
        case 40:
            tipmsg = qsTr("登录失败");
            break;
        default:
            tipmsg = qsTr("登录失败");
            break;
        }
        tips.text = tipmsg;
    }
}

// 获取图片下载路径等配置信息
function getConfig(){
    var url = API.api_root+API.api_config;
    var verb = "GET";
    API.httpRequest(verb, url, "", getConfigCB);

}

// 获取配置信息回调
function getConfigCB(data){
//    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
//        console.log("获取配置信息回调成功");
        // 获取图片下载路径等配置信息
        API.api_upload_img = data.basic_data.upload_img;
        API.api_upload_file = data.basic_data.upload_file;

        // 获取最新客户端版本
        API.api_windows_download = data.basic_data.windows_download;
        API.windows_newversion = data.basic_data.windows_version;
        API.windows_newdescribe = data.basic_data.windows_descrption;

        // 如果有最新版本，提醒客户更新
        utilityControl.checkUpdate(API.windows_newversion, API.api_windows_download, API.windows_newdescribe)

    }else{
        console.log("获取配置信息回调成功失败");
        smsTipDialog.msg = qsTr("获取配置信息回调成功失败!")
        smsTipDialog.flag = 1;
        smsTipDialog.okTitle = qsTr("确定")
        smsTipDialog.show();
        tips.text = qsTr("获取配置信息回调成功失败");
        return ;
    }
}
