#include "tcontactthread.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

TContactThread* TContactThread::m_instance = NULL;

TContactThread::TContactThread(QObject *parent)
    : QThread(parent)
{
}

void TContactThread::run()
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
            qDebug()<<"TContactThread exception:"<<exception;

        }
    }
    emit updateFinished();
}
