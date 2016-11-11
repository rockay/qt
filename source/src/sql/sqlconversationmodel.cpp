/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "sqlconversationmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSettings>
#include "ryimpl.h"

static const char *conversationsTableName = "Conversations";

SqlConversationModel::SqlConversationModel(QObject *parent) :
    QSqlTableModel(parent)
{
    setTable(conversationsTableName);
    setSort(9, Qt::AscendingOrder);
//    // Ensures that the model is sorted correctly after submitting a new row.
//    setEditStrategy(QSqlTableModel::OnManualSubmit);
    emit countChanged(rowCount());
}

QString SqlConversationModel::targetid() const
{
    return m_targetid;
}

void SqlConversationModel::setTargetid(const QString &targetid)
{
    if (targetid == m_targetid)
        return;

    m_targetid = targetid;
    refresh();
}

QVariant SqlConversationModel::data(const QModelIndex &index, int role) const
{
    if (role < Qt::UserRole)
        return QSqlTableModel::data(index, role);

    const QSqlRecord sqlRecord = record(index.row());
    return sqlRecord.value(role - Qt::UserRole);
}

QHash<int, QByteArray> SqlConversationModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::UserRole] = "msgUId";
    names[Qt::UserRole + 1] = "messageid";
    names[Qt::UserRole + 2] = "recipient";
    names[Qt::UserRole + 3] = "senderid";
    names[Qt::UserRole + 4] = "message";
    names[Qt::UserRole + 5] = "targetid";
    names[Qt::UserRole + 6] = "result";
    names[Qt::UserRole + 7] = "ctype";
    names[Qt::UserRole + 8] = "timestamp";
    names[Qt::UserRole + 9] = "sendtime";
    names[Qt::UserRole + 10] = "rcvtime";
    return names;
}

bool SqlConversationModel::addMessage(const QString &msgUId, const QString &messageid, const QString &recipient
                                       ,const QString &senderid, const QString &message, const QString &targetid
                                       , int result, int ctype, const QString &sendtime)
{
    QDateTime timestamp = QDateTime::currentDateTime();
    qDebug()<<"addMessage param..."<<msgUId<<messageid<<recipient<<senderid<<message<<targetid<<result<<ctype<<sendtime ;
    QSqlRecord newRecord = record();
    newRecord.setValue("msgUId", msgUId);
    newRecord.setValue("messageid", messageid);
    newRecord.setValue("recipient", recipient);
    newRecord.setValue("senderid", senderid);
    newRecord.setValue("message", message);
    newRecord.setValue("targetid", targetid);
    newRecord.setValue("result", result);
    newRecord.setValue("ctype", ctype);
    newRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    newRecord.setValue("sendtime", sendtime.isEmpty() ? timestamp.toString("yyyy-MM-dd hh:mm:ss") : sendtime);
    newRecord.setValue("rcvtime", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
//    QString sql = "INSERT INTO Conversations(msgUId,messageid,recipient,senderid,message,targetid,result,ctype,timestamp,sendtime) VALUES('%1','%2','%3','%4','%5','%6',%7,%8,'%9','%10')";
//    QSqlQuery query;
//    sql = sql.arg(msgUId).arg(messageid).arg(recipient).arg(senderid).arg(message).arg(targetid).arg(result?1:0).arg(ctype).arg(timestamp).arg(sendtime);
//    query.prepare(sql);
    rowCount();
    if (!insertRecord(rowCount(), newRecord)) {
        qDebug() << "Failed to send message:" << lastError().text() <<  tableName();
        return false;
    }

    submitAll();
    refresh();
    return true;
}

QVariantMap SqlConversationModel::get(int row) {
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
        //cout << i.key() << ": " << i.value() << endl;
    }
    return res;
}

void SqlConversationModel::updateMsgStatus(const QString &msgUId, int result,uint timestamp)
{
    qDebug()<<"updateMsgStatus sendtime:"<<timestamp;
    // 先判断是否存在数据，存在则更新
    QSqlQuery query;
    QString sql = "SELECT count(*) FROM Conversations WHERE messageid='"+msgUId+"'";
    if(!query.exec(sql)){
        return;
    }
    query.next();
    int count = query.value(0).toInt();
    if(count>0){
        // 更新数据库
        if(timestamp==0)
            sql = tr("UPDATE Conversations SET result=%1 WHERE messageid='%2'")
                .arg(QString::number(result),msgUId);
        else
        {
            QString sendtime = QDateTime::fromTime_t(QString::number(timestamp).mid(0,10).toInt()).toString("yyyy-MM-dd hh:mm:ss");
            sql = tr("UPDATE Conversations SET result=%1,sendtime='%3' WHERE messageid='%2'")
                .arg(QString::number(result),msgUId,sendtime);
        }
        query.exec(sql);
    }else{
        qDebug()<<sql;
    }

    submitAll();
    refresh();
}

void SqlConversationModel::updateMsgStatusByLastTime(const QString &messageid, const QString &senderid, const QString &targetid, const QString &recvTime, int result)
{
    // 先判断是否存在数据，存在则更新
    QSqlQuery query;
    QString  sql = tr("update conversations set result=%1,rcvTime='%2' where senderid='%3' and targetid='%4' and result=1 and messageid<='%5'")
            .arg(QString::number(result), recvTime, senderid, targetid, messageid);
    if(!query.exec(sql))
        qDebug() << "Failed to send message:" << lastError().text() <<  tableName();
    submitAll();
    refresh();
}


void SqlConversationModel::updateMsgContent(const QString &msgUId, const QString &content)
{
    // 先判断是否存在数据，存在则更新
    QSqlQuery query;
    QString sql = "SELECT count(*) FROM Conversations WHERE messageid='"+msgUId+"'";
    if(!query.exec(sql)){
        return;
    }
    query.next();
    int count = query.value(0).toInt();
    if(count>0){
        // 更新数据库
        sql = tr("UPDATE Conversations SET message='%1' WHERE messageid='%2'")
                .arg(content,msgUId);
        query.exec(sql);
    }else
        qDebug()<<sql;

    submitAll();
    refresh();
}

void SqlConversationModel::deleteMsgByID(const QString &msgUId)
{ QSqlQuery query;
    QString sql = "DELETE FROM Conversations WHERE messageid='"+msgUId+"'";
    if(!query.exec(sql)){
        qDebug() << "Failed to send message:" << lastError().text() <<  tableName();
        return;
    }

    submitAll();
    refresh();
}


void SqlConversationModel::refresh()
{
    setTable(conversationsTableName);
//    QSettings settings;
//    settings.setObjectName("settings");
//    QString user_id = settings.value("user_id").toString();

    const QString filterString = QString::fromLatin1(
        " targetid = '%1'  OR (senderid = '%1' AND targetid='%2') ").arg(m_targetid, RYImpl::getInstance()->m_userid);
    setFilter(filterString);
    setSort(9, Qt::AscendingOrder);
    select();
    emit countChanged(rowCount());
}



QString SqlConversationModel::getLastMsgId(int senderid)
{
    QString sql = tr("SELECT msgUId,sendtime FROM conversations WHERE senderid='%1' ORDER BY timestamp DESC LIMIT 0,1").arg(senderid);
    QString msgUID = "";
    QString timestamp = "";
    QString retStr = "";
    QSqlQuery query(sql);
    while (query.next()) {
        msgUID = query.value(0).toString();
        timestamp = QString::number(QDateTime::fromString(query.value(1).toString(),"yyyy-MM-dd hh:mm:ss").toTime_t())+"999";
        retStr = msgUID + "|" + timestamp;
        break;
    }
    return retStr;
}
