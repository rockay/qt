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

#include "sqlgroupmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

static const char *groupsTableName = "Groups";

static void createTable()
{
    if (QSqlDatabase::database().tables().contains(groupsTableName)) {
        // The table already exists; we don't need to do anything.
        return;
    }
    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Groups' ("
                "'group_id' int NOT NULL,"
                "'group_name' TEXT NOT NULL,"
                "'group_cover' TEXT ,"
                "'group_admin_id' int,"
                "'group_admin_name' TEXT ,"
                "'is_manager' int ,"
                "'manag_num' int ,"
                "'is_top' int ,"
                "'timestamp' DATETIME NOT NULL,"
                "   PRIMARY KEY(msgUId)"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

SqlGroupModel::SqlGroupModel(QObject *parent) :
    QSqlTableModel(parent)
{
    createTable();
    setTable(groupsTableName);
    setSort(8, Qt::DescendingOrder);
//    // Ensures that the model is sorted correctly after submitting a new row.
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

QString SqlGroupModel::targetid() const
{
    return m_targetid;
}

void SqlGroupModel::setTargetid(const QString &targetid)
{
    if (targetid == m_targetid)
        return;

    m_targetid = targetid;

    const QString filterString = QString::fromLatin1(
        " targetid = '%1'  OR senderid = '%1' ").arg(m_targetid);
    setFilter(filterString);
    select();

    emit targetidChanged();
}

QVariant SqlGroupModel::data(const QModelIndex &index, int role) const
{
    if (role < Qt::UserRole)
        return QSqlTableModel::data(index, role);

    const QSqlRecord sqlRecord = record(index.row());
    return sqlRecord.value(role - Qt::UserRole);
}

QHash<int, QByteArray> SqlGroupModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::UserRole] = "msgUId";
    names[Qt::UserRole + 1] = "Groupid";
    names[Qt::UserRole + 2] = "recipient";
    names[Qt::UserRole + 3] = "senderid";
    names[Qt::UserRole + 4] = "Group";
    names[Qt::UserRole + 5] = "targetid";
    names[Qt::UserRole + 6] = "result";
    names[Qt::UserRole + 7] = "ctype";
    names[Qt::UserRole + 8] = "timestamp";
    names[Qt::UserRole + 9] = "sendtime";
    names[Qt::UserRole + 10] = "rcvtime";
    return names;
}

void SqlGroupModel::addGroup(const QString &msgUId, const QString &Groupid, const QString &recipient
                                       ,const QString &senderid, const QString &Group, const QString &targetid
                                       , bool result, int ctype)
{
    QDateTime timestamp = QDateTime::currentDateTime();
    qDebug()<<"addGroup invoked..."<<msgUId<<Groupid<<recipient<<senderid<<Group<<targetid<<result<<ctype ;
    QSqlRecord newRecord = record();
    newRecord.setValue("msgUId", msgUId);
    newRecord.setValue("Groupid", Groupid);
    newRecord.setValue("recipient", recipient);
    newRecord.setValue("senderid", senderid);
    newRecord.setValue("Group", Group);
    newRecord.setValue("targetid", targetid);
    newRecord.setValue("result", result);
    newRecord.setValue("ctype", ctype);
    newRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    newRecord.setValue("sendtime", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    if (!insertRecord(rowCount(), newRecord)) {
        qDebug() << "Failed to send Group:" << lastError().text();
        return;
    }
    qDebug()<<"addGroup successed...";

    submitAll();
}
