#include "tconversationthread.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

TConversationThread* TConversationThread::m_instance = NULL;

TConversationThread::TConversationThread(QObject *parent)
    : QThread(parent)
{
}

void TConversationThread::run()
{
    QString sql="";
    while(sqlList.length()>0){
        sql = sqlList.at(0);
        sqlList.removeAt(0);
        QSqlQuery query;
        try{
            if (!query.exec(sql)) {
                qDebug()<<sql;
                qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
            }
        }catch(QString exception) {
            qDebug()<<"TCONTACTTHREAD_H exception:"<<exception;
        }
    }
    emit updateFinished();
}
