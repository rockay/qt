#ifndef RYIMPL_H
#define RYIMPL_H

#include <QObject>

typedef int(*DLLFunc)(const char *appId, const char* appName, const char* deviceId, const wchar_t* localPath, const wchar_t* databasePath);
typedef void(__stdcall *PublishAckListenerCallback)(const wchar_t* json_str);
//typedef void(__stdcall *ConversationInfoCallback)(const wchar_t* json_str);

enum MSGTYPE{
    OTHER=0,    // 其他
    MESSAGE=1,  // 收到消息
    TYPING=2,   // 对方正在输入
    SENDTIME=3  // lastMessageSendTime
};

class RYImpl : public QObject
{
    Q_OBJECT
public:
    static RYImpl* getInstance()
    {
        if(m_instance==NULL)
            m_instance = new RYImpl;
        return m_instance;
    }

    void initLib();
signals:
    void receivedMsg(int type,const QString &senderid,const QString &messageid, const QString &msg,const QString &sendtime);
    void receivedException(const QString &code,const QString &data);
public slots:
    int sendMsg(const QString &targetId,int categoryId, const QString &msg);         // 发送消息，返回消息ID号

    int GetChatList();

private:
    static RYImpl* m_instance;
    int convertype[3];

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

};

#endif // RYIMPL_H
