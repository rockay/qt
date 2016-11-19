

#include "sqlconversationmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSettings>
#include <QApplication>
#include "ryimpl.h"
#include "tconversationthread.h"

static const char *conversationsTableName = "Conversations";

SqlConversationModel::SqlConversationModel(QObject *parent) :
    QSqlTableModel(parent)
{
    connect(TConversationThread::getInstance(),SIGNAL(updateFinished()), this,SLOT(receviedModel()));
    watchTimer = new QTimer(this);
    connect(watchTimer,SIGNAL(timeout()),this,SLOT(updateDBTable()));
    watchTimer->setInterval(1000);

    setTable(conversationsTableName);
    setSort(9, Qt::AscendingOrder);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    emit countChanged(rowCount());
}

QString SqlConversationModel::targetid() const
{
    return m_targetid;
}

void SqlConversationModel::setTargetid(QString targetid)
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

bool SqlConversationModel::addMessage(QString msgUId, QString messageid, QString recipient
                                       ,QString senderid, QString message, QString targetid
                                       , int result, int ctype, QString sendtime,QString curuser_id)
{
    watchTimer->stop();

    QDateTime timestamp = QDateTime::currentDateTime();
    QString tempMsg = message;
    tempMsg = tempMsg.replace('\n',"<br/>");
    qDebug()<<"addMessage param..."<<msgUId<<messageid<<recipient<<senderid<<message<<targetid<<result<<ctype<<sendtime << curuser_id;
    QSqlRecord newRecord = record();
    newRecord.setValue("timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    newRecord.setValue("sendtime", sendtime.isEmpty() ? timestamp.toString("yyyy-MM-dd hh:mm:ss") : sendtime);
    newRecord.setValue("rcvtime", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    if(curuser_id.isNull() || curuser_id.isEmpty() || targetid == curuser_id  || (senderid == curuser_id && targetid == RYImpl::getInstance()->m_userid)){
        newRecord.setValue("msgUId", msgUId);
        newRecord.setValue("messageid", messageid);
        newRecord.setValue("recipient", recipient);
        newRecord.setValue("senderid", senderid);
        newRecord.setValue("message", tempMsg);
        newRecord.setValue("targetid", targetid);
        newRecord.setValue("result", result);
        newRecord.setValue("ctype", ctype);

        rowCount();
        if (!insertRecord(rowCount(), newRecord)) {
            qDebug() << "Failed to send message:" << lastError().text() <<  tableName();
            return false;
        }
    }

    // 替换转义符
    msgUId = convert(msgUId);
    messageid = convert(messageid);
    recipient = convert(recipient);
    senderid = convert(senderid);
    tempMsg = convert(tempMsg);
    targetid = convert(targetid);
    sendtime = convert(sendtime);
    curuser_id = convert(curuser_id);

    QString sql = tr(" INSERT into conversations(msgUId, messageid, recipient, senderid, message, targetid, result, ctype, timestamp, sendtime, rcvtime) VALUES('%1','%2','%3','%4','%5','%6',%7,%8,'%9','%10','%11')")
            .arg(msgUId, messageid, recipient, senderid, tempMsg, targetid, QString::number(result), QString::number(ctype)).arg(newRecord.value("timestamp").toString(), newRecord.value("sendtime").toString(), newRecord.value("rcvtime").toString());
    TConversationThread::getInstance()->sqlList.push_back(sql);
    watchTimer->start();
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

void SqlConversationModel::updateMsgStatus(QString msgUId, int result,uint timestamp)
{
    qDebug()<<"updateMsgStatus()...:"<<rowCount()<<msgUId << result<<timestamp;
    watchTimer->stop();


    for(int i=0; i< rowCount();i++){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("messageid") == msgUId){
            curRecord.setValue("result",result);
            QString sendtime = QDateTime::fromTime_t(QString::number(timestamp).mid(0,10).toInt()).toString("yyyy-MM-dd hh:mm:ss");
            if(timestamp!=0){
                curRecord.setValue("sendtime",sendtime);
            }
            setRecord(i,curRecord);

            // 替换转义符
            msgUId = convert(msgUId);
            QString sql = "";
            if(timestamp!=0){
                sql = tr(" UPDATE conversations SET result=%1,sendtime='%2' WHERE messageid='%3' ")
                        .arg(QString::number(result),sendtime,msgUId);
            }else{
                sql = tr(" UPDATE conversations SET result=%1 WHERE messageid='%2' ")
                        .arg(QString::number(result),msgUId);
            }
            TConversationThread::getInstance()->sqlList.push_back(sql);
            break;
        }
    }
    watchTimer->start();
}

void SqlConversationModel::updateMsgStatusByLastTime(QString lasttime, QString senderid, QString targetid, QString recvTime, int result)
{
    watchTimer->stop();


    for(int i=0; i< rowCount();i++){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("senderid") == senderid
                && curRecord.value("targetid") == targetid
                && curRecord.value("result") == 1
                && curRecord.value("sendtime") <= QDateTime::fromTime_t(lasttime.toLongLong())){
            curRecord.setValue("result",result);
            curRecord.setValue("rcvTime",recvTime);
            setRecord(i,curRecord);
        }
    }

    // 替换转义符
    lasttime = convert(lasttime);
    senderid = convert(senderid);
    targetid = convert(targetid);
    recvTime = convert(recvTime);
    QString sql = tr(" UPDATE conversations SET result=%1,rcvTime='%2' WHERE senderid='%3' AND targetid='%4' AND result=1 AND sendtime<='%5' ")
            .arg(QString::number(result), recvTime, senderid, targetid, QDateTime::fromTime_t(lasttime.toLongLong()).toString("yyyy-MM-dd hh:mm:ss"));
    TConversationThread::getInstance()->sqlList.push_back(sql);
    watchTimer->start();
}


void SqlConversationModel::updateMsgContent(QString msgUId, QString content)
{
    watchTimer->stop();


    for(int i=0; i< rowCount();i++){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("messageid") == msgUId){
            curRecord.setValue("message",content);
            setRecord(i,curRecord);

            // 替换转义符
            msgUId = convert(msgUId);
            content = convert(content);
            QString sql = tr(" UPDATE conversations SET message='%1' WHERE messageid='%2' ")
                    .arg(content, msgUId);
            TConversationThread::getInstance()->sqlList.push_back(sql);
            break;
        }
    }
    watchTimer->start();
}

void SqlConversationModel::deleteMsgByID(QString msgUId)
{

    qDebug()<<"deleteMsgByID()...:"<<msgUId;
    for(int i=0; i< rowCount();i++){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("messageid") == msgUId){
            removeRow(i);

            // 替换转义符
            msgUId = convert(msgUId);
            QString sql = tr("DELETE FROM conversations WHERE messageid='%1' ").arg(msgUId);
            qDebug()<<"sql...:"<<sql;
            TConversationThread::getInstance()->sqlList.push_back(sql);
            TConversationThread::getInstance()->start();
            break;
        }
    }
    refresh();
}

QString SqlConversationModel::getLastMsgId(int senderid)
{
    QString retStr = "";
    for(int i=rowCount()-1; i > 0; i--){
        QSqlRecord curRecord = record(i);
        if(curRecord.value("senderid") == senderid){
            retStr = curRecord.value("msgUId").toString() + "|" + QString::number(QDateTime::fromString(curRecord.value("sendtime").toString(),"yyyy-MM-dd hh:mm:ss").toTime_t())+"999";;
            break;
        }
    }
    return retStr;
}

void SqlConversationModel::refresh()
{
    if(TConversationThread::getInstance()->isRunning()){
        emit saveMsgINGNoRefresh();
        return;
    }
    // 替换转义符
    const QString filterString = QString::fromLatin1(
        " targetid = '%1'  OR (senderid = '%1' AND targetid='%2') ").arg(convert(m_targetid), convert(RYImpl::getInstance()->m_userid));
    setFilter(filterString);
    select();
    int count = rowCount();
    int limit = 3;
    if(count>limit){
        removeRows(limit,count-limit);
    }
    emit countChanged(rowCount());
}
void SqlConversationModel::updateDBTable()
{
    qDebug()<<"SqlConversationModel Thread start:"<<QDateTime::currentDateTime();
    m_lastTime = QDateTime::currentDateTime();
    if(TConversationThread::getInstance()->sqlList.count()>20){
        // 超过10条才发消息
        emit saveMsgING();
    }
    TConversationThread::getInstance()->start();
    watchTimer->stop();
}

void SqlConversationModel::receviedModel()
{
    qDebug()<<"SqlConversationModel Thread completed:"<<QDateTime::currentDateTime();
    TConversationThread::getInstance()->quit();
    refresh(); // 更新完了刷新一次
    if(m_lastTime.secsTo(QDateTime::currentDateTime()) > 3) // 超过3秒
        emit saveMsgFinished();
}


