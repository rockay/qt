
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>

#include "qtdatabase.h"
QTDataBase* QTDataBase::m_instance = NULL;


void QTDataBase::initDB(const QString &user_id)
{
    QSqlDatabase database = QSqlDatabase::database();
    if (!database.isValid()) {
        database = QSqlDatabase::addDatabase("QSQLITE");
        if (!database.isValid())
            qFatal("Cannot add database: %s", qPrintable(database.lastError().text()));
    }

    QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    writeDir.setPath(writeDir.path()+"/"+user_id);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable directory at %s", qPrintable(writeDir.absolutePath()));

    // Ensure that we have a writable location on all devices.
    const QString fileName = writeDir.absolutePath() + "/quantu.db";
    // When using the SQLite driver, open() will create the SQLite database if it doesn't exist.
    database.setDatabaseName(fileName);
    qDebug()<<"db path:"<<fileName;
    if (!database.open()) {
        qFatal("Cannot open database: %s", qPrintable(database.lastError().text()));
        QFile::remove(fileName);
    }else{
        createContact();
        createConversations();
    }
}

void QTDataBase::createContact()
{
    if (QSqlDatabase::database().tables().contains("Contacts")) {
        // The table already exists; we don't need to do anything.
        return;
    }
    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Contacts' ("
                "   'user_id' TEXT NOT NULL,"
                "   'user_name' TEXT NOT NULL,"
                "   'user_remark' TEXT,"
                "   'user_photo' TEXT,"
                "   'last_msg' TEXT,"
                "   'categoryId' int,"  // 1 - PRIVATE 单人 , 3 - GROUP 群组
                "   'newcount' int,"      // 有条新消息
                "   'timestamp' DATETIME NOT NULL,"
                "   PRIMARY KEY(user_id)"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

void QTDataBase::createConversations()
{
    if (QSqlDatabase::database().tables().contains("Conversations")) {
        // The table already exists; we don't need to do anything.
        return;
    }
    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Conversations' ("
                "'msgUId' TEXT NOT NULL,"
                "'messageid' TEXT NOT NULL,"
                "'recipient' TEXT ,"
                "'senderid' TEXT,"
                "'message' TEXT NOT NULL," // 图片格式为 缩略图|融云地址|原图
                "'targetid' TEXT NOT NULL,"
                "'result' bool NOT NULL,"   // 0为新建 1为成功未读 -1为失败 2已读
                "'ctype' int ," // 1为文本 2为图片 与融云对应
                "'timestamp' DATETIME NOT NULL,"
                "'sendtime' DATETIME ,"
                "'rcvTime' DATETIME,"
                "   PRIMARY KEY(msgUId)"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}


