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

#include "sqlgroupmembermodel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

static const char *memberTableName = "Group_Member";

static void createTable()
{
    if (QSqlDatabase::database().tables().contains(memberTableName)) {
        // The table already exists; we don't need to do anything.
        return;
    }
    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Group_Member' ("
                "'group_id' int NOT NULL,"
                "'group_name' TEXT ,"
                "'user_id' int ,"
                "'user_name' int,"
                "'user_photo' TEXT ,"
                "'timestamp' DATETIME NOT NULL,"
                "   PRIMARY KEY(group_id,user_id)"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

SqlGroupMemmberModel::SqlGroupMemmberModel(QObject *parent) :
    QSqlTableModel(parent)
{
    createTable();
    setTable(memberTableName);
    setSort(8, Qt::DescendingOrder);
//    // Ensures that the model is sorted correctly after submitting a new row.
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

QString SqlGroupMemmberModel::targetid() const
{
    return m_targetid;
}

void SqlGroupMemmberModel::setTargetid(const QString &targetid)
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

QVariant SqlGroupMemmberModel::data(const QModelIndex &index, int role) const
{
    if (role < Qt::UserRole)
        return QSqlTableModel::data(index, role);

    const QSqlRecord sqlRecord = record(index.row());
    return sqlRecord.value(role - Qt::UserRole);
}

QHash<int, QByteArray> SqlGroupMemmberModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::UserRole] = "group_id";
    names[Qt::UserRole + 1] = "group_name";
    names[Qt::UserRole + 2] = "user_id";
    names[Qt::UserRole + 3] = "user_name";
    names[Qt::UserRole + 4] = "user_photo";
    names[Qt::UserRole + 5] = "timestamp";
    return names;
}

void SqlGroupMemmberModel::addGroupMember(const QString &group_id, const QString &group_name, const QString &user_id
                                       ,const QString &user_name, const QString &user_photo)
{
    QDateTime timestamp = QDateTime::currentDateTime();
    qDebug()<<"addGroup invoked..." ;
    QSqlRecord newRecord = record();
    newRecord.setValue("group_id", group_id);
    newRecord.setValue("group_name", group_name);
    newRecord.setValue("user_id", user_id);
    newRecord.setValue("user_name", user_name);
    newRecord.setValue("user_photo", user_photo);
    newRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    if (!insertRecord(rowCount(), newRecord)) {
        qDebug() << "Failed to send Group:" << lastError().text();
        return;
    }
    qDebug()<<"addGroup successed...";

    submitAll();
}
