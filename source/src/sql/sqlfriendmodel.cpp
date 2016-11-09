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

#include "sqlfriendmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>

static const char *contactsTableName = "Friends";

static void createTable()
{
    if (QSqlDatabase::database().tables().contains(contactsTableName)) {
        // The table already exists; we don't need to do anything.
        return;
    }

    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Friends' ("
                "   'friend_id' int NOT NULL,"
                "   'friend_name' TEXT NOT NULL,"
                "   'friend_remark_name' TEXT,"
                "   'friend_mobile' TEXT,"
                "   'friend_photo' TEXT,"
                "   'friend_common' int,"
                "   'timestamp' DATETIME NOT NULL,"
                "   PRIMARY KEY(friend_id)"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

SqlFriendModel::SqlFriendModel(QObject *parent) :
    QSqlTableModel(parent)
{
    createTable();
    setTable(contactsTableName);
    setSort(5, Qt::DescendingOrder);
    select();
}

QVariant SqlFriendModel::data(const QModelIndex &index, int role) const
{
    if (role < Qt::UserRole)
        return QSqlTableModel::data(index, role);

    const QSqlRecord sqlRecord = record(index.row());
    return sqlRecord.value(role - Qt::UserRole);
}

QString SqlFriendModel::name() const
{
    return m_name;
}

void SqlFriendModel::setName(const QString &name)
{
    if (name == m_name)
        return;

    m_name = name;

    const QString filterString = QString::fromLatin1(
        " user_name LIKE '%1%'").arg(m_name);
    setFilter(filterString);
    select();

    emit nameChanged();
}\
QHash<int, QByteArray> SqlFriendModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::UserRole] = "friend_id";
    names[Qt::UserRole + 1] = "friend_name";
    names[Qt::UserRole + 2] = "friend_remark_name";
    names[Qt::UserRole + 3] = "friend_mobile";
    names[Qt::UserRole + 4] = "friend_photo";
    names[Qt::UserRole + 5] = "friend_common";
    names[Qt::UserRole + 6] = "timestamp";

    return names;
}

void SqlFriendModel::addFriend(const QString &friend_id, const QString &friend_name,const QString &friend_remark_name,
                                 const QString &friend_mobile, const QString &friend_photo, int friend_common)
{
    QDateTime timestamp = QDateTime::currentDateTime();

    QSqlRecord newRecord = record();
    newRecord.setValue("friend_id", friend_id);
    newRecord.setValue("friend_name", friend_name);
    newRecord.setValue("friend_remark_name", friend_remark_name);
    newRecord.setValue("friend_mobile", friend_mobile);
    newRecord.setValue("friend_photo", friend_photo);
    newRecord.setValue("friend_common", friend_common);
    newRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    if (!insertRecord(rowCount(), newRecord)) {
        qWarning() << "Failed to send message:" << lastError().text();
        return;
    }

    submitAll();
}

QString SqlFriendModel::updateFriend(int idx, const QString &last_msg)
{
    QDateTime timestamp = QDateTime::currentDateTime();
    qDebug()<<"updateFriend:"<<idx;
    QSqlRecord curRecord = record(idx);
    if(!last_msg.isNull() && !last_msg.isEmpty())
        curRecord.setValue("last_msg", last_msg);
    curRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));

    if (!setRecord(idx,curRecord)) {
        qWarning() << "Failed to send message:" << lastError().text();
        return "";
    }

    submitAll();
    select();
    QString str = curRecord.value(0).toString()+"|"+curRecord.value(1).toString()+"|"+curRecord.value(4).toString();
    return str;
}

void SqlFriendModel::addFriendById(const QString &friend_id, const QString &last_msg){
    // 先判断是否存在数据，存在则更新
    QSqlQuery query;
    QString sql = "SELECT count(*) FROM friends WHERE friend_id='"+friend_id+"'";
    if(!query.exec(sql)){
        return;
    }
    query.next();
    int count = query.value(0).toInt();
    if(count>0){
        // 更新数据库
        QDateTime timestamp = QDateTime::currentDateTime();
        sql = tr("UPDATE friends SET last_msg='%1',timestamp='%2' WHERE friend_id='%3'").arg(last_msg).arg(timestamp.toString("yyyy-MM-dd hh:mm:ss")).arg(friend_id);
        query.exec(sql);
        qDebug()<<sql;
    }

    submitAll();
    select();
}

QString SqlFriendModel::getId(int idx)
{
    QSqlRecord curRecord = record(idx);
    return curRecord.value("user_id").toString();
}

void SqlFriendModel::refresh()
{
    select();
}
