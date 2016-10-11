var allFiles;

// 获取云库文件
function getClouds(){
    var url = API.api_root+API.api_getcloudfile;
    // token：当前用户登录
    // token key_words：搜索关键字（可选）
    // file_mold：文件类型，0 表示获取所有，1 表示只获取图片 get_mold：获取类型，1 表示首次获取或下拉刷新，2 表示上拉获取更多
    // last_node：获取节点，get_mold 为 1 时为 0，
    // get_mold 为 2 时为已获取到的最 后一条 cloud_id
    var obj = "token="+API.token+"&file_mold=0&get_mold=1&last_node=0";
    var verb = "POST";
    API.httpRequest(verb, url, obj, getCloudsCB);

}

// 获取云库文件回调
function getCloudsCB(data){
//    console.log("data:"+JSON.stringify(data));
    if(data.errorcode =="-1"){
        console.log("获取云库文件成功");
        allFiles = data.cloud_list;
        allmodel.append(data.cloud_list);
    }else{
        console.log("获取云库文件失败");
    }
}

function filter(mode,name){
    allmodel.clear();
    var tempdata;
    if(mode==0 || mode==4)
    {
        tempdata = allFiles.filter(function(item){
            return item.file_name.indexOf(name)>=0;
        });
    }
    else{
        tempdata = allFiles.filter(function(item){
            return (item.file_mold==mode && item.file_name.indexOf(name)>=0);
        });
    }
    allmodel.append(tempdata);
}
