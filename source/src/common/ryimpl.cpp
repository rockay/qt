#include "ryimpl.h"
#include "rcsdk.h"
#include <QLibrary>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QImage>
#include <QPixmap>
#include <QFile>
#include "Utility.h"

RYImpl* RYImpl::m_instance = NULL;

QJsonObject getJsonObjectFromString(const QString jsonString){
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8().data());
    if( jsonDocument.isNull() ){
        qDebug()<< "===> getJsonObjectFromString error string:"<< jsonString.toUtf8().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}


//异常监听
void __stdcall exception_callback(const wchar_t* json_str)
{
    QString u16 = QString::fromUtf16((const ushort*)json_str);
    qDebug()<<"异常监听:"<<u16.toUtf8();

    QJsonObject obj = getJsonObjectFromString(u16.toUtf8());
    QString data = obj.value("data").toString();
    QString code = obj.value("nstatus").toString();

    emit RYImpl::getInstance()->receivedException(code, data);

    auto connectCallback = [](const wchar_t* json_str)
    {
        QString str1= QString::fromWCharArray(json_str);
        qDebug()<<"------auto reconnectCallback json_str:"<<str1;
        QString u16 = QString::fromUtf16((const ushort*)json_str);
        QJsonObject obj = getJsonObjectFromString(u16.toUtf8());
        int err_code = obj.value("err_code").toInt();
        QString result = obj.value("result").toString();
        emit RYImpl::getInstance()->receivedException(QString::number(err_code),result);
    };
    RYImpl::getInstance()->Connect(RYImpl::getInstance()->m_token.toUtf8().data(), connectCallback,false);

}

//消息监听
void __stdcall message_callback(const wchar_t* json_str)
{
    QString u16 = QString::fromUtf16((const ushort*)json_str);

    QString msg = u16.toUtf8();

    QJsonObject obj = getJsonObjectFromString(msg);
    QJsonObject objContent = getJsonObjectFromString(obj.value("m_Message").toString());

    qDebug()<<"msg:"<<msg;
    // 这里判断是对方发送消息，还是对方在输入内容。
    QString content ="";
    MSGTYPE type = MSGTYPE::OTHER;
    if(objContent.contains("content") || objContent.contains("file_url"))
    {
        emit RYImpl::getInstance()->recccvMsg("");
        // 对方发送的消息
        type = MSGTYPE::MESSAGE;
        // 判断是文字还是图片，还是其它
        QString className = obj.value("m_ClazzName").toString();
        content = objContent.value("content").toString();
        if(className.compare("RC:TxtMsg")==0) // 文字
        {
            type = MSGTYPE::MSG_TXT;
            qDebug()<<"文字消息:"<<content;
        }
        else if(className.compare("RC:ImgMsg")==0) // 图片
        {
            type = MSGTYPE::MSG_IMG;
            QString uri = objContent.value("imageUri").toString();
            qDebug()<<"图片消息Uri:"<<uri;
            content = RYImpl::getInstance()->saveImage(content); // 返回路径保存
            // 保存缩略图路径和远程路径
            content = content+"|"+uri;
            qDebug()<<"图片缩略图路径:"<<content;
        }
        else if(className.compare("RC:VcMsg")==0) // 语音
        {
            type = MSGTYPE::MSG_VC;
            qDebug()<<"语音消息:"<<content;
            int duration = objContent.value("duration").toInt(); // 语音长度
            content = RYImpl::getInstance()->saveVoice(content); // 返回路径保存
            content = QString::number(duration)+"|"+content; // 时长｜内容
            qDebug()<<"语音存放路径:"<<content;
        }
        else if(className.compare("RC:ImgTextMsg")==0) // 文字+图片
        {
            type = MSGTYPE::MSG_IMGTXT;
            qDebug()<<"文字+图片消息:"<<content;
        }
        else if(className.compare("RC:LBSMsg")==0) // 位置
        {
            type = MSGTYPE::MSG_LBS;
            qDebug()<<"位置消息:"<<content;
        }
        else if(className.compare("app:IMFileMessage")==0) // 云库图片
        {
            type = MSGTYPE::MSG_CLOUDIMG;
            qDebug()<<"云库图片:"<<objContent;
            qDebug()<<"file_name:"<<objContent.value("file_name").toString();
            QString file_ext = objContent.value("file_ext").toString();
            int file_mold= objContent.value("file_mold").toInt();
            int file_size = objContent.value("file_size").toInt();
            QString file_name  = objContent.value("file_name").toString();
            QString file_url = objContent.value("file_url").toString();
            // 保存缩略图路径和远程路径
            content = QString("%1|%2|%3|%4|%5").arg(file_ext, QString::number(file_mold), QString::number(file_size), file_name, file_url);
            qDebug()<<"云库图片:"<<content;
        }
    }
    else if (objContent.contains("typingContentType")){
        // 对方正在录入消息
        type = MSGTYPE::TYPING;
        content = "对方正在输入..."; //objContent.value("typingContentType").toString();
    }else if(obj.value("m_ClazzName").toString().toLower().compare("rc:readntf")==0
             && objContent.contains("lastMessageSendTime")){
        type = MSGTYPE::SENDTIME;
        content = obj.value("m_Message").toString();
        qDebug()<<"content..."<<content;
        // 消息已读
    }else{
        content = obj.value("m_Message").toString();
        // 不知道的格式，直接返回
        return;
    }
    QString msgUId = obj.value("m_MsgUId").toString();
    QString sender = obj.value("m_SenderId").toString();
    QString sendtime = obj.value("m_SendTime").toString();
    QString targetid = obj.value("m_TargetId").toString();
    int messageid = obj.value("m_MessageId").toInt();
    int conversationType = obj.value("m_ConversationType").toInt();
    QString rcvTime = obj.value("m_RcvTime").toString();
    qDebug()<<"m_MessageId"<<messageid;

    QJsonObject objMention = objContent.value("mentionedInfo").toObject();
    qDebug()<<"mentioned..."<<objMention;
    // 如果mentionedInfo，则有@
    bool isMentionedMe = false;
    if(!objMention.isEmpty()){
        int mtype = objMention.value("type").toInt();
        QJsonArray idList = objMention.value("userIdList").toArray();
        qDebug()<<"has mentioned...type"<<mtype;
        foreach (const QJsonValue & value, idList) {
            QString id = value.toString();
            //qDebug()<<"id:"<<id;
            if(id == RYImpl::getInstance()->m_userid){
                isMentionedMe = true;
                break;
            }
        }
    }

    // 如果是回执消息，把lastMessageSendTime当成messgeid
    if(type == MSGTYPE::SENDTIME){
        sendtime = rcvTime; // 把sendtime当接收时间来传递。
        sender = RYImpl::getInstance()->m_userid; // 发送者为本人
    }
    emit RYImpl::getInstance()->receivedMsg(type,sender,msgUId,QString::number(messageid),content,sendtime,conversationType,targetid, isMentionedMe);
}

void RYImpl::initLib(const QString &token)
{
    m_token = token;
    QSettings settings;
    QString user_id = settings.value("user_id").toString();
    m_userid = user_id;

    QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    writeDir.setPath(writeDir.path()+"/"+m_userid);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable root directory at %s", qPrintable(writeDir.absolutePath()));
    m_rootPath =  writeDir.absolutePath();
    m_picPath = m_rootPath +"/images/";
    writeDir.setPath(m_picPath);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable picture directory at %s", qPrintable(writeDir.absolutePath()));
    m_voicePath = m_rootPath +"/voice/";
    writeDir.setPath(m_voicePath);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable voice directory at %s", qPrintable(writeDir.absolutePath()));

    convertype[0]= 1;
    convertype[1]= 2;
    convertype[2]= 3;

    // 如果加载过就不再加载函数
    if(m_isLoaded)
        return;

    QString dir = QCoreApplication::applicationDirPath();
    QString path = tr("%1/rcsdk.dll").arg(dir);
    QFile file(path);
    if(file.exists())
        qDebug()<<"file exists..";
    QLibrary mylib(path);//声明dll文件路径
    if(mylib.load())//加载dll，并判断是否调用成功
    {
        pInitClient = (DLLFunc)mylib.resolve("InitClient");//援引接口函数
        if(pInitClient == NULL )//判断是否连接上接口函数
        {
            qDebug()<<"load InitClient failed..";
            return;
        }
        const wchar_t * dir1 = reinterpret_cast<const wchar_t *>(dir.utf16());
        int ret = pInitClient("x4vkb1qpva0xk","quantu","deviceId", dir1, dir1);

        Connect = (DLLFuncb)mylib.resolve("Connect");
        if (Connect == NULL)
        {
            qDebug()<<"load Connect failed..";
            return;
        }

        Disconnect = (DLLFunDis)mylib.resolve("Disconnect");
        if (Disconnect == NULL)
        {
            qDebug()<<"load Disconnect failed..";
            return;
        }

        //注册消息类型
        typedef int(*DLLFuncd)(const char* clazzName, const unsigned int operateBits);
        DLLFuncd RegisterMessageType;
        RegisterMessageType = (DLLFuncd)mylib.resolve("RegisterMessageType");
        if (RegisterMessageType == NULL)
        {
            qDebug()<<("load RegisterMessageType failed");
            return;
        }
        else
        {
            //文本消息
            RegisterMessageType("RC:TxtMsg", 3);
            //图片消息
            RegisterMessageType("RC:ImgMsg", 3);
            //图文消息
            RegisterMessageType("RC:VcMsg", 3);
            //位置消息
            RegisterMessageType("RC:LBSMsg", 3);
            //添加联系人消息
            RegisterMessageType("RC:ContactNtf", 3);
            //提示条（小灰条）通知消息
            RegisterMessageType("RC:InfoNtf", 3);
            //资料通知消息
            RegisterMessageType("RC:ProfileNtf", 3);
            //通用命令通知消息
            RegisterMessageType("RC:CmdNtf", 3);
        }

        //设置消息监听
        typedef int(*DLLFunce)(MessageListenerCallback callback);
        DLLFunce SetMessageListener;
        SetMessageListener = (DLLFunce)mylib.resolve("SetMessageListener");
        if (SetMessageListener == NULL)
        {
            qDebug()<<"load 设置消息监听 failed..";
            return;
        }
        else
        {
            SetMessageListener(message_callback);
        }


        //设置网络异常监听
        typedef int(*DLLFuncf)(ExceptionListenerCallback callback);
        DLLFuncf SetExceptionListener;
        SetExceptionListener = (DLLFuncf)mylib.resolve("SetExceptionListener");
        if (SetExceptionListener == NULL)
        {
            qDebug()<<"load 设置网络异常监听 failed..";
            return;
        }
        else
        {
            SetExceptionListener(exception_callback);
        }


        // 保存消息
        SaveMessage = (DLLFuncbb)mylib.resolve("SaveMessage");
        if (SaveMessage == NULL)
        {
            qDebug()<<"load SaveMessage failed..";
            return;
        }

        // 发送消息
        sendMessage = (DLLFunbc)mylib.resolve("sendMessage");
        if (sendMessage == NULL)
        {
            qDebug()<<"load sendMessage failed..";
            return;
        }

        // 获取历史会话
        GetConversationList = (DFGetConversationList)mylib.resolve("GetConversationList");
        if (GetConversationList == NULL)
        {
            qDebug()<<"load GetConversationList failed..";
            return;
        }

        // 上传文件
        UpLoadFile = (DLLFunUP)mylib.resolve("UpLoadFile");
        if (UpLoadFile == NULL)
        {
            qDebug()<<"load UpLoadFile failed..";
            return;
        }

        // 转换WAV文件
        DecodeAmrToWav = (DFDecodeAmrToWav)mylib.resolve("DecodeAmrToWav");
        if (DecodeAmrToWav == NULL)
        {
            qDebug()<<"load DecodeAmrToWav failed..";
            return;
        }

    }
    else
        qDebug()<<"load dll fail.."<<mylib.errorString();
    m_isLoaded = true;

}

void RYImpl::connect()
{
    qDebug()<<"connect:"<<m_isConnected;

    // 已经连接要断开连接
    if(Disconnect != NULL){
        Disconnect(4);
    }

    if(Connect != NULL){\
        auto connectCallback = [](const wchar_t* json_str)
        {
            QString str1= QString::fromWCharArray(json_str);
            qDebug()<<"connectCallback json_str:"<<str1;
        };
        qDebug()<<"connect1:"<<m_isConnected;
        Connect(m_token.toUtf8().data(), connectCallback,false);
        m_isConnected = true;
    }
}

void RYImpl::disconnect()
{
    if(m_isConnected &&  Disconnect != NULL){
       qDebug()<<("disconnect");
        Disconnect(4);
    }
}

int RYImpl::sendMsg(int messageId, const QString &targetId,int categoryId, const QString &msg, int type,const QString& mention)
{
    m_categoryId = categoryId;
    m_targetid = targetId;

    QString fmsg= tr("{\"content\":\"%1\"}").arg(msg.toUtf8().data());
    if(categoryId == 3 && !mention.isEmpty())
        fmsg = tr("{\"content\":\"%1\",\"mentionedInfo\":{\"type\":2,\"userIdList\":%2}}").arg(msg.toUtf8().data(),mention);
    const wchar_t * msgw = reinterpret_cast<const wchar_t *>(fmsg.utf16());
    qDebug()<<tr("targetid:%1 \t categoryid:%2 \t content:%3").arg(targetId,QString::number(categoryId),msg);
    QString u16 = QString::fromUtf16((const ushort*)msgw);
    qDebug()<<"msgw src:"<<u16.toUtf8()<<endl;
    auto sendMessageCallback = [](const wchar_t* json_str)
    {
        QString str1= QString::fromWCharArray(json_str);
        qDebug()<< "发送回执:"+str1;
        QString u16 = QString::fromUtf16((const ushort*)json_str);
        QString retMsg = u16.toUtf8();
        QJsonObject obj = getJsonObjectFromString(retMsg);
        int msgUId = obj.value("messageId").toInt();
        int result = obj.value("result").toString() == "success" ? 1 : -1;
        emit RYImpl::getInstance()->sendMsgDealCallback(msgUId, result);
    };
    m_imagePath = "";
    if(type==MSGTYPE::MSG_IMG) // 只有发送图片才有上传
    {
        m_imagePath = msg;
        auto sendImageCallback = [](const wchar_t* json_str)
        {
            QString u16 = QString::fromUtf16((const ushort*)json_str);
            qDebug()<<"图片监听进来:"<<u16.toUtf8();
            QString msg = u16.toUtf8();
            QJsonObject obj = getJsonObjectFromString(msg);
            QString img_id = obj.value("img_id").toString();
             RYImpl::getInstance()->SendImage(u16,img_id.toInt());
        };
        auto processImageCallback = [](const wchar_t* json_str)
        {
            QString u16 = QString::fromUtf16((const ushort*)json_str);
            qDebug()<<"图片处理进来:"<<u16.toUtf8();
            QString msg = u16.toUtf8();
            QJsonObject obj = getJsonObjectFromString(msg);
            QString img_id = obj.value("img_id").toString();
            int process = obj.value("process").toInt();
            int targetId = obj.value("targetId").toInt();
            emit RYImpl::getInstance()->proceeFile(img_id,process,targetId);
        };

        QFile file(msg);
        QByteArray blob;
        // 先上传文件
        if (!file.open(QIODevice::ReadOnly)){
            qDebug()<< "UpLoadFile open file failed path:"<<"file:///"+msg;
            return messageId;
        }else{
            qDebug()<< "UpLoadFile open file path:"<<"file:///"+msg;
        }
        blob = file.readAll();
        qDebug()<<blob.toHex();
        qDebug()<<blob.size();
        uchar *picData = (uchar *)(blob.data());
        UpLoadFile(targetId.toUtf8().data(),categoryId,1,picData,blob.size(),QString::number(messageId).toUtf8().data(),sendImageCallback,processImageCallback);
        qDebug()<<"UpLoadFile ok";
    }
    qDebug()<<"sendMessage categoryId:"<<categoryId;
    if(sendMessage!=NULL){
        switch(type){
        case MSGTYPE::MSG_TXT:
            sendMessage(targetId.toUtf8().data(), categoryId, 3, "RC:TxtMsg", msgw, "", "", messageId, sendMessageCallback);
            break;
        case MSGTYPE::MSG_VC:
            sendMessage(targetId.toUtf8().data(), categoryId, 3, "RC:VcMsg", msgw, "", "", messageId, sendMessageCallback);
            break;
        case MSGTYPE::MSG_IMGTXT:
            sendMessage(targetId.toUtf8().data(), categoryId, 3, "RC:ImgTextMsg", msgw, "", "", messageId, sendMessageCallback);
            break;
        }
    }
    return messageId;
}

int RYImpl::sendCloudMsg(int messageId,const QString &targetId,int categoryId, const QString &msg, int type)
{
    qDebug()<<"发送云文件:"<<msg;
    m_categoryId = categoryId;
    m_targetid = targetId;
    QStringList strList = msg.split("|");
    if(strList.count()!=5)
        qDebug()<<"云文件发送格式不对:"<<msg;
    QString fmsg= tr("{\"file_url\":\"%1\",\"file_name\":\"%2\",\"file_ext\":\"%3\",\"file_size\":%4,\"file_mold\":%5}")
            .arg(strList.at(4), strList.at(3), strList.at(0), strList.at(2), strList.at(1));
    const wchar_t * msgw = reinterpret_cast<const wchar_t *>(fmsg.utf16());
    qDebug()<<tr("targetid:%1 \t categoryid:%2 \t content:%3").arg(targetId,QString(categoryId),fmsg);

    auto sendMessageCallback = [](const wchar_t* json_str)
    {
        QString u16 = QString::fromUtf16((const ushort*)json_str);
        QString retMsg = u16.toUtf8();
        QJsonObject obj = getJsonObjectFromString(retMsg);
        int msgUId = obj.value("messageId").toInt();
        int result = obj.value("result").toString() == "success" ? 1 : -1;
        emit RYImpl::getInstance()->sendMsgDealCallback(msgUId, result);
    };
    if(sendMessage!=NULL){
        sendMessage(targetId.toUtf8().data(), categoryId, 3, "app:IMFileMessage", msgw, "", "", messageId, sendMessageCallback);
    }
    return messageId;
}

void RYImpl::sendNtfMsg(const QString& msguid, const QString &targetId, int categoryId, const QString &msg)
{
    QString fmsg= tr("{\"lastMessageSendTime\":%1,\"type\":1,\"messageUId\":\"%2\"}").arg(msg.toUtf8().data(), msguid);
    const wchar_t * msgw = reinterpret_cast<const wchar_t *>(fmsg.utf16());
    qDebug()<<tr("targetid:%1 \t categoryid:%2 \t content:%3").arg(targetId,QString::number(categoryId),fmsg);

    auto sendMessageCallback = [](const wchar_t* json_str)
    {
        QString u16 = QString::fromUtf16((const ushort*)json_str);
        qDebug()<<"sendNtfMsg back:"<<u16;
    };
    if(sendMessage!=NULL){
        sendMessage(targetId.toUtf8().data(), categoryId, 1, "RC:ReadNtf", msgw, "", "", 0, sendMessageCallback);
    }
}


int RYImpl::GetChatList(){
    qDebug()<<"GetChatList";
//    auto getListCallback = [](const wchar_t* json_str)
//    {
//        QString str1= QString::fromWCharArray(json_str);
//        qDebug()<<"getListCallback json_str:"<<str1;
//    };
//    if(GetConversationList != NULL)
//        GetConversationList(convertype,3,getListCallback);
    return 0;
}

QString RYImpl::saveImage(const QString &base64)
{
    const QString fileName = m_picPath + Utility::getInstance()->getGuid()+".jpg";
    qDebug()<<"picture fileName:"<<fileName;
    QByteArray by = QByteArray::fromBase64(base64.toUtf8());
    QImage image = QImage::fromData(by, "JPG");
    if(image.isNull()){
        qDebug()<<" load JPG image failed...";
        image = QImage::fromData(by, "PNG");
    }

    bool ret = image.save(fileName,"JPG");
    if(!ret){
        qDebug()<<" save iamge failed...";

        ret = image.save(fileName,"PNG");
        // 深度PNG
    }

    return fileName;
}

QString RYImpl::saveVoice(const QString &base64)
{
    const QString amrfileName = m_voicePath + Utility::getInstance()->getGuid()+".amr";
    qDebug()<<"amrfileName fileName:"<<amrfileName;
    QByteArray b64 = QByteArray::fromBase64(base64.toUtf8());
    QFile f(amrfileName);
    f.open(QIODevice::WriteOnly);
    f.write(b64);
    f.close();

    // 将AMR文件转WAV
     const QString wavfileName = m_voicePath + Utility::getInstance()->getGuid()+".wav";
    DecodeAmrToWav(amrfileName.toUtf8().constData(),wavfileName.toUtf8().constData());
    qDebug()<<"remove amr file"<<f.remove();
    return wavfileName;
}

void RYImpl::SendImage(const QString &json, int imgid)
{
    QString msg = json.toUtf8();
    QJsonObject obj = getJsonObjectFromString(msg);
    int errCode = obj.value("errorCode").toInt();
    QString result = obj.value("result").toString();
    QString img_id = obj.value("img_id").toString();
    QString targetId = QString(obj.value("targetId").toString());
    QString url = obj.value("url").toString();
    QString retStr = "";
    if(result.compare("success")==0){
        qDebug()<<"errorCode==0";
        // 先生成缩略图
        QString path = m_imagePath.replace("file:///","");
        qDebug() << QFile::exists(path) <<QFile::exists("file:///"+path);
        QImage img;
        img.load(path);
        // 等比例
        int scalew = img.width()<img.height() ? 200.00/img.height()*img.width() : 200 ;
        int scaleh = img.width()<img.height() ? 200 : (200.00/img.width()*img.height()) ;
        qDebug() << "width:"<<img.width();
        qDebug() << "height:"<<img.height();
        qDebug() << "scalew:"<<scalew;
        qDebug() << "scaleh:"<<scaleh;


        const QString fileName = m_picPath + Utility::getInstance()->getGuid()+".jpg";
        QImage cutImg = img.scaled(scalew, scaleh, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cutImg.save(fileName);
        retStr = fileName+"|"+url+"|"+ path; // 缩略图|网上图|原图
        emit RYImpl::getInstance()->uploadFileCallback(QString::number(imgid) ,retStr);
        // 保存消息

        QString base64data = Utility::getInstance()->getPictureBase64(fileName).toString();
        QString fmsg= tr("{\"content\":\"%1\",\"imageUri\":\"%2\",\"extra\":\"\"}").arg(base64data.toUtf8().data(),url);
        qDebug() << "image content:"<<fmsg;
        const wchar_t * msgw = reinterpret_cast<const wchar_t *>(fmsg.utf16());
//        int msgId = SaveMessage(targetId.toUtf8().data(), m_categoryId, "RC:ImgMsg", m_userid.toLatin1().data(), msgw, "", "");

        auto sendMessageCallback = [](const wchar_t* json_str)
        {
            QString u16 = QString::fromUtf16((const ushort*)json_str);
            QString retMsg = u16.toUtf8();
            QJsonObject obj = getJsonObjectFromString(retMsg);
            int msgUId = obj.value("messageId").toInt();
            int result = obj.value("result").toString() == "success" ? 1 : -1;
            emit RYImpl::getInstance()->sendMsgDealCallback(msgUId, result);
        };

        // 上传成功，发送消息
        sendMessage(targetId.toUtf8().data(), m_categoryId, 3, "RC:ImgMsg", msgw, "", "", imgid, sendMessageCallback);
    }else{
        qDebug()<<"results!=succes:"<<result;
        emit RYImpl::getInstance()->sendImageFailed(errCode);
    }
}