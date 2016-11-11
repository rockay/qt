#include "downloadmanager.h"
#include <QCoreApplication>
DownloadManager::DownloadManager()
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::doDownload(const QString &url, const QString &saveFileName)
{
    qDebug()<<"url:"<<url;
    qDebug()<<"save path:"<< saveFileName;
    QUrl urlpath = QUrl::fromEncoded(url.toUtf8());
    m_reply = manager.get(QNetworkRequest(urlpath));
    m_fileName = saveFileName;

    connect(m_reply,&QNetworkReply::downloadProgress,this, &DownloadManager::downloadProgress);

#ifndef QT_NO_SSL
    connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

//    currentDownloads.append(reply);
}

void DownloadManager::downloadProgress(qint64 up, qint64 toal)
{
    if(up == toal){
        emit downProcess(100, "123");
    }else{
        emit downProcess(up*100/toal, "123");
    }
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}


void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    /* Check if we need to redirect */
    QUrl redurl = reply->attribute (
                   QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!redurl.isEmpty()) {
        doDownload (redurl.toString(), m_fileName);
        return;
    }

    QUrl url = reply->url();
    if (reply->error()) {
        qDebug()<<"Download of %s failed: %s\n"<<url.toEncoded().constData()<<qPrintable(reply->errorString());
        emit downloadFailed();
        return;
    } else {
        QString filename = m_fileName; //saveFileName(url);
        if (saveToDisk(filename, reply))
            printf("Download of %s succeeded (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(filename));
    }

//    currentDownloads.removeAll(reply);
    reply->close();
    reply->deleteLater();
    qDebug()<<"下载完成";
    emit downloadSuccessed();

}
