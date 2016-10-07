var allData = null

// 获取通讯录好友
function getContacts(){
    var url = API.api_root+API.api_friendlist;
    var obj = "token="+API.token;
    var verb = "POST";
    API.httpRequest(verb, url, obj, getContactsCB);

}

// 获取通讯录回调
function getContactsCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode =="-1"){
        console.log("获取通讯录好友成功");
        allmodel.append(data.friend);
    }else{
        console.log("获取通讯录好友失败");
    }
}

// 获取企业好友
function getCompany(){
    var url = API.api_root+API.api_companyflist;
    var obj = "token="+API.token+"&company_id=1021"; // 企业ID先写死，可能要接口
    var verb = "POST";
    API.httpRequest(verb, url, obj, getCompanyCB);

}

// 获取企业好友回调
function getCompanyCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode =="-1"){
        console.log("获取企业好友成功");
        busimodel.append(data.friend);
    }else{
        console.log("获取企业好友失败");
    }
}


function search(txt) {
    console.log("count:"+allmodel.count);
}
