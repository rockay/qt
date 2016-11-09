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

#include "sqlcontactmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>

static const char *contactsTableName = "Contacts";


SqlContactModel::SqlContactModel(QObject *parent) :
    QSqlTableModel(parent)
{
    setTable(contactsTableName);
    setSort(7, Qt::DescendingOrder);
    select();
    emit countChanged(rowCount());
}

QVariant SqlContactModel::data(const QModelIndex &index, int role) const
{
    if (role < Qt::UserRole)
        return QSqlTableModel::data(index, role);

    const QSqlRecord sqlRecord = record(index.row());
    return sqlRecord.value(role - Qt::UserRole);
}

QString SqlContactModel::name() const
{
    return m_name;
}

void SqlContactModel::setName(const QString &name)
{
    if (name == m_name)
        return;

    m_name = name;

    refresh();

    emit nameChanged();
}\
QHash<int, QByteArray> SqlContactModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::UserRole] = "user_id";
    names[Qt::UserRole + 1] = "user_name";
    names[Qt::UserRole + 2] = "user_remark";
    names[Qt::UserRole + 3] = "user_photo";
    names[Qt::UserRole + 4] = "last_msg";
    names[Qt::UserRole + 5] = "categoryId";
    names[Qt::UserRole + 6] = "newcount";
    names[Qt::UserRole + 7] = "timestamp";

    return names;
}

void SqlContactModel::addContacts(const QString &user_id, const QString &user_name, const QString &user_remark,
                                  const QString &user_photo, const QString &last_msg,int categoryId,int newcount)
{
    QDateTime timestamp = QDateTime::currentDateTime();

    QSqlRecord newRecord = record();
    newRecord.setValue("user_id", user_id);
    newRecord.setValue("user_name", user_name);
    newRecord.setValue("user_remark", user_remark);
    newRecord.setValue("user_photo", user_photo);
    newRecord.setValue("last_msg", last_msg);
    newRecord.setValue("categoryId", categoryId);
    newRecord.setValue("newcount", newcount);
    newRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    if (!insertRecord(rowCount(), newRecord)) {
        qWarning() << "Failed to addContacts:" << lastError().text();
        return;
    }

    submitAll();
    refresh();
    emit countChanged(rowCount());
}

QString SqlContactModel::updateContacts(int idx, const QString &last_msg)
{
    QDateTime timestamp = QDateTime::currentDateTime();
    QSqlRecord curRecord = record(idx);
    if(!last_msg.isNull() && !last_msg.isEmpty())
        curRecord.setValue("last_msg", last_msg);
    curRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));

    if (!setRecord(idx,curRecord)) {
        qWarning() << "Failed to updateContacts:" << lastError().text();
        return "";
    }

    submitAll();
    refresh();
    QString str = curRecord.value(0).toString()+"|"+curRecord.value(1).toString()+"|"+curRecord.value(5).toString();
    return str;
}

bool SqlContactModel::addContactById(const QString &user_id, const QString &last_msg, int newcount){
    bool flag = false;
    // 先判断是否存在数据，存在则更新
    QSqlQuery query;
    QString sql = "SELECT count(*) FROM contacts WHERE user_id='"+user_id+"'";
    if(!query.exec(sql)){
        return flag;
    }
    query.next();
    int count = query.value(0).toInt();
    if(count>0){
        // 更新数据库
        QDateTime timestamp = QDateTime::currentDateTime();
        sql = tr("UPDATE contacts SET last_msg='%1',timestamp='%2',newcount=%3 WHERE user_id='%4'")
                .arg(last_msg,timestamp.toString("yyyy-MM-dd hh:mm:ss"),QString::number(newcount),user_id);
        query.exec(sql);
        flag = true;
    }else{
        qDebug()<<sql;
    }

    submitAll();
    refresh();
    return flag;
}

void SqlContactModel::setCount(const QString &user_id, int newcount){
    // 先判断是否存在数据，存在则更新
    QSqlQuery query;
    QString sql = "SELECT count(*) FROM contacts WHERE user_id='"+user_id+"'";
    if(!query.exec(sql)){
        return;
    }
    query.next();
    int count = query.value(0).toInt();
    if(count>0){
        // 更新数据库
        sql = tr("UPDATE contacts SET newcount=%1 WHERE user_id='%2'")
                .arg(QString::number(newcount),user_id);
        query.exec(sql);
    }else{
        qDebug()<<sql;
    }

    submitAll();
    refresh();
}

int SqlContactModel::getId(int idx)
{
    QSqlRecord curRecord = record(idx);
    return curRecord.value("user_id").toInt();
}

QVariantMap SqlContactModel::get(int row) {
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

void SqlContactModel::remove(int row)
{
    removeRow(row);
    refresh();
}

void SqlContactModel::refresh()
{
    setTable(contactsTableName);
    const QString filterString = QString::fromLatin1(
        " user_name LIKE '%%1%' OR user_remark LIKE '%%1%' ").arg(m_name);
    setSort(7, Qt::DescendingOrder);
    setFilter(filterString);
    select();
    emit countChanged(rowCount());
}

