
// 修改密码
function sndVCode(){
    var phone = "13550324665";
//    var phone = oldPWD.text;
    if(oldPwd=="" || newPwd=="" || confirmPwd==""){
        tooltip.msg = "请输入旧密码和新密码";
        tooltip.visible=true;
        return;
    }

    // 提交接口修改
    var url = API.api_root+API.api_sndVcode;
    var obj = "user_mobile="+phone;
    var verb = "POST";
    console.log("url:"+url);
    API.httpRequest(verb, url, obj, changePwdCallback);
}

// 修改密码回调
function changePwdCallback(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode =="-1"){
       console.log("获取验证码成功");
    }else{
        console.log("获取验证码失败");
    }
}
