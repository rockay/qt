#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QStringList>
#include <QTimer>
#include <QUrl>

#include <stdio.h>

QT_BEGIN_NAMESPACE
class QSslError;
class QNetworkReply;
QT_END_NAMESPACE

class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;

public:
    DownloadManager();

    static DownloadManager* getInstance()
    {
        if(m_instance == NULL){
            m_instance = new DownloadManager;
        }
        return m_instance;
    }

    QString saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);

public slots:
    void doDownload(const QString &url, const QString &saveFileName);
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
    void downloadProgress(qint64 up, qint64 toal);
signals:
    void downloadSuccessed(QString fileName);
    void downloadFailed(QString fileName);
    void downProcess(int percent,const QString fileName);

private:
    QMap<QNetworkReply *,QString> currentDownloadsID;

    static DownloadManager* m_instance;
};

#endif // DOWNLOADMANAGER_H
