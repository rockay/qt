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
//var token = "6EBB10896EBB5C1F4D754D755C1F4D7526CC6EBB";
//var rong_token = "BFJgjNcFWmdVMB0W0q9jmz6LgOT5GtjMBORyJ/SMMB/Ai4SgesOUyx7VFQEDhV3ISmHPNBHothR7uHSWxYPLGf6/sJrJ7b4J"
//var weix_token = ""
//var user_id = "1012332"
//var user_mobile = "13550324665"
//var user_name = "刘涛"
//var user_wx_name = "刘裕"
//var user_photo = "http://wx.qlogo.cn/mmopen/Q3auHgzwzM7LGvbFZdOCibjYOXnwcQiccZ7KXX7fxPXYZsEUN5AVYsEvY2yMl7LcaG5Vx8ibBXLHjfxGqQdFATkBg/0"
//var is_vip_user = "0"
//var is_company_user = "0"

var windows_newversion = ""
var windows_newdescribe = ""


// 配置相关
var api_config = "api_v2/config/bootup"                 // 获取配置信息
var api_upload_img = "" //"http://f2.2045kpt.com/upload/upload_img" // 图片上传路径
var api_upload_file = "" //"http://f2.2045kpt.com/upload/upload_file" // 文件上传路径
var api_windows_download = ""

// 登录相关
var api_sndVcode = "api_v2/account/rndcode_send";       // 获取验证码
var api_login = "api_v3/account/login_rndcode";         // 验证码登录
var api_voiceCode = "api_v3/account/voicecode_send"      // 获取语音验证码

// 好友相关
var api_friendlist = "api_v2/friends/friend_list"       //获取通讯录好友
var api_companyflist = "/api_v3/friends/client_friend"  //获取企业好友
var api_frienddetail = "/api_v3/friends/search_user"   // 获取好友详情


// 群组相关
var api_grouplist = "api_v2/group/get_group"            // 获取群组列表
var api_groupfriend = "api_v3/friends/group_friend"     // 获取群内好友列表
var api_groupmember = "api_v2/group/group_detail"       // 获取群内成员和详情

// 云库相关
var api_getcloudfile = "api_v2/chat/get_file"           // 获取云库文件
var api_savefile = "/api_v3/cloud/save"                 // 保存上传云库文件
var api_removefile = "/api_v3/cloud/file_remove"        // 移除云库文件



var photoObjMap={};

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

// 比较日期
function compareDate(time1,time2){
    var oldtime=Date.parse(time1);
    var newtime=Date.parse(time2);
    var subTime = (newtime - oldtime)/1000/60;
    return subTime;
}

// 获取坐标
function getAbsolutePosition(node) {
    var returnPos = {};
    returnPos.x = 0;
    returnPos.y = 0;
    if(node !== undefined && node !== null) {
        var parentValue = getAbsolutePosition(node.parent);
        returnPos.x = parentValue.x + node.x;
        returnPos.y = parentValue.y + node.y;
    }
    return returnPos;
}

// 验证图片和PDF格式
function validateFileExtension(filePath) {
    var ext = filePath.split('.').pop().toUpperCase();
    var valid = false;

    if(ext == "JPG" || ext == "BMP" || ext == "GIF"
            || ext == "JPEG" || ext == "ICO" || ext == "PNG" || ext == "PDF") {
        valid = true;
    }

    return valid;
}


