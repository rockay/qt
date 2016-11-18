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

#ifndef SQLCONVERSATIONMODEL_H
#define SQLCONVERSATIONMODEL_H

#include <QSqlTableModel>
#include<QTimer>

class SqlConversationModel : public QSqlTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString targetid READ targetid WRITE setTargetid NOTIFY targetidChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    SqlConversationModel(QObject *parent = 0);

    QString targetid() const;
    void setTargetid(QString targetid);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    Q_INVOKABLE bool addMessage(QString msgUId, QString messageid, QString recipient
                                 ,QString senderid, QString message, QString targetid
                                 , int result, int ctype, QString sendtime, QString curuser_id="");
    Q_INVOKABLE void updateMsgStatus(QString msgUId, int result, uint timestamp=0);
    Q_INVOKABLE void updateMsgStatusByLastTime(QString lasttime,QString senderid, QString targetid,QString recvTime, int result);
    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE void updateMsgContent(QString msgUId, QString content);
    Q_INVOKABLE void deleteMsgByID(QString msgUId);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE QString getLastMsgId(int senderid);         // 获取发送者给自己的最新messageid，也就是时间戳

signals:
    void targetidChanged();
    void countChanged(int c);
    void needRefresh();

public slots:
    void updateDBTable();
    void commitAll();
    void receviedModel();


private:
    QString m_targetid;
    QTimer *watchTimer;

    QString convert(QString orginalStr){
        return orginalStr.replace("'","''"); // 将单引号转义，不然数据库出错
    }
};

#endif // SQLCONVERSATIONMODEL_H
