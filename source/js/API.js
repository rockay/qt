.pragma library

var api_root = "http://www.2045kpt.com/";

// token 测试用
var token = "NvoV8I+kQY9LdrDLgKSuWkrx+SVR6RWcfp10gBCm8NehKdbrOh5dNZ4BdFjQ77o6rYN9uOdwEIqwDvfXqpJiXQ==";

//登录
var api_login = "/CompanyUser/AppLogin";

// 获取验证码
var api_sndVcode = "api_v2/account/rndcode_send";




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
            print('HEADERS_RECEIVED')
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
