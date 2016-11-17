#ifndef TCONVERSATIONTHREAD_H
#define TCONVERSATIONTHREAD_H

#include <QThread>
#include "sqlconversationmodel.h"

class TConversationThread : public QThread
{
    Q_OBJECT
public:
    TConversationThread(QObject *parent = 0);

    static TConversationThread* getInstance()
    {
        if(m_instance==NULL)
            m_instance = new TConversationThread;
        return m_instance;
    }


    void run();

    QStringList sqlList;
signals:
    void updateFinished();

private:
    static TConversationThread* m_instance;


};

#endif // TCONVERSATIONTHREAD_H
