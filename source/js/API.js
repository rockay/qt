.pragma library

var api_root = "http://www.2045kpt.com/";

//{
//    "token": "6EBB10896EBB6EBB1A1C6EBB4F885C1F0A9D6EBB",
//    "rong_token": "68S/N6pLvPu4n2GzNkvNBRjUT4yuNhCsE8yk4WJbGM2+Ok6dNPjwaV1FCieObWa9gbUzA9+lyid6SdGFwvgjd1L53KtrtPF2",
//    "weix_token": "",
//    "user_info": {
//        "user_id": 1011815,
//        "user_mobile": "13987871719",
//        "user_name": "测试账号1",
//        "user_wx_name": "",
//        "user_photo": "http://qf.2045kpt.com/Files/common/User_Head/E43B16646AB1D1352F2F654E64434BAE.png",
//        "is_vip_user": 0,
//        "is_company_user": 0
//    },
//    "errorcode": -1
//}
// token 测试用
var token = "6EBB10896EBB5C1F4D754D755C1F3F4E4D756EBB";
var rong_token = "wF95F73D6ZTsWt1SYAMwQhjUT4yuNhCsE8yk4WJbGM26BuVaS2WNvdRI933kuN3GF3scOwpS24L1CoOfm/CrNlL53KtrtPF2"
var weix_token = ""
var user_id = "1012332"
var user_mobile = "13550324665"
var user_name = "刘涛"
var user_wx_name = "刘裕"
var user_photo = "http://qf.2045kpt.com/Files/common/User_Head/E43B16646AB1D1352F2F654E64434BAE.png"
var is_vip_user = "0"
var is_company_user = "0"

// 登录相关
var api_sndVcode = "api_v2/account/rndcode_send";       // 获取验证码
var api_login = "api_v3/account/login_rndcode";         // 验证码登录

// 好友相关
var api_friendlist = "api_v2/friends/friend_list"       //获取通讯录好友
var api_companyflist = "api_v2/friends/get_company_friends"    //获取企业好友


// 群组相关
var api_grouplist = "api_v2/group/get_group"            // 获取群组列表

// 云库相关
var api_getcloudfile = "api_v2/chat/get_file"           // 获取云库文件



// 公共网络请求函数
// 参数说明：
// verb 请求类型，支持"GET","POST","PUT","DELETE"
// url 请求的webapi地址
// obj 为数据json对象，POST、PUT和DELETE等的时候用，要看web api_myshare
// cb 为callback函数，就是这个请求完成后，会调用你传进来的回调JS函数。
function httpRequest(verb, url, obj, cb) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState === XMLHttpRequest.HEADERS_RECEIVED) {
//            print('HEADERS_RECEIVED')
        } else if(xhr.readyState === XMLHttpRequest.DONE) {
            try{
                var retjson = JSON.parse(xhr.responseText.toString());

                if(cb) // 调用传进来的callback函数
                    cb(retjson);
            }
            catch(e){
                console.log(xhr.responseText.toString());
                console.log("httpRequest 发生异常："+e.message);
                if(cb) // 调用传进来的callback函数
                    cb("");
            }
        }
    }
    xhr.open(verb,url);
//    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    xhr.setRequestHeader('Accept', 'application/json');
//    var data = obj?JSON.stringify(obj):''
    xhr.send(obj);
}
