#ifndef TTHREAD_H
#define TTHREAD_H

#include <QThread>
#include "sqlcontactmodel.h"

class TThread : public QThread
{
    Q_OBJECT
public:
    TThread(QObject *parent = 0);

    static TThread* getInstance()
    {
        if(m_instance==NULL)
            m_instance = new TThread;
        return m_instance;
    }

    void run();

    QStringList sqlList;
signals:
    void updateFinished();
private:
    static TThread* m_instance;


};

#endif // TTHREAD_H
