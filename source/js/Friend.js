var allContacts ;   // 所有通讯录好友
var allCompany ;    // 所有企业好友

// 获取通讯录好友
function getContacts(){
    var url = API.api_root+API.api_friendlist;
    var obj = "token="+settings.token;
    var verb = "POST";
    API.httpRequest(verb, url, obj, getContactsCB);

}

// 获取通讯录回调
function getContactsCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        allmodel.clear();
        allContacts = {};
        console.log("获取通讯录好友成功");
        allContacts = data.friend;
        allmodel.append(data.friend);
    }else{
        console.log("获取通讯录好友失败");
    }
}

// 获取企业好友
function getCompany(){
    var url = API.api_root+API.api_companyflist;
    var obj = "token="+settings.token; // 企业ID先写死，可能要接口
    var verb = "POST";
    API.httpRequest(verb, url, obj, getCompanyCB);

}

// 获取企业好友回调
function getCompanyCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        busimodel.clear();
        allCompany={};
        console.log("获取企业好友成功");
        var companylist = data.company_list;
        var allobj = [];

        console.log("companylist");
        console.log(JSON.stringify(companylist));

        for(var i=0; i<companylist.length; i++){
            var obj = companylist[i].friend_list;
            console.log("friend_list");
            console.log(JSON.stringify(obj));
            for(var k=0;k<obj.length;k++){
                obj[k].visibled = false ;
                obj[k].company_id = companylist[i].company_id;
                obj[k].company_name = companylist[i].company_name;
                allobj.push(obj[k]);
            }
        }
        allCompany = allobj;
        busimodel.append(allobj);
        console.log("allobj");
        console.log(JSON.stringify(allobj));
        // 将头像放到头像列表
//        for(var i=0; i< data.friend_list.length;i++){
//            var id = data.friend_list[i].friend_id;
//            API.photoObjMap[id] = data.friend_list[i].friend_photo;
//        }
    }else{
        console.log("获取企业好友失败");
    }
}


function search(mode, name) {
    var tempdata;
    if(mode==0)
    {
        allmodel.clear();
        tempdata = allContacts.filter(function(item){
            return (item.friend_name.indexOf(name)>=0 || item.friend_remark_name.indexOf(name)>=0);
        });
        allmodel.append(tempdata);
    }
    else{
        busimodel.clear();
        tempdata = allCompany.filter(function(item){
            return  (item.friend_name.indexOf(name)>=0 || item.friend_remark_name.indexOf(name)>=0);
        });
        busimodel.append(tempdata);
    }
}
