#include "Utility.h"
#include <QQuickItemGrabResult>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

Utility::Utility(QObject *parent) : QObject()
{
    Q_UNUSED(parent);
    connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &Utility::replyFinished);
}




QVariant Utility::getPictureBase64(QString path)
{
    QVariant data;
    qDebug() << QFile::exists(path) <<QFile::exists("file:///"+path);
    if(QFile::exists(path)){
        QByteArray array;
        QFile file(path);
        if(file.open(QIODevice::ReadOnly))
        {
            array = file.readAll();
            data = array.toBase64();
            file.close();
            qDebug()<<data;
        }
    }
    return data;// file is not exist
}


void Utility::uploadMaterial(QString url, QString filePath, QString materialType)
{
    crtUploadType = materialType;
    filePath = filePath.replace("file:///","");
    QFileInfo fileinfo(filePath);
    QString fileName = fileinfo.fileName();
    QString ext = fileinfo.completeSuffix();
    if(fileinfo.size() > 20 * 1024 * 1024){
        emit uploadMaterialRet(1, crtUploadType, "文件过大");
        return;
    }

    qDebug()<<" Utility::uploadImage" <<fileName<<ext<<endl;
//    QHttpPart imagePart;
//     imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
//     imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"image\""));
//     QFile *file = new QFile("image.jpg");
//     file->open(QIODevice::ReadOnly);
//     imagePart.setBodyDevice(file);
//     file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

//     multiPart->append(textPart);
//     multiPart->append(imagePart);

//     QUrl url("http://my.server.tld");
//     QNetworkRequest request(url);

//     QNetworkAccessManager manager;
//     QNetworkReply *reply = manager.post(request, multiPart);
//     multiPart->setParent(reply); // delete the multiPart with the reply
    QHttpMultiPart *picPost = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QString cth = "";
    if(materialType == "mImage"){
        cth = "image/"+ext;
    }
    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file_id\""));
    textPart.setBody("32323232");

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"file_data\" multiple filename=\"%1\"").arg(fileName)));
    QFile *file = new QFile(filePath);
    if (!file->exists()) {
        qDebug()<<"file not exists: "<<endl;
    }else{
        qDebug()<<"file ------- exists: "<<endl;
    }
    file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(file);
    file->setParent(picPost);
    qDebug()<<"file count: "<<file->bytesAvailable() <<endl;


    picPost->append(imagePart);
    QNetworkRequest request(QUrl(url.toLocal8Bit().data()));
    QNetworkReply *reply;
    reply = networkAccessManager.post(request, picPost);;
    connect(reply,&QNetworkReply::uploadProgress,this, &Utility::uploadProgress);
}

void Utility::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug()<<"http request failed."<<reply->errorString()<<endl;
        emit uploadMaterialRet(1, crtUploadType, reply->errorString());
    }
    else
    {
        QByteArray array = reply->readAll();
        qDebug()<<"------------Response: ------------\r\n"<<endl;
        qDebug()<<QString::fromUtf8(array)<<endl;
        qDebug()<<"------------Response END ------------\r\n"<<endl;
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(array,&error);
        if(error.error == QJsonParseError::NoError){
            QJsonObject obj = doc.object();
            if(obj.value("success").toBool()){
                emit uploadMaterialRet(0, crtUploadType,QString::fromUtf8(array));
            }else{
                emit uploadMaterialRet(1, crtUploadType,obj.value("msg").toString());
            }
        }else{
            emit uploadMaterialRet(1, crtUploadType,"上传失败");
        }
        reply->deleteLater();
    }
}

void Utility::uploadProgress(qint64 up, qint64 toal)
{
    if(up == toal){
        emit updateProgress("100");
    }else{
        emit updateProgress(QString::number(up*100/toal));
    }
}

