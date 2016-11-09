#ifndef LMOUSELISTNER_H
#define LMOUSELISTNER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QDebug>
#ifdef Q_CC_MSVC
#include <windows.h>
    #pragma comment(lib, "user32.lib")
#endif

class LMouseListner : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_SIGNAL void outClicked();
public:
    explicit LMouseListner(QObject * parent = 0) : QObject(parent) {}

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE
    {
        if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
        {
            MSG * pMsg = reinterpret_cast<MSG *>(message);

//            qDebug()<<"message"<<pMsg->message;
            if (pMsg->message == WM_LBUTTONDOWN)
            {
                //获取到系统鼠标移动，可以做像qq一样的忙碌检测
                qDebug()<<"单击事件";
                emit outClicked();
            }
            if (pMsg->message == WM_COPYDATA)
            {
                //获取到系统鼠标移动，可以做像qq一样的忙碌检测
                qDebug()<<"WM_COPYDATA";
            }
        }

        return false;
    }
};

#endif // LMOUSELISTNER_H
