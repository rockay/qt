#include "downloadmanager.h"
#include <QCoreApplication>
#include "Utility.h"
#include <QPixmap>
#include "ryimpl.h"

DownloadManager* DownloadManager::m_instance = NULL;

DownloadManager::DownloadManager()
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));

    connect(RYImpl::getInstance(),SIGNAL(needDownload(QString,QString)), this, SLOT(doDownload(QString,QString)));
}

void DownloadManager::doDownload(const QString &url, const QString &saveFileName)
{
    qDebug()<<"url:"<<url;
    qDebug()<<"save path:"<< saveFileName;
    QUrl urlpath = QUrl::fromEncoded(url.toUtf8());
    QNetworkReply *m_reply = manager.get(QNetworkRequest(urlpath));

    connect(m_reply,&QNetworkReply::downloadProgress,this, &DownloadManager::downloadProgress);

#ifndef QT_NO_SSL
    connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    currentDownloadsID.insert(m_reply,saveFileName);
}

void DownloadManager::downloadProgress(qint64 up, qint64 toal)
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    qDebug()<<"下载进度:"<<up*100/toal;
    if(up == toal){
        emit downProcess(100,currentDownloadsID.value(reply));
    }else{
        emit downProcess(up*100/toal, currentDownloadsID.value(reply));
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
    QString path = filename;
    path = path.replace("file:///","");
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(path),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    // 对图片特殊处理，因为有格式问题
    QString file_ext = Utility::getInstance()->getFileExt(path);
    if(file_ext == "JPG" || file_ext == "JPEG" || file_ext == "PNG") {
        QString orignalFile_ext = file_ext; // 原来的后缀名
        QPixmap img;
        if(!img.load(path,file_ext.toUtf8().data())) // 一直试图片格式
        {
            file_ext = "JPG";
            if(!img.load(path,"JPG")){
                file_ext = "PNG";
                if(!img.load(path,"PNG")){
                    file_ext = "BMP";
                    if(!img.load(path,"BMP")){
                        file_ext = "GIF";
                        if(!img.load(path,"GIF")){
                            file_ext = "ICO";
                            if(!img.load(path,"ICO")){
                                return false;
                            }
                        }
                    }
                }
            }
        }
        img.save(filename,orignalFile_ext.toUtf8().data());
    }
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
        doDownload (redurl.toString(),currentDownloadsID.value(reply));
        return;
    }

    QUrl url = reply->url();
    if (reply->error()) {
        qDebug()<<"Download of %s failed: %s\n"<<url.toEncoded().constData()<<qPrintable(reply->errorString());
        emit downloadFailed(currentDownloadsID.value(reply));
    } else {
        QString filename = currentDownloadsID.value(reply);
        if (saveToDisk(filename, reply)){
            printf("Download of %s succeeded (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(filename));
            qDebug()<<"下载成功";
            emit downloadSuccessed(currentDownloadsID.value(reply));
        }else{
            emit downloadFailed(currentDownloadsID.value(reply));
        }
    }

    reply->close();
    reply->deleteLater();
    currentDownloadsID.remove(reply);

}
