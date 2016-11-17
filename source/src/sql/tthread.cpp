#include "tthread.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

TThread* TThread::m_instance = NULL;

TThread::TThread(QObject *parent)
    : QThread(parent)
{
}

void TThread::run()
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
            qDebug()<<"TThread exception:"<<exception;

        }
    }
    emit updateFinished();
}
