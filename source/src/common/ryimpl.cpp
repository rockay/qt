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
#include <QApplication>
#include <QEventLoop>
#include <QTimer>
#include "ryimpl.h"
#include "rcsdk.h"
#include "Utility.h"
#include "downloadmanager.h"

RYImpl* RYImpl::m_instance = NULL;
QJsonObject getJsonObjectFromString(const QString jsonString){
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8().data());
    if( jsonDocument.isNull() ){
        qDebug()<< "===> getJsonObjectFromString error string:"<< jsonString.toUtf8().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}

void writeLog(const QString &msg)
{
    qDebug()<<"msg:"<<msg;
    QString strDateTime = QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss");
    QString path1 = RYImpl::getInstance()->m_logPath+"timing.log";
    QString path2 = RYImpl::getInstance()->m_logPath+strDateTime.mid(0,8)+".log";
    QByteArray localMsg = msg.toUtf8();
    QString strMessage = QString("[%2]: Message:%1 \r\n\r\n")
            .arg(localMsg.constData()).arg(strDateTime);
    QFile outFile1(path1);
    QFile outFile2(path2);
    outFile1.open(QIODevice::WriteOnly | QIODevice::Append);
    if(outFile1.size() >= 1024*1024*5) // 5M
    {
        outFile1.close();
        outFile2.remove();
        QFile::copy(path1,path2);
        outFile1.remove();

        QFile outFile3(path1);
        outFile3.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile3);
        ts << strMessage << endl;
        outFile3.flush();
        outFile3.close();
    }
    else
    {
        QTextStream ts(&outFile1);
        ts << strMessage << endl;
    }
    outFile1.flush();
    outFile1.close();
}

//异常监听
void __stdcall exception_callback(const wchar_t* json_str)
{
    QString u16 = QString::fromUtf16((const ushort*)json_str);
    qDebug()<<"异常监听:"<<u16.toUtf8();
    writeLog("异常监听:"+u16.toUtf8());

    QJsonObject obj = getJsonObjectFromString(u16.toUtf8());
    QString data = obj.value("data").toString();
    int code = obj.value("nstatus").toInt();

    emit RYImpl::getInstance()->receivedException(QString::number(code), data);

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
    RYImpl::getInstance()->isReceving = true;
    QString u16 = QString::fromUtf16((const ushort*)json_str);
    QString msg = u16.toUtf8();
    writeLog("开始接收消息(message_callback):"+msg);

    QJsonObject obj = getJsonObjectFromString(msg);
    int count = obj.value("count").toInt();
    int conversationType = obj.value("m_ConversationType").toInt();

    if(count == 0){
        RYImpl::getInstance()->isReceving = false;
        qApp->processEvents();
        Sleep(400); //睡个100MS让其它程序处理
    }
    else if(conversationType == 3) // 如果是群组，则只需要收最近50条既可
    {

        QString sendtime = obj.value("m_SendTime").toString();
        QString recvTime = obj.value("m_RcvTime").toString();
        int ReadStatus = obj.value("m_ReadStatus").toInt();
        bool isDirection = obj.value("m_Direction").toBool();
        int days = QDateTime::fromString(sendtime,"yyyy-MM-dd hh:mm:ss").daysTo(QDateTime::fromString(recvTime,"yyyy-MM-dd hh:mm:ss"));
        int outDays = 2 ; // 2天前的消息不看了，或者自己发出的也不看
        int seconds = QDateTime::fromString(sendtime,"yyyy-MM-dd hh:mm:ss").secsTo(QDateTime::fromString(recvTime,"yyyy-MM-dd hh:mm:ss"));
        int outSeconds = 0.2*60 ; // 30秒之前的已读或自己发的消息就不再看了
        if((ReadStatus != 0 && isDirection && seconds > outSeconds)  // 已读，接收的，超过好多秒不显示
                || (ReadStatus==0 && isDirection && days>outDays)   // 未读，接收的，超过2天不接受
                || !isDirection )                                    // 自己发出去的不，不显示
        {
            RYImpl::getInstance()->isReceving = false;
            return;
        }
    }
    RYImpl::getInstance()->messageList.append(msg);
}

void RYImpl::initLib(const QString &token, const QString &user_id)
{
    m_token = token;
    m_userid = user_id;
    delayTimer = new QTimer(this);
    delayTimer->setInterval(300);
    delayTimer->setSingleShot(false);
    QObject::connect(delayTimer,SIGNAL(timeout()),this,SLOT(slotSendMsg()));

    initPath(user_id);

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
    QLibrary ryLib(path);//声明dll文件路径
    if(ryLib.load())//加载dll，并判断是否调用成功
    {
        pInitClient = (DLLFunc)ryLib.resolve("InitClient");//援引接口函数
        if(pInitClient == NULL )//判断是否连接上接口函数
        {
            qDebug()<<"load InitClient failed..";
            return;
        }
        const wchar_t * dir1 = reinterpret_cast<const wchar_t *>(dir.utf16());
        int ret = pInitClient("x4vkb1qpva0xk","quantu","deviceId", dir1, dir1);

        Connect = (DLLFuncb)ryLib.resolve("Connect");
        if (Connect == NULL)
        {
            qDebug()<<"load Connect failed..";
            return;
        }

        Disconnect = (DLLFunDis)ryLib.resolve("Disconnect");
        if (Disconnect == NULL)
        {
            qDebug()<<"load Disconnect failed..";
            return;
        }

        //注册消息类型
        typedef int(*DLLFuncd)(const char* clazzName, const unsigned int operateBits);
        DLLFuncd RegisterMessageType;
        RegisterMessageType = (DLLFuncd)ryLib.resolve("RegisterMessageType");
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
        SetMessageListener = (DLLFunce)ryLib.resolve("SetMessageListener");
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
        SetExceptionListener = (DLLFuncf)ryLib.resolve("SetExceptionListener");
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
        SaveMessage = (DLLFuncbb)ryLib.resolve("SaveMessage");
        if (SaveMessage == NULL)
        {
            qDebug()<<"load SaveMessage failed..";
            return;
        }

        // 发送消息
        sendMessage = (DLLFunbc)ryLib.resolve("sendMessage");
        if (sendMessage == NULL)
        {
            qDebug()<<"load sendMessage failed..";
            return;
        }

        // 获取历史会话
        GetConversationList = (DFGetConversationList)ryLib.resolve("GetConversationList");
        if (GetConversationList == NULL)
        {
            qDebug()<<"load GetConversationList failed..";
            return;
        }

        // 上传文件
        UpLoadFile = (DLLFunUP)ryLib.resolve("UpLoadFile");
        if (UpLoadFile == NULL)
        {
            qDebug()<<"load UpLoadFile failed..";
            return;
        }

        // 转换WAV文件
        DecodeAmrToWav = (DFDecodeAmrToWav)ryLib.resolve("DecodeAmrToWav");
        if (DecodeAmrToWav == NULL)
        {
            qDebug()<<"load DecodeAmrToWav failed..";
            return;
        }

        // 撤回消息
        Recall = (DLLFunRecall)ryLib.resolve("RecallMessage");
        if (Recall == NULL)
        {
            qDebug()<<"load RecallMessage failed..";
            return;
        }

    }
    else
        qDebug()<<"load dll fail.."<<ryLib.errorString();
    m_isLoaded = true;

    writeLog("ry init success...");
    delayTimer->start();
}

void RYImpl::connect()
{
    qDebug()<<"connect:"<<m_isConnected;

    // 已经连接要断开连接
    if(Disconnect != NULL){
        Disconnect(4);
    }

    if(Connect != NULL){
        auto connectCallback = [](const wchar_t* json_str)
        {
            QString u16 = QString::fromUtf16((const ushort*)json_str);
            qDebug()<<"异常监听:"<<u16.toUtf8();

            QJsonObject obj = getJsonObjectFromString(u16.toUtf8());
            QString data = obj.value("data").toString();
            int code = obj.value("nstatus").toInt();

            emit RYImpl::getInstance()->receivedException(QString::number(code), data);
        };
        if(Connect(m_token.toUtf8().data(), connectCallback,false)){
            m_isConnected = true;
            writeLog("ry Connect success...");
        }
    }
}

void RYImpl::disconnect()
{
    if(m_isConnected &&  Disconnect != NULL){
       qDebug()<<("disconnect");
        Disconnect(4);
        Utility::getInstance()->releseAccount();
    }
}

int RYImpl::sendMsg(int messageId, const QString &targetId,int categoryId, const QString &msg, int type,const QString& mention)
{
    m_categoryId = categoryId;
    m_targetid = targetId;

    qDebug()<<"sendMsg msg:"<<msg;
    QString tempmsg = msg;
    tempmsg = tempmsg.replace('\\',"\\\\").replace('\n',"\\n"); // 解决苹果上面显示问题
    QString fmsg= tr("{\"content\":\"%1\"}").arg(tempmsg.toUtf8().data());
    if(categoryId == 3 && !mention.isEmpty())
        fmsg = tr("{\"content\":\"%1\",\"mentionedInfo\":{\"type\":2,\"userIdList\":%2}}").arg(tempmsg.toUtf8().data(),mention);
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
        QVariant time = obj.value("timestamp").toVariant();
        int timestamp = QString::number(time.toLongLong()).left(10).toInt();
        emit RYImpl::getInstance()->sendMsgDealCallback(msgUId, result,timestamp);
    };
    if(type==MSGTYPE::MSG_IMG) // 只有发送图片才有上传
    {
        m_imagePath = msg;
        m_imgmessageId = messageId;
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
        uchar *picData = (uchar *)(blob.data());

        if(!img_idmap.contains(QString::number(messageId))){
            img_idmap.insert(QString::number(messageId),msg);
        }
        UpLoadFile(targetId.toUtf8().data(),categoryId,1,picData,blob.size(),QString::number(messageId).toUtf8().data(),sendImageCallback,processImageCallback);
        qDebug()<<"UpLoadFile ok:"<<messageId;
    }
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
    m_categoryId = categoryId;
    m_targetid = targetId;
    QStringList strList = msg.split("|");
    if(strList.count()!=5)
        qDebug()<<"云文件发送格式不对:"<<msg;
    QString fmsg= tr("{\"size\":%1,\"fileUrl\":\"%2\",\"name\":\"%3\",\"type\":\"%4\"}")
            .arg(strList.at(2),strList.at(4), strList.at(3), strList.at(0));
    const wchar_t * msgw = reinterpret_cast<const wchar_t *>(fmsg.utf16());
//    qDebug()<<tr("targetid:%1 \t categoryid:%2 \t content:%3").arg(targetId,QString(categoryId),fmsg);

    auto sendMessageCallback = [](const wchar_t* json_str)
    {
        QString u16 = QString::fromUtf16((const ushort*)json_str);
        QString retMsg = u16.toUtf8();
        qDebug()<<"send cloud messag callback:"<<retMsg;
        QJsonObject obj = getJsonObjectFromString(retMsg);
        int msgUId = obj.value("messageId").toInt();
        int result = obj.value("result").toString() == "success" ? 1 : -1;
        QVariant time = obj.value("timestamp").toVariant();
        int timestamp = QString::number(time.toLongLong()).left(10).toInt();
        emit RYImpl::getInstance()->sendMsgDealCallback(msgUId, result, timestamp);
    };
    if(sendMessage!=NULL){
        sendMessage(targetId.toUtf8().data(), categoryId, 3, "RC:FileMsg", msgw, "", "", messageId, sendMessageCallback);
    }
    return messageId;
}

int RYImpl::sendCustMsg(int messageId,const QString &targetId,int categoryId, const QString &msg)
{
    m_categoryId = categoryId;
    m_targetid = targetId;
    qDebug()<<"发送自定义格式消息:"<<msg;
    const wchar_t * msgw = reinterpret_cast<const wchar_t *>(msg.utf16());

    auto sendMessageCallback = [](const wchar_t* json_str)
    {
        QString u16 = QString::fromUtf16((const ushort*)json_str);
        QString retMsg = u16.toUtf8();
        qDebug()<<"send cust messag callback:"<<retMsg;
    };
    if(sendMessage!=NULL){
        qDebug()<<"targetId:"<<targetId;
        qDebug()<<"categoryId:"<<categoryId;
        qDebug()<<"messageId:"<<messageId;
        sendMessage(targetId.toUtf8().data(), categoryId, 3, "APP:ReadReceiptMessage", msgw, "", "", messageId, sendMessageCallback);
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
    qDebug()<<"imgid:"<<imgid;
    qDebug()<<"img_id:"<<img_id;
    QString targetId = QString(obj.value("targetId").toString());
    QString url = obj.value("url").toString();
    QString retStr = "";
    if(result.compare("success")==0){
        // 先生成缩略图
        QString tempPath = img_idmap.value(QString::number(imgid));
        QString path = tempPath.replace("file:///","");
        qDebug() << QFile::exists(path) <<QFile::exists("file:///"+path);
        QString file_ext = path.right(path.length()-path.lastIndexOf(".")-1);
        QPixmap img;
        if(!img.load(path,file_ext.toUtf8().data())) // 一直试图片格式
        {
            file_ext = "JPG";
            if(!img.load(path,"JPG")){
                file_ext = "PNG";
                if(!img.load(path,"PNG")){
                    file_ext = "BMP";
                    if(!img.load(path,"BMP")){
                        file_ext = "GIF";
                        if(!img.load(path,"GIF")){
                            file_ext = "ICO";
                            if(!img.load(path,"ICO")){
                                emit RYImpl::getInstance()->sendImageFailed(m_imgmessageId,errCode);
                                return ;
                            }
                        }
                    }
                }
            }
        }

        // 等比例
        int scalew = img.width()<img.height() ? 200.00/img.height()*img.width() : 200 ;
        int scaleh = img.width()<img.height() ? 200 : (200.00/img.width()*img.height()) ;
        qDebug() << "width:"<<img.width();


        const QString fileName = m_picPath + Utility::getInstance()->getGuid()+"."+file_ext.toLower();
        QPixmap cutImg = img.scaled(scalew, scaleh, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cutImg.save(fileName,file_ext.toUtf8().data());
        retStr = fileName+"|"+url+"|"+ path; // 缩略图|网上图|原图
        emit RYImpl::getInstance()->uploadFileCallback(QString::number(imgid) ,retStr);
        // 保存消息

        QString base64data = Utility::getInstance()->getPictureBase64(fileName).toString();
        QString fmsg= tr("{\"content\":\"%1\",\"imageUri\":\"%2\",\"extra\":\"\"}").arg(base64data.toUtf8().data(),url);
        const wchar_t * msgw = reinterpret_cast<const wchar_t *>(fmsg.utf16());
//        int msgId = SaveMessage(targetId.toUtf8().data(), m_categoryId, "RC:ImgMsg", m_userid.toLatin1().data(), msgw, "", "");

        auto sendMessageCallback1 = [](const wchar_t* json_str)
        {
            QString u16 = QString::fromUtf16((const ushort*)json_str);
            QString retMsg = u16.toUtf8();
            qDebug()<<"send image callback..."<<retMsg;
            QJsonObject obj = getJsonObjectFromString(retMsg);
            int msgUId = obj.value("messageId").toInt();
            int result = obj.value("result").toString() == "success" ? 1 : -1;
            QVariant time = obj.value("timestamp").toVariant();
            int timestamp = QString::number(time.toLongLong()).left(10).toInt();
            emit RYImpl::getInstance()->sendMsgDealCallback(msgUId, result, timestamp);
        };
//        qDebug()<<"m_txtlocked:"<<m_txtlocked;
//        while(m_txtlocked);
//        m_txtlocked = true;
        // 上传成功，发送消息
        sendMessage(targetId.toUtf8().data(), m_categoryId, 3, "RC:ImgMsg", msgw, "", "",imgid, sendMessageCallback1);
    }else{
        qDebug()<<"results!=succes:"<<result;
        emit RYImpl::getInstance()->sendImageFailed(imgid,errCode);
    }
}

void RYImpl::initPath(const QString &user_id)
{
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

    m_cachePicPath = m_rootPath +"/temp/pic/";
    writeDir.setPath(m_cachePicPath);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable cache directory at %s", qPrintable(writeDir.absolutePath()));

    m_cacheImagePath = m_rootPath +"/temp/images/";
    writeDir.setPath(m_cacheImagePath);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable cache directory at %s", qPrintable(writeDir.absolutePath()));

    m_logPath = m_rootPath+"/qlog/";
    writeDir.setPath(m_logPath);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable log directory at %s", qPrintable(writeDir.absolutePath()));
}

void RYImpl::slotSendMsg()
{
    // 没有接收消息，且有数据  或者  数据大于200条开始向上提交。
    while((!isReceving && messageList.count()>0) || messageList.count()>199){
        delayTimer->stop();

        QString msg = messageList.at(0);
        messageList.removeAt(0);

        QJsonObject obj = getJsonObjectFromString(msg);
        QJsonObject objContent = getJsonObjectFromString(obj.value("m_Message").toString());

     /*
        "m_TargetId":"99",          //交互方id
        "m_SenderId": "" ,          //发方id
        "m_ClazzName":"",		    //类别
        "m_ExtraMessage":"",        //信息
        "m_ExtraMessage":"",        //附加信息
        "m_Push":"",                //推送消息
        "m_AppData":"",             //AppData
        "m_ConversationType":0,     //通道类型  1-PRIVATE 2-DISCUSSION 3-GROUP 4-CHATROOM 5-CUSTOMSERVICE
        "m_MessageId":9,            //消息id   消息在数据库中的id号
        "m_Direction":0,            //消息方向  0-发送的消息 1-接收的消息
        "m_ReadStatus":0,           //读取状态  0-未读 1-已读
        "m_SendStatus":20,          //发送状态  10-发送中 20-发送失败 30-发送成功
        "m_SendTime":9,             //接收时间
        "m_RcvTime":9,              //接收时间
        "m_MessageType":0		    //信息类别  0-实时接收信息 1-历史漫游
    */
        QString content ="";
        MSGTYPE type = MSGTYPE::OTHER;
        QString msgUId = obj.value("m_MsgUId").toString();
        QString sender = obj.value("m_SenderId").toString();
        QString sendtime = obj.value("m_SendTime").toString();
        QString recvTime = obj.value("m_RcvTime").toString();
        QString targetid = obj.value("m_TargetId").toString();
        int ReadStatus = obj.value("m_ReadStatus").toInt();
        int messageid = obj.value("m_MessageId").toInt();
        int conversationType = obj.value("m_ConversationType").toInt();
        bool isDirection = obj.value("m_Direction").toBool();

        writeLog("===============send msg to qml:"+msg);

        // 如果是 1013463、 1011485 忽略这两个ID发的消息，直接忽略
        if(sender.compare("1013463") == 0 || sender.compare("1011485") == 0){
            writeLog("===============1013463、 1011485不处理==================\r\n\r\n");
            delayTimer->start();
            return;
        }

        if(objContent.contains("content") || objContent.contains("file_url"))
        {

            // 自己发的消息，且发送时间与现在大于10分钟
            int seconds = QDateTime::fromString(sendtime,"yyyy-MM-dd hh:mm:ss").secsTo(QDateTime::fromString(recvTime,"yyyy-MM-dd hh:mm:ss"));
            int outSeconds = 0.2*60 ; // 秒之前的已读或自己发的消息就不再看了
            if(sender.compare(RYImpl::getInstance()->m_userid) == 0
                    && (seconds > outSeconds)){
                writeLog("===============自己发的消息不处理==================\r\n\r\n");
                delayTimer->start();
                return ;
            }
            else if(sender.compare(RYImpl::getInstance()->m_userid) != 0
                    && ReadStatus != 0 && isDirection && seconds > outSeconds ){ // 接收的消息，且已读，且时间超过可看时间
                // 群组会重复发，所以这里把群组屏蔽掉。
                writeLog("===============其它端已读消息不接收时间超过可看时间，不处理==================\r\n\r\n");
                delayTimer->start();
                return;
            }

            // 告诉托盘闪烁
            emit RYImpl::getInstance()->recccvMsg("");

            // 对方发送的消息
            type = MSGTYPE::MESSAGE;
            // 判断是文字还是图片，还是其它
            QString className = obj.value("m_ClazzName").toString();
            content = objContent.value("content").toString();
            qDebug()<<"消息 className:"<<className;
            if(className.compare("RC:TxtMsg")==0) // 文字
            {
                type = MSGTYPE::MSG_TXT;
    //            qDebug()<<"文字消息:"<<content;
            }
            else if(className.compare("RC:ImgMsg")==0) // 图片
            {
                type = MSGTYPE::MSG_IMG;
                QString uri = objContent.value("imageUri").toString();
    //            qDebug()<<"图片消息Uri:"<<uri;
                content = RYImpl::getInstance()->saveImage(content); // 返回路径保存
                QString localpath = RYImpl::getInstance()->m_cacheImagePath+Utility::getInstance()->getGuid()+".png";
                emit RYImpl::getInstance()->needDownload(uri, localpath);
                // 保存缩略图路径和远程路径
                content = content+"|"+uri+"|"+localpath;
    //            qDebug()<<"图片缩略图消息message:"<<content;
            }
            else if(className.compare("RC:VcMsg")==0) // 语音
            {
                type = MSGTYPE::MSG_VC;
    //            qDebug()<<"语音消息:"<<content;
                int duration = objContent.value("duration").toInt(); // 语音长度
                content = RYImpl::getInstance()->saveVoice(content); // 返回路径保存
                content = QString::number(duration)+"|"+content; // 时长｜内容
    //            qDebug()<<"语音存放路径:"<<content;
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
                QString file_ext = objContent.value("file_ext").toString();
                int file_mold= objContent.value("file_mold").toInt();
                int file_size = objContent.value("file_size").toInt();
                QString file_name  = objContent.value("file_name").toString();
                QString file_url = objContent.value("file_url").toString();
                // 保存缩略图路径和远程路径
                content = QString("%1|%2|%3|%4|%5").arg(file_ext, QString::number(file_mold), QString::number(file_size), file_name, file_url);
            }
            else if(className.toLower().compare("app:receiptmessage")==0) //|| className.compare("app:readreceiptnessage")==0) // 回执消息
            {
                type = MSGTYPE::MSG_ReceiptMessage;
                QString subcontent = objContent.value("content").toString();
                QString sendUserName= ""; //objContent.value("sendUserName").toString();
                QString sendUserId = ""; //objContent.value("sendUserId").toString();
                QString groupId  = objContent.value("userId").toString();
                content = QString("%1|%2|%3|%4").arg(subcontent, sendUserName, sendUserId, groupId);
                qDebug()<<"回执消息 content:"<<content;
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
            // 消息已读
        }
        else if(obj.value("m_ClazzName").toString().toLower().compare("rc:filemsg")==0)  // 文件消息
        {
            type = MSGTYPE::MSG_FILE;
            QString file_ext = objContent.value("type").toString().toLower();
            int file_mold= 4; //默认先用4 其它
            if(file_ext == "jpg" || file_ext == "jpeg" || file_ext == "png" || file_ext == "bmp" || file_ext == "ico" || file_ext == "gif") {
                file_mold = 1;
            }else if(file_ext == "pdf"){
                file_mold = 2;
            }else if(file_ext == "ppt" || file_ext == "pptx" || file_ext == "xls" || file_ext == "xlsx" || file_ext == "doc" || file_ext == "docx"){
                file_mold = 3;
            }
            int file_size = objContent.value("size").toInt();
            QString file_name  = objContent.value("name").toString();
            QString file_url = objContent.value("fileUrl").toString();
            // 保存缩略图路径和远程路径
            content = QString("%1|%2|%3|%4|%5").arg(file_ext, QString::number(file_mold), QString::number(file_size), file_name, file_url);
            qDebug()<<"文件消息 content:"<<content;
        }
        else{
            content = obj.value("m_Message").toString();
            delayTimer->start();
            // 不知道的格式，直接返回
            return;
        }

        QString rcvTime = obj.value("m_RcvTime").toString();

        QJsonObject objMention = objContent.value("mentionedInfo").toObject();
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
        qApp->processEvents();
    }
    delayTimer->start(); //执行完再开启
}

void RYImpl::reCallMessage(const QString &msgid)
{
    if(msgid.isNull() || msgid.isEmpty())
        return;
    emit recallMessageFinished(Recall(msgid.toUtf8().data()));
}
