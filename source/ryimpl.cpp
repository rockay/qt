#include "ryimpl.h"
#include "rcsdk.h"
#include <QLibrary>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

RYImpl* RYImpl::m_instance = NULL;

QJsonObject getJsonObjectFromString(const QString jsonString){
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8().data());
    if( jsonDocument.isNull() ){
        qDebug()<< "===> please check the string "<< jsonString.toUtf8().data();
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

    emit RYImpl::getInstance()->receivedException(data,code);

}

//消息监听
void __stdcall message_callback(const wchar_t* json_str)
{
    QString u16 = QString::fromUtf16((const ushort*)json_str);
    qDebug()<<"消息监听:"<<u16.toUtf8();

    QString msg = u16.toUtf8();
    QJsonObject obj = getJsonObjectFromString(msg);
    QJsonObject objContent = getJsonObjectFromString(obj.value("m_Message").toString());

    // 这里判断是对方发送消息，还是对方在输入内容。
    QString content ="";
    MSGTYPE type = MSGTYPE::OTHER;
    if(objContent.contains("content"))
    {
        // 对方发送的消息
        type = MSGTYPE::MESSAGE;
        content = objContent.value("content").toString();
    }
    else if (objContent.contains("typingContentType")){
        // 对方发送的消息
        type = MSGTYPE::TYPING;
        content = "对方正在输入..."; //objContent.value("typingContentType").toString();
    }else if(objContent.contains("lastMessageSendTime")){
        type = MSGTYPE::SENDTIME;
        content = "对方最后发送消息时间："+objContent.value("lastMessageSendTime").toString();
    }else{
        content = obj.value("m_Message").toString();
    }
    QString sender = obj.value("m_SenderId").toString();
    QString sendtime = obj.value("m_SendTime").toString();
    QString messageid = obj.value("m_MessageId").toString();

    emit RYImpl::getInstance()->receivedMsg(type,sender,messageid,content,sendtime);
}

//// 会话列表
//void __stdcall conversation_callback(const wchar_t* json_str)
//{
//    QString u16 = QString::fromUtf16((const ushort*)json_str);
//    qDebug()<<"会话列表:"<<u16.toUtf8();

//}

void RYImpl::initLib()
{
    convertype[0]= 1;
    convertype[1]= 2;
    convertype[2]= 3;
    QString dir = QCoreApplication::applicationDirPath();
    QString path = tr("%1/rcsdk.dll").arg(dir);
    QFile file(path);
    if(file.exists())
        qDebug()<<"file exists..";
    QLibrary mylib(path);//声明dll文件路径
    if(mylib.load())//加载dll，并判断是否调用成功
    {
        pInitClient = (DLLFunc)mylib.resolve("InitClient");//援引接口函数
        qDebug()<<"load dll successed..";
        if(pInitClient != 0 )//判断是否连接上接口函数
        {
            const wchar_t * dir1 = reinterpret_cast<const wchar_t *>(dir.utf16());
            int ret = pInitClient("x4vkb1qpva0xk","quantu","deviceId", dir1, dir1);
            qDebug()<<"load pInitClient successed.."<<ret;
            char szToken[] = {"tmGNZPeAALCa/HzXiwx8VBjUT4yuNhCsE8yk4WJbGM2+Ok6dNPjwaXrKtspqLzEdWHnITJ3aHWb1CoOfm/CrNlL53KtrtPF2" };
            typedef int(*DLLFuncb)(const char* token, ConnectAckListenerCallback callback,bool ipv6);
            DLLFuncb Connect;
            Connect = (DLLFuncb)mylib.resolve("Connect");
            if (Connect == NULL)
            {
                qDebug()<<"load Connect failed..";
                return;
            }
            auto connectCallback = [](const wchar_t* json_str)
            {
                QString str1= QString::fromWCharArray(json_str);
                qDebug()<<"connectCallback json_str:"<<str1;
            };
            qDebug()<<"load Connect successed..";
            Connect(szToken, connectCallback,false);


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
                qDebug()<<("load RegisterMessageType successed");
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
                qDebug()<<"load 设置消息监听 successed..";
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
                qDebug()<<"load 设置网络异常监听 successed..";
                SetExceptionListener(exception_callback);
            }


            // 保存消息
            SaveMessage = (DLLFuncbb)mylib.resolve("SaveMessage");
            if (SaveMessage == NULL)
            {
                qDebug()<<"load SaveMessage failed..";
                return;
            }
            else
            {
                qDebug()<<"load SaveMessage successed..";
            }

            // 发送消息
            sendMessage = (DLLFunbc)mylib.resolve("sendMessage");
            if (sendMessage == NULL)
            {
                qDebug()<<"load sendMessage failed..";
                return;
            }
            else
            {
                qDebug()<<"load sendMessage successed..";


            }

            GetConversationList = (DFGetConversationList)mylib.resolve("GetConversationList");
            if (sendMessage == NULL)
            {
                qDebug()<<"load GetConversationList failed..";
                return;
            }
            else
            {
                qDebug()<<"load GetConversationList successed..";


            }


        }
        else
            qDebug()<<"load pInitClient fail..";
    }
    else
        qDebug()<<"load dll fail.."<<mylib.errorString();

//    char *text = "32323323\xF0\x9F\x98\x80";
//    ui->textEdit->setText(QString::fromUtf8(text));
}

int RYImpl::sendMsg(const QString &targetId,int categoryId, const QString &msg)
{
    int messageId = 0;
//    for(int i=0;i<msg.count();i+){
//         QString str = QString::fromUtf8(msg.at(i));
//        qDebug()<<str.size();
//    }
    qDebug()<<tr("targetid:%1 \t categoryid:%2 \t").arg(targetId,QString(categoryId));
    QString fmsg= tr("{\"content\":\"%1\",\"extra\":\"helloExtra\"}").arg(msg.toUtf8().data());
    const wchar_t * msgw = reinterpret_cast<const wchar_t *>(fmsg.utf16());
    if (SaveMessage != NULL)
        messageId = SaveMessage(targetId.toUtf8().data(), categoryId, "RC:TxtMsg", "1012332", msgw, "", "");
    QString u16 = QString::fromUtf16((const ushort*)msgw);
    qDebug()<<"msgw src:"<<u16.toUtf8()<<endl;
    auto sendMessageCallback = [](const wchar_t* json_str)
    {
        QString str1= QString::fromWCharArray(json_str);
        qDebug()<<"sendMessageCallback json_str:"<<str1;
    };
    if(sendMessage!=NULL)
        sendMessage(targetId.toUtf8().data(), categoryId, 3, "RC:TxtMsg", msgw, "", "", messageId, sendMessageCallback);
    return messageId;
}

int RYImpl::GetChatList(){
    qDebug()<<"GetChatList";
    auto getListCallback = [](const wchar_t* json_str)
    {
        QString str1= QString::fromWCharArray(json_str);
        qDebug()<<"getListCallback json_str:"<<str1;
    };
    if(GetConversationList != NULL)
        GetConversationList(convertype,3,getListCallback);
    return 0;
}
