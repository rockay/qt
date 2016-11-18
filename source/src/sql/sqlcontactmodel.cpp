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
#include "tcontactthread.h"

static const char *contactsTableName = "Contacts";


SqlContactModel::SqlContactModel(QObject *parent) :
    QSqlTableModel(parent)
{
    connect(TContactThread::getInstance(),SIGNAL(updateFinished()), this,SLOT(receviedModel()));
    watchTimer = new QTimer(this);
    connect(watchTimer,SIGNAL(timeout()),this,SLOT(updateDBTable()));
    watchTimer->setInterval(1000);

    setEditStrategy(OnManualSubmit);
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

void SqlContactModel::setName(QString name)
{
    if ( name == m_name)
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

void SqlContactModel::addContacts( QString user_id, QString user_name, QString user_remark,
                                   QString user_photo, QString last_msg,int categoryId,int newcount)
{
    watchTimer->stop();

    QDateTime timestamp = QDateTime::currentDateTime();
    for(int i=0; i< rowCount();i++){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("user_id") == user_id){
            // 已经存在，只需要更新可
            curRecord.setValue("user_name", user_name);
            curRecord.setValue("user_remark", user_remark);
            curRecord.setValue("user_photo", user_photo);
            curRecord.setValue("last_msg", last_msg);
            curRecord.setValue("categoryId", categoryId);
            curRecord.setValue("newcount", newcount);
            curRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
            setRecord(i,curRecord);

            // 替换转义符
            user_id = convert(user_id);
            user_name = convert(user_name);
            user_remark = convert(user_remark);
            user_photo = convert(user_photo);
            last_msg = convert(last_msg);
            QString sql = tr(" UPDATE contacts SET user_name='%1',user_remark='%2',user_photo='%3',last_msg='%4',categoryId=%5,newcount=%6,timestamp='%7' WHERE user_id='%8'")
                    .arg(user_name, user_remark, user_photo, last_msg, QString::number(categoryId), QString::number(newcount)
                         , timestamp.toString("yyyy-MM-dd hh:mm:ss"), user_id);
            TContactThread::getInstance()->sqlList.push_back(sql);
            watchTimer->start();
            return;
        }
    }



    QSqlRecord newRecord = record();
    newRecord.setValue("user_id", user_id);
    newRecord.setValue("user_name", user_name);
    newRecord.setValue("user_remark", user_remark);
    newRecord.setValue("user_photo", user_photo);
    newRecord.setValue("last_msg", last_msg);
    newRecord.setValue("categoryId", categoryId);
    newRecord.setValue("newcount", newcount);
    newRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    if (!insertRecord(0, newRecord)) {
        qWarning() << "Failed to addContacts:" << lastError().text();
        watchTimer->start();
        return;
    }

    // 替换转义符
    user_id = convert(user_id);
    user_name = convert(user_name);
    user_remark = convert(user_remark);
    user_photo = convert(user_photo);
    last_msg = convert(last_msg);
    QString sql = tr(" INSERT into contacts(user_id, user_name, user_remark, user_photo, last_msg, categoryId, newcount, timestamp) VALUES('%1','%2','%3','%4','%5',%6,%7,'%8')")
            .arg(user_id, user_name, user_remark, user_photo, last_msg, QString::number(categoryId), QString::number(newcount)
                 , timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    TContactThread::getInstance()->sqlList.push_back(sql);
    qDebug()<<"======addContacts successed";

    watchTimer->start();
}

QString SqlContactModel::updateContacts(int idx, QString last_msg)
{
    watchTimer->stop();

    QDateTime timestamp = QDateTime::currentDateTime();
    QSqlRecord curRecord = record(idx);
    if(!last_msg.isNull() && !last_msg.isEmpty())
        curRecord.setValue("last_msg", last_msg);
    curRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    qDebug()<<"updateContacts() called..";
    if (!setRecord(idx,curRecord)) {
        qWarning() << "Failed to updateContacts:" << lastError().text();
        return "";
    }

    // 替换转义符
    last_msg = convert(last_msg);

    QString sql = "";
    if(!last_msg.isNull() && !last_msg.isEmpty()){
        sql = tr(" UPDATE contacts SET last_msg='%1',timestamp='%2' WHERE user_id='%3' ")
                    .arg(last_msg,timestamp.toString("yyyy-MM-dd hh:mm:ss"),curRecord.value("user_id").toString());
    }else{
        sql = tr(" UPDATE contacts SET timestamp='%1' WHERE user_id='%2' ")
                    .arg(timestamp.toString("yyyy-MM-dd hh:mm:ss"),curRecord.value("user_id").toString());
    }
    TContactThread::getInstance()->sqlList.push_back(sql);
    watchTimer->start();

    QString str = curRecord.value(0).toString()+"|"+curRecord.value(1).toString()+"|"+curRecord.value(5).toString();
    return str;
}

bool SqlContactModel::addContactById(QString user_id, QString last_msg, int newcount){
    bool flag = false;
    watchTimer->stop();


    QDateTime timestamp = QDateTime::currentDateTime();
    for(int i=0; i< rowCount();i++){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("user_id") == user_id){
            flag = true;
            curRecord.setValue("last_msg",last_msg);
            curRecord.setValue("newcount",newcount);
            curRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));

            // 与0记录交换
            if(i!=0){
                QSqlRecord zeroRecord = record(0);
                setRecord(0,curRecord);
                setRecord(i,zeroRecord);
            }else{
                setRecord(i,curRecord);
            }


            qDebug()<<"addContactById() called.."<< curRecord.value("timestamp")<<newcount;

            // 替换转义符
            user_id = convert(user_id);
            last_msg = convert(last_msg);
            QString sql = tr(" UPDATE contacts SET last_msg='%1',timestamp='%2',newcount=%3 WHERE user_id='%4' ")
                    .arg(last_msg,timestamp.toString("yyyy-MM-dd hh:mm:ss"),QString::number(newcount),user_id);
            TContactThread::getInstance()->sqlList.push_back(sql);
            flag = true;
            break;
        }
    }
    emit countChanged(rowCount());
    watchTimer->start();
    return flag;
}

void SqlContactModel::setCount(QString user_id, int newcount){
    watchTimer->stop();

    for(int i=0; i< rowCount();i++){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("user_id") == user_id){
            curRecord.setValue("newcount",newcount);
            setRecord(i,curRecord);
            break;
        }
    }
    // 替换转义符
    user_id = convert(user_id);
    QString sql = tr(" UPDATE contacts SET newcount=%1 WHERE user_id='%2' ")
            .arg(QString::number(newcount),user_id);
    TContactThread::getInstance()->sqlList.push_back(sql);
    watchTimer->start();
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
    QSqlRecord curRecord = record(row);
    QString user_id = curRecord.value("user_id").toString();
    removeRow(row);
    QString sql = tr("DELETE FROM contacts WHERE user_id='%1' ").arg(user_id);
    TContactThread::getInstance()->sqlList.push_back(sql);
    TContactThread::getInstance()->start();
    refresh();

}

void SqlContactModel::refresh()
{
    setTable(contactsTableName);
    const QString filterString = QString::fromLatin1(
        " user_name LIKE '%%1%' OR user_remark LIKE '%%1%' ").arg(convert(m_name));
    setSort(7, Qt::DescendingOrder);
    setFilter(filterString);
    select();
    emit countChanged(rowCount());
}


void SqlContactModel::updateDBTable()
{
//    TContactThread::getInstance()->setContact(this);
    TContactThread::getInstance()->start();
    watchTimer->stop();
}

void SqlContactModel::commitAll()
{
//    submitAll();
//    emit needRefresh();
}

void SqlContactModel::receviedModel()
{
    qDebug()<<"Thread completed...............";
//    this = model;
//    memcpy(this,model,sizeof(SqlContactModel));
}
