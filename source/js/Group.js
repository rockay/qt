var allGroups;

// 获取群组
function getGroups(){
    var url = API.api_root+API.api_grouplist;
    var obj = "token="+settings.token+"&last_node=0";
    var verb = "POST";
    API.httpRequest(verb, url, obj, getGroupsCB);

}

// 获取群组回调
function getGroupsCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode == -1){
        allmodel.clear();
        allGroups={};
        console.log("获取群组成功");
        allGroups = data.group_list;
        allmodel.append(data.group_list);
        // 将头像放到头像列表
//        for(var i=0; i< data.group_list.length;i++){
//            var id = data.group_list[i].group_id;
//            API.photoObjMap[id] = data.group_list[i].group_cover;
//        }
    }else{
        console.log("获取群组失败");
    }
}

function search(name) {
    var tempdata;

    allmodel.clear();
    tempdata = allGroups.filter(function(item){
        return item.group_name.indexOf(name)>=0;
    });
    allmodel.append(tempdata);

}

