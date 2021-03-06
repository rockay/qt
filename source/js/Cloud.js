var allFiles;

// 获取云库文件
function getClouds(){
    var url = API.api_root+API.api_getcloudfile_new;
    // token：当前用户登录
    // token key_words：搜索关键字（可选）
    // file_mold：文件类型，0 表示获取所有，1 表示只获取图片 get_mold：获取类型，1 表示首次获取或下拉刷新，2 表示上拉获取更多
    // last_node：获取节点，get_mold 为 1 时为 0，
    // get_mold 为 2 时为已获取到的最 后一条 cloud_id
    var obj = "token="+API.token+"&file_mold=0&get_mold=1&last_node=0";
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
//        allmodel.append(data.cloud_list);
        searchFile(categoryView.currentIndex,search.value);
    }else{
        console.log("获取云库文件失败");
    }
}

function searchFile(mode,name){
    if(mode>=2)
        mode += 1;
    allmodel.clear();
    if(allFiles.length==0)
        return;
    var tempdata;
    if(mode==0)
    {
        tempdata = allFiles.filter(function(item){
            return item.file_name.indexOf(name)>=0;
        });
    }
    else if(mode==4)
    {
        tempdata = allFiles.filter(function(item){
            return ((item.file_mold==2 || item.file_mold==4) && item.file_name.indexOf(name)>=0);
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
        var ret = utilityControl.uploadMaterial(API.api_upload_file,upfilemodel.get(curIdx).file_url,fileType,1,curIdx) // messageid就传curIdx
        if(!ret){
            console.log("upload file failed...");
            upfilemodel.setProperty(curIdx, "percent", -1);
            // 失败继续下一个
            curIdx++;
            uploadFie();
        }
    }else{
        console.log("文件上传完成...");
        for(var i=0; i<upfilemodel.count; i++){
            upfilemodel.setProperty(i, "percent", 101);
        }

        upFinishDialog.msg = "文件上传完成";
        if(upFinishDialog.visible)
            upFinishDialog.requestActivate();
        else
            upFinishDialog.show();
    }
}


// 文件上传成功后，保存文件。
function saveFile(fileinfo){
    var url = API.api_root + API.api_savefile;
    var obj = "token="+API.token+"&file_info="+fileinfo;
    var verb = "POST";
    console.log("post param:"+obj);
    API.httpRequest(verb, url, obj, saveFileCB);
}

// 保存云库文件回调
function saveFileCB(data){
//    console.log("保存云库文件回调:"+JSON.stringify(data));
    if(data.errorcode === -1){
        upfilemodel.setProperty(curIdx, "percent", 100);
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
    var obj = "token="+API.token+"&file_mold=0&get_mold=1&last_node=0";
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
    if(fileUrls.length>20){
        upFinishDialog.msg = "请一次发送不超过20个文件";
        if(upFinishDialog.visible)
            upFinishDialog.requestActivate();
        else
            upFinishDialog.show();
        return;
    }
    var showSize = 0;
    for (var i = 0; i < fileUrls.length; i++){
        var strPath = fileUrls[i].toString();
        var ext = strPath.split('.').pop().toLowerCase();
        if(!(ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "pdf"
             ||ext == "ppt" || ext == "pptx" || ext == "xls" || ext == "xlsx"
             ||ext == "doc" || ext == "docx" || ext == "txt" || ext == "rar"
             ||ext == "zip" || ext == "7z" || ext == "dwg" || ext == "skp")){
            return ;
        }

        var fileinfo = utilityControl.getFileInfo(strPath).split('|');
        if(fileinfo.length>1){ // 2016.11.11
            var fsize = fileinfo.length>1 ? Number(fileinfo[0]) : 0;
            var file_nme = fileinfo.length>1 ? fileinfo[1] : "";
            console.log(strPath);
            upfilemodel.append({"percent": 0 , "file_url": strPath, "size": fsize, "file_ext": ext.toUpperCase(),
                                   "file_name":file_nme,
                                   "file_mold": API.getFileMode(strPath)
                               })
            allSize += fsize;
            showSize = allSize/1024 > 1024 ? (allSize/1024/1024).toFixed(2)+"M" : (allSize/1024).toFixed(2)+"k"
        }
    }
    bottomtips.text = qsTr("共"+upfilemodel.count+"个文件，"+showSize)
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
    bottomtips.text = qsTr("共"+upfilemodel.count+"个文件，"+showSize)
    bottomtips.visible = true;
}

// 删除云库文件
function deleteFile(cloudid){
    // 先本地删除文件
    for(var i=0; i <allFiles.length;i++){
        if(allFiles[i].cloud_id == cloudid)
        {
            allFiles.splice(i,1);
            break;
        }
    }
    allmodel.remove(filegrid.currentIndex);

    // 提交接口修改
    var url = API.api_root+API.api_removefile;
    var obj = "token="+API.token+"&cloud_id="+cloudid;
    var verb = "POST";
    API.httpRequest(verb, url, obj, deleteFileCB);
}

// 删除云库文件回调
function deleteFileCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode == -1){
        console.log("删除云库文件成功")
    }else if(data.errorcode == 10){
        console.log("登录失效");
        delErrorDialog.msg = "删除云库文件失败，登录失效";
        delErrorDialog.show();
    }else{
        getClouds(); // 失败重新获取
        console.log("删除云库文件失败");
        delErrorDialog.show();
    }
}
