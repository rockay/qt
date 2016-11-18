#ifndef TCONTACTTHREAD_H
#define TCONTACTTHREAD_H

#include <QThread>
#include "sqlcontactmodel.h"

class TContactThread : public QThread
{
    Q_OBJECT
public:
    TContactThread(QObject *parent = 0);

    static TContactThread* getInstance()
    {
        if(m_instance==NULL)
            m_instance = new TContactThread;
        return m_instance;
    }

    void run();

    QStringList sqlList;
signals:
    void updateFinished();
private:
    static TContactThread* m_instance;


};

#endif // TCONTACTTHREAD_H
