var allMessage;

//当前日期时间
function currentDateTime(){
    return Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz ddd");
}

// 发送信息
function sendMsg(sendtxt,user_type,ctype){
    tips.text = "";
    var idx = contactListView.currentIndex;
    if(idx==-1)
        return;
        var targetid = chatview.user_id;
        var recipient = topTitle.text;
        var categoryId = chatview.user_type;

        if(ctype==5) // 图片
            sendtxt = sendtxt.replace("file:///","");

        // 如果是群消息，检查是否有@
        var mentionList = [];
        if(user_type==3){
            console.log("发送群消息")
            if(sendtxt.indexOf("@")>=0){
                var itemList = sendtxt.split('@');
                console.log("@list:"+itemList.length);
                for(var i=0; i<itemList.length; i++){
                    var item = itemList[i];
                    console.log("@item:"+item);
                    if(item!=""){
                        // 空格切断
                        var userList = item.split(' ');
                        if(userList.length>=1){
                            // 根据名称查ID
                            var modeList = grpMemberListModel;
                            for(var k=0;k<grpMemberListModel.count;k++){
                                if(grpMemberListModel.get(k).user_name === userList[0])
                                    mentionList.push(grpMemberListModel.get(k).user_id+'');
                            }
                        }
                    }
                }
            }
        }
        console.log("mentionList:"+JSON.stringify(mentionList));

        // 先保存数据库
        var messgeid = utilityControl.getMessageId();
        chatview.chatListModel.addMessage(utilityControl.getGuid(),messgeid,targetid, API.user_id,sendtxt,targetid,0,ctype,""); // 空为发送时间，CPP中获取
        console.log("send message before")
        if(mentionList.length==0 || categoryId !=3 )
            ryControl.sendMsg(messgeid, targetid,categoryId,sendtxt,ctype,"");
        else
            ryControl.sendMsg(messgeid, targetid,categoryId,sendtxt,ctype,JSON.stringify(mentionList));


        // 如果是图片
        if(ctype==5)
            sendtxt = qsTr("[图片]");
        else if(ctype==31)
            sendtxt = qsTr("[云库文件]");
        contactListView.model.addContactById(targetid, sendtxt,0);
        contactListView.currentIndex = 0;


        chattool.document.setText("");
        sendText.clear();
        chatview.reload = false
        chatview.reload = true
//    }
}

function sendNtyMsg(userid,categoryid){

    // 根据ID，获取发送过来的消息最新的ID
    var retStr = chatview.chatListModel.getLastMsgId(chatview.user_id);
    if(retStr != ""){
        console.log("retStr:"+retStr)
        var retList = retStr.split('|');
        if(retList.length==2){
            console.log("retList[0]:"+retList[0])
            console.log("retList[1]:"+retList[1])
            ryControl.sendNtfMsg(retList[0], userid, categoryid,retList[1]);
        }
    }
    // 停止闪烁
    systrayControl.stopFlash();

}

var allGroupUser ;    // 所有群成员
// 获取群成员
function getGroupMember(groupid){
    var url = API.api_root+API.api_groupmember;
    var obj = "token="+API.token+"&group_id="+groupid;
    var verb = "POST";
    API.httpRequest(verb, url, obj, getGroupMemberCB);

}

// 获取群成员回调
function getGroupMemberCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        console.log("获取群成员成功");
        grpMemberListModel.clear();
        grpMemberListModel.append(data.group_info.user_list);
        grpMemberListModelFilter.clear();
        grpMemberListModelFilter.append(data.group_info.user_list);

        allGroupUser={};
        allGroupUser = data.group_info.user_list;

        // 将群组成员头像放到头像列表，因为不是好友，所以没有缓存
        for(var i=0; i< data.group_info.user_list.length;i++){
            var id = data.group_info.user_list[i].user_id;
            API.photoObjMap[id] = data.group_info.user_list[i].user_photo;
        }
        console.log(JSON.stringify(API.photoObjMap));
        grouptitle.text = qsTr("群成员（"+grpMemberListModel.count+"）")
    }else{
        console.log("获取群成员失败");
    }
}

// 文件上传成功后，保存文件。
function saveFileMsg(fileinfo,messageid){
    var url = API.api_root + API.api_savefile;
    var obj = "token="+API.token+"&file_info="+fileinfo;
    var verb = "POST";
    console.log("post url:"+url);
    console.log("post param:"+obj);
    API.httpRequestID(verb, url, obj, saveFileMsgCB,messageid);
}

// 保存云库文件回调
function saveFileMsgCB(data,messageid){
    console.log("保存文件回调:"+JSON.stringify(data));
    if(data.errorcode === -1){
        // 上传成功，更新数据库MSG字段，未完成。
        var sendtxt = data.cloud_info.file_ext+"|"+data.cloud_info.file_mold+"|"
                +data.cloud_info.file_size+"|"+data.cloud_info.file_name+"|"+data.cloud_info.file_url
        chatview.chatListModel.updateMsgContent(messageid, sendtxt);
        var msgid = ryControl.sendCloudMsg(messageid,chatview.user_id,chatview.user_type,sendtxt,31);

    }else{
//        upfilemodel.setProperty(curIdx, "percent", -1);
        tips.text = "发送文件失败";
        console.log("保存云库文件失败");
    }
}

// 获取用户信息
var tempfmsg = "";
function getUserInfoById(user_id, msg){
    var url = API.api_root+API.api_frienddetail;
    var obj = "token="+API.token+"&search_key="+user_id;
    var verb = "POST";
    tempfmsg = msg;
    API.httpRequest(verb, url, obj, getUserInfoByIdCB);
}

// 获取好友信息回调
function getUserInfoByIdCB(data){
    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        console.log("查询好友信息成功");
        var obj = data.search_user;
        if(obj.length>0)
            chatListView.model.addContacts(obj[0].user_id,obj[0].user_name,obj[0].user_remark,obj[0].user_photo,tempfmsg,1,1) // 单人
        setCurrentIdx();
    }else{
        console.log("查询好友信息失败");
    }
}

var grouptempid = "";
var tempgmsg = "";
// 获取群组信息
function getGroupInfoById(groupid, msg){
    var url = API.api_root+API.api_groupmember;
    var obj = "token="+API.token+"&group_id="+groupid;
    var verb = "POST";
    grouptempid = groupid;
    tempgmsg = msg;
    API.httpRequest(verb, url, obj, getGroupInfoByIdCB);
}

// 获取群组信息回调
function getGroupInfoByIdCB(data){
//    console.log("data:"+JSON.stringify(data));
    if(data.errorcode === -1){
        console.log("获取群组信息成功");
        var obj = data.group_info;
        chatListView.model.addContacts(grouptempid,obj.group_name,"",obj.group_cover,tempgmsg,3,1) // 单人
        setCurrentIdx();
    }else{
        console.log("获取群组信息失败");
    }
}

function setCurrentIdx(){
    console.log("curr_id:"+chatviewp.user_id);
    for( var i=0; i<contactListView.model.count; i++){
        console.log("user_id:"+contactListView.model.get(i).user_id);
        if(contactListView.model.get(i).user_id == chatviewp.user_id){
            contactListView.currentIndex = i;
            break;
        }
    }
}

function search(name) {
    if(name == undefined)
        name = "";
    var tempdata;
    grpMemberListModelFilter.clear();
    console.log(JSON.stringify("name:"+name));
    tempdata = allGroupUser.filter(function(item){
        return  (item.user_name.indexOf(name)>=0 && item.user_id != API.user_id);
    });
    grpMemberListModelFilter.append(tempdata);
    if(grpMemberListModelFilter.count>0)
        subtipslistview.currentIndex = 0;
    console.log(JSON.stringify(tempdata));
}

// 选择文件列表后的操作
function selectFiles(fileUrls){
    for (var i = 0; i < fileUrls.length; i++){
        var strPath = fileUrls[i].toString();
        var ext = strPath.split('.').pop().toUpperCase();
        if(ext == "JPG" || ext == "BMP" || ext == "GIF"
                || ext == "JPEG" || ext == "ICO" || ext == "PNG"){// 图片
            chatview.ctype = 5;
            console.log("message send image path:"+strPath)
            sendMsg(strPath,chatview.user_type,chatview.ctype);
        }else if(ext == "PDF"){ // 图片
            console.log("message send PDF path:"+strPath)
            chatview.ctype = 31;
            // 1.上传文件
            sendCloudMsg(strPath);
        }else{ // 其它
            console.log("其它文件 失败")
        }
    }
}

function sendCloudMsg(strPath){
    // 文档先调用上传，再发送云文件
    chatview.ctype = 31;
    // 先把消息入库，后面发送成功再更新既可
    if(contactListView.currentIndex==-1)
        return;

    // 先保存数据库
    var messageid = utilityControl.getMessageId();
    file_messageid = messageid;
    var sendtxt = utilityControl.getFileFullInfo(strPath); // 获取组装的发送格式
    if(!chatview.chatListModel.addMessage(messageid, messageid, chatview.user_id, API.user_id, sendtxt, chatview.user_id,0,31,"")) // 空为发送时间，CPP中获取
        return; // 发送失败


    sendtxt = qsTr("[云库文件]");
    // 更新会话列表
    contactListView.model.addContactById(chatview.user_id, sendtxt,0);


    // 1.上传文件
    var ret = utilityControl.uploadMaterial(API.api_upload_file,strPath,"mFile",0,messageid) // 0为会话上传，1为云上传，消息获取区分用
    if(!ret){
        console.log("upload file failed...");
    }
}
