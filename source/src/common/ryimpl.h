#ifndef RYIMPL_H
#define RYIMPL_H

#include <QObject>
typedef void(__stdcall *ConnectAckListenerCallback)(const wchar_t* json_str);

typedef void(__stdcall *PublishAckListenerCallback)(const wchar_t* json_str);
//typedef void(__stdcall *ImageListenerCallback)(const wchar_t* json_str);
//typedef void(__stdcall *ImageListenerCallback)(const wchar_t* json_str);

//typedef void(__stdcall *ConversationInfoCallback)(const wchar_t* json_str);

enum MSGTYPE{
    OTHER=0,            // 其他
    MESSAGE=1,          // 收到消息
    TYPING=2,           // 对方正在输入
    SENDTIME=3,         // lastMessageSendTime
    MSG_TXT=4,          // 收到文字消息
    MSG_IMG=5,          // 收到图片消息
    MSG_VC=6,           // 收到语音消息
    MSG_IMGTXT=7,       // 收到图片文字消息
    MSG_LBS=8,           // 收到位置消息
    MSG_CLOUDIMG=31           // 收到云库文件
};

class RYImpl : public QObject
{
    Q_OBJECT
public:
    static RYImpl* getInstance()
    {
        if(m_instance == NULL){
            m_instance = new RYImpl;
            m_instance->m_isLoaded = false; // 初始化没有加载
            m_instance->m_isConnected = false;
            m_instance->m_token = "";
        }
        return m_instance;
    }

    int m_categoryId;
    QString m_userid;
    QString m_imagePath;
    QString m_targetid;
    QString m_token;
    QString m_msguuid;

    QString m_rootPath;
    QString m_voicePath;
    QString m_picPath;


signals:
    void receivedMsg(int type,const QString &senderid,const QString &msgUid, const QString &messageid, const QString &msg,const QString &sendtime,int conversationType,const QString& targetid, bool isMetionedMe);
    void sendMsgCallback(const QString &retJson);
    void receivedException(const QString &code,const QString &data);
    void proceeFile(const QString &img_id, int process, int targetid);
    void uploadFileCallback(const QString &msgid, const QString &content);
    void sendMsgDealCallback(int msgid, int result);
    void sendImageFailed(int messageid,int errorcode);
    void recccvMsg(const QString &senderid);
public slots:
    void initLib(const QString &token, const QString &user_id);
    void connect();
    void disconnect();
    int sendMsg(int msgid,const QString &targetId,int categoryId, const QString &msg, int type, const QString &mention="");     // 发送消息，返回消息ID号
    int sendCloudMsg(int msgid,const QString &targetId,int categoryId, const QString &msg, int type);                           // 发送云文件消息
    void sendNtfMsg(const QString& msguid,const QString &targetId,int categoryId, const QString &msg);

    int GetChatList();
    QString saveImage(const QString & base64);
    QString saveVoice(const QString & base64);
    void SendImage(const QString &json, int imgid);    // 发送图片，返回图片存放路径

private:
    static RYImpl* m_instance;
    int convertype[3];

    bool m_isLoaded;
    bool m_isConnected;
    int m_imgmessageId;
public:
    typedef int(*DLLFunDis)(int);
    DLLFunDis Disconnect;

    typedef bool(*DLLFuncb)(const char* token, ConnectAckListenerCallback callback,bool ipv6);
    DLLFuncb Connect;

    typedef int(*DLLFunc)(const char *appId, const char* appName, const char* deviceId, const wchar_t* localPath, const wchar_t* databasePath);
    DLLFunc pInitClient;

    typedef int(*DLLFuncbb)(const char* targetId, int categoryId, const char* clazzName, const char* senderId, const wchar_t* message, const char* push, const char* appData);
    DLLFuncbb SaveMessage;

    typedef int(*DLLFunbc)(const char* targetId, int categoryId, int transferType, const char* clazzname, const wchar_t* message, const char* push, const char* appData, int messageId, PublishAckListenerCallback callback);
    DLLFunbc sendMessage;

    /**
        *  获取最近的会话列表
        *
        *  @param conversationDict	通道类型 //通道类型  1-PRIVATE 2-DISCUSSION 3-GROUP 4-CHATROOM 5-CUSTOMSERVICE
        *  @param conversationCount 数目
        *  @param callback			json结构的会话列表
        *
        *  @return 操作结果 true - 成功 false - 失败
        */
    typedef bool(*DFGetConversationList)(int conversationDict[], int conversationCount, PublishAckListenerCallback callback);
    DFGetConversationList GetConversationList;


    // 将AMR文件解码成WAVE文件
    typedef int (*DFDecodeAmrToWav)(const char* pchAMRFileName, const char* pchWAVEFilename);
    DFDecodeAmrToWav DecodeAmrToWav;

    typedef void (*DLLFunUP)(const char* targetId, int categoryId, int nType, const unsigned char* pbData, long nl, const char* img_id, PublishAckListenerCallback callback,PublishAckListenerCallback process_callback);
    DLLFunUP UpLoadFile;
};

#endif // RYIMPL_H
