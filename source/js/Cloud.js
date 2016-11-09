var allFiles;

// 获取云库文件
function getClouds(){
    var url = API.api_root+API.api_getcloudfile;
    // token：当前用户登录
    // token key_words：搜索关键字（可选）
    // file_mold：文件类型，0 表示获取所有，1 表示只获取图片 get_mold：获取类型，1 表示首次获取或下拉刷新，2 表示上拉获取更多
    // last_node：获取节点，get_mold 为 1 时为 0，
    // get_mold 为 2 时为已获取到的最 后一条 cloud_id
    var obj = "token="+settings.token+"&file_mold=0&get_mold=1&last_node=0";
    var verb = "POST";
    console.log("clouds::::::::"+obj);
    allmodel.clear();
    allFiles={};
    API.httpRequest(verb, url, obj, getCloudsCB);

}

// 获取云库文件回调
function getCloudsCB(data){
//    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        console.log("获取云库文件成功");
        allFiles = data.cloud_list;
        allmodel.append(data.cloud_list);
        searchFile(categoryView.currentIndex,search.value);
    }else{
        console.log("获取云库文件失败");
    }
}

function searchFile(mode,name){
    allmodel.clear();
    if(allFiles.length==0)
        return;
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

function uploadFie(){
    if(upfilemodel.count>curIdx)
    {
        // 只有percent为0的才要继续上传
        if(upfilemodel.get(curIdx).percent>0){
            // 已经在上传，下一个
            curIdx++;
            uploadFie();
            return;
        }
        var fileType = upfilemodel.get(curIdx).file_mold === 1 ? "mImage":"mFile";
        var ret = utilityControl.uploadMaterial(API.api_upload_file,upfilemodel.get(curIdx).file_url,fileType,1)
        if(!ret){
            console.log("upload file failed...");
            upfilemodel.setProperty(curIdx, "percent", -1);
            // 失败继续下一个
            curIdx++;
            uploadFie();
        }
    }else{
//        upFinishDialog.open();
//        bottomtips.text = qsTr("文件上传完成");

        if(upFinishDialog.visible)
            upFinishDialog.requestActivate();
        else
            upFinishDialog.show();
    }
}


// 文件上传成功后，保存文件。
function saveFile(fileinfo){
    var url = API.api_root + API.api_savefile;
    var obj = "token="+settings.token+"&file_info="+fileinfo;
    var verb = "POST";
    console.log("post param:"+obj);
    API.httpRequest(verb, url, obj, saveFileCB);
}

// 保存云库文件回调
function saveFileCB(data){
//    console.log("保存云库文件回调:"+JSON.stringify(data));
    if(data.errorcode === -1){
        uploadFileSuccessed();
        upfilemodel.setProperty(curIdx, "percent", 101);
//        console.log("保存云库文件成功");
    }else{
        upfilemodel.setProperty(curIdx, "percent", -1);
        console.log("保存云库文件失败");
    }
    curIdx++
    uploadFie();
}

// 获取云库文件(发送消息时)
function getCloudsByMsg(){
    var url = API.api_root+API.api_getcloudfile;
    var obj = "token="+settings.token+"&file_mold=0&get_mold=1&last_node=0";
    var verb = "POST";
    API.httpRequest(verb, url, obj, getCloudsByMsgCB);

}

// 获取云库文件回调(发送消息时)
function getCloudsByMsgCB(data){
//    console.log("data:"+JSON.stringify(data));
    choosefilemodel.clear();
    if(data.errorcode === -1){
//        console.log("获取云库文件成功(发送消息时)");
        choosefilemodel.append(data.cloud_list);
    }else{
        console.log("获取云库文件失败(发送消息时)");
    }
}


// 选择文件列表后的操作
function selectFiles(fileUrls){
    var showSize = 0;
    for (var i = 0; i < fileUrls.length; i++){
        var strPath = fileUrls[i].toString();
        var ext = strPath.split('.').pop().toUpperCase();
        if(!(ext == "JPG" || ext == "BMP" || ext == "GIF"
                || ext == "JPEG" || ext == "ICO" || ext == "PNG" || ext == "PDF")){
            return ;
        }

        var fileinfo = utilityControl.getFileInfo(strPath).split('|');
        var fsize = fileinfo.length>1 ? Number(fileinfo[0]) : 0;
        var file_nme = fileinfo.length>1 ? fileinfo[1] : "";
        console.log(strPath);
        upfilemodel.append({"percent": 0 , "file_url": strPath, "size": fsize, "file_ext": ext,
                               "file_name":file_nme,
                               "file_mold": ( ext == "JPG" || ext == "BMP" || ext == "GIF"
                                             || ext == "JPEG" || ext == "ICO" || ext=="PNG") ? 1 : 2
                           })
        allSize += fsize;
        showSize = allSize/1024 > 1024 ? (allSize/1024/1024).toFixed(2)+"M" : (allSize/1024).toFixed(2)+"k"
    }
    bottomtips.text = qsTr("共"+upfilemodel.count+"个文件，"+showSize+"（上传过程中请不要删除原始文件）")
    bottomtips.visible = true;
}

// 重新计算文件和大小
function reCalculate(){
    allSize = 0;
    var showSize = 0;
    for (var i = 0; i < upfilemodel.count; i++){
        var fsize = Number(upfilemodel.get(i).size);
        allSize += fsize;
        showSize = allSize/1024 > 1024 ? (allSize/1024/1024).toFixed(2)+"M" : (allSize/1024).toFixed(2)+"k"
    }
    bottomtips.text = qsTr("共"+upfilemodel.count+"个文件，"+showSize+"（上传过程中请不要删除原始文件）")
    bottomtips.visible = true;
}

// 删除云库文件
function deleteFile(fileid){
    console.log("remove file:"+fileid);
    // 提交接口修改
    var url = API.api_root+API.api_removefile;
    var obj = "token="+settings.token+"&cloud_id="+fileid;
    var verb = "POST";
    API.httpRequest(verb, url, obj, deleteFileCB);
}

// 删除云库文件回调
function deleteFileCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        console.log("删除云库文件成功")
        var cloudid = allmodel.get(filegrid.currentIndex).cloud_id;
        console.log(cloudid);
        for(var i=0; i <allFiles.length;i++){
            if(allFiles[i].cloud_id == cloudid)
            {
                allFiles.splice(i,1);
                break;
            }
        }
        allmodel.remove(filegrid.currentIndex);
        console.log("删除云库文件成功2")
;
    }else if(data.errorcode === -1){
        console.log("登录失效");
    }else{
        console.log("删除云库文件失败");
    }
}
