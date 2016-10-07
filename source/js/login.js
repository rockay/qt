var phone = "13550324665";

// 获取验证码
function sndVCode(){
    if(phone=="" || phone.length!=11){
        return;
    }

    // 提交接口修改
    var url = API.api_root+API.api_sndVcode;
    var obj = "user_mobile="+phone;
    var verb = "POST";
//    API.httpRequest(verb, url, obj, sndVCodeCB);
    main.visible = true;
    login.hide();
}

// 获取验证码回调
function sndVCodeCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode =="-1"){
       console.log("获取验证码成功");
       loginfuc(phone,data.rnd_code);
    }else{
        console.log("获取验证码失败");
    }
}

// 验证码登录
function loginfuc(phone,code){
//    http://www.2045kpt.com/api_v3/account/login_rndcode
//    user_mobile:用户手机号
//    rnd_code:短信验证码
//    user_device:用户登录设备类型,1 表示 IOS,2 表示 Android
    var url = API.api_root+API.api_login;
    var obj = "user_mobile="+phone+"&rnd_code="+code+"&user_device=1";
    var verb = "POST";
    API.httpRequest(verb, url, obj, loginCB);

}

// 验证码登录回调
function loginCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode =="-1"){
        main.visible = true;
        login.hide();
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
        API.token = data.token;
        API.rong_token = data.rong_token;
        API.weix_token = data.weix_token;
        API.user_id = data.user_info.user_id;
        API.user_mobile = data.user_info.user_mobile;
        API.user_name = data.user_info.user_name;
        API.user_wx_name = data.user_info.user_wx_name;
        API.user_photo = data.user_info.user_photo;
        API.is_vip_user = data.user_info.is_vip_user;
        API.is_company_user = data.user_info.is_company_user;

    }else{
        console.log("登录失败");
    }
}
