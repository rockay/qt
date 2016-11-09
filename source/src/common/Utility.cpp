#include "Utility.h"
#include <QQuickItemGrabResult>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QUuid>
#include <QScreen>
#include <QGuiApplication>
#include <QtWidgets>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QClipboard>
#include <QMimeData>
#include <QPixmap>
#include <QLibrary>
#include <QQuickItem>
#include <QQuickWindow>
#include <QVersionNumber>
#include "ryimpl.h"
#include "qtquickcontrolsapplication.h"

# pragma execution_character_set("utf-8")

Utility* Utility::m_instance = NULL;

Utility::Utility(QObject *parent) : QObject()
{
    Q_UNUSED(parent);
    updater = new UpdaterHttp;
    QLibrary mylib("PrScrn.dll");
    if(mylib.load()){
        print = (DLLFuncd)mylib.resolve("PrScrn");
        if (print == NULL)
            qDebug()<<"load screenshot PrScrn failed...";
    }else{
        qDebug()<<"load screenshot failed...";
    }
    connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &Utility::replyFinished);
}




QVariant Utility::getPictureBase64(QString path)
{
    path = path.replace("file:///","");
    QVariant data;
    qDebug() << "Utility::getPictureBase64 path:" << path;
    qDebug() << QFile::exists(path) <<QFile::exists("file:///"+path);
    if(QFile::exists(path)){
        QByteArray array;
        QFile file(path);
        if(file.open(QIODevice::ReadOnly))
        {
            array = file.readAll();
            data = array.toBase64();
            file.close();
            qDebug()<<"getPictureBase64 successed...";
        }
    }
    return data;// file is not exist
}


bool Utility::uploadMaterial(QString url, QString filePath, QString materialType, int filefrom)
{
    qDebug()<<"“uploadMaterial:"<<url;
    crtUploadType = materialType;
    m_filefrom = filefrom;  // 0表示来自会话 1表示来自云上传
    filePath = filePath.replace("file:///","");
    QFileInfo fileinfo(filePath);
    QString fileName = fileinfo.fileName();
    QString ext = fileinfo.completeSuffix();
    if(fileinfo.size() > 20 * 1024 * 1024){
        emit uploadMaterialRet(1, crtUploadType, "文件过大", m_filefrom);
        return false;
    }
    QString cth = "";
    if(materialType == "mImage"){
        cth = "image/"+ext;
    }else if(materialType == "mFile"){
        cth = "application/"+ext;
    }
    qDebug()<<" Utility::uploadfile path:" <<filePath<<endl;
    qDebug()<<" Utility::uploadfile name: " <<fileName<<ext<<endl;

    QFile file(filePath.replace("file:///",""));
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open file failed...";
        file.close();
        return false;
    }


    QString crlf="\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    QString b=QVariant(qrand()).toString()+QVariant(qrand()).toString()+QVariant(qrand()).toString();
    QString boundary="---------------------------"+b;
    QString endBoundary=crlf+"--"+boundary+"--"+crlf;
    QString contentType="multipart/form-data; boundary="+boundary;
    boundary="--"+boundary+crlf;
    QByteArray bond=boundary.toUtf8();
    QByteArray send;


    send.append(bond);
    boundary = crlf + boundary;
    bond = boundary.toUtf8();
    send.append(QString("Content-Disposition: form-data; name=\"name\""+crlf).toUtf8());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toUtf8());
    send.append(crlf.toUtf8());
    send.append(QString("文件名称").toUtf8());


    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"file\"; filename=\""+QString(fileName)+"\""+crlf).toUtf8());
    //send.append(QString("Content-Type: "+fileMimes.at(i)+crlf+crlf).toUtf8());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toUtf8());
    send.append(crlf.toUtf8());
    send.append(file.readAll());

    send.append(endBoundary.toUtf8());
    file.close();

    QUrl hostUrl(url);
    QNetworkRequest request(hostUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toUtf8());
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(send.size()).toString());


    QNetworkReply *reply;
    reply = networkAccessManager.post(request,send);
    connect(reply,&QNetworkReply::uploadProgress,this, &Utility::uploadProgress);
    return true;
}

void Utility::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug()<<"http request failed."<<reply->errorString()<<endl;
        emit uploadMaterialRet(1, crtUploadType, reply->errorString(), m_filefrom);
    }
    else
    {
        QByteArray array = reply->readAll();
        qDebug()<<QString::fromUtf8(array)<<endl;
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(array,&error);
        if(error.error == QJsonParseError::NoError){
            QJsonObject obj = doc.object();
            qDebug()<<"errorcode:"<<obj.value("errorcode").toInt();
            if(obj.value("errorcode").toInt()==-1){ // 上传成功
                emit uploadMaterialRet(0, crtUploadType,obj.value("file_info").toString(), m_filefrom);
            }else{  // 错误码
                qDebug()<<"errorcode:"<<obj.value("errorcode").toInt()<<" m_filefrom:"<<m_filefrom;
                emit uploadMaterialRet(1, crtUploadType,tr("error code：%1").arg(obj.value("errorcode").toInt()), m_filefrom);
            }
        }else{
            emit uploadMaterialRet(1, crtUploadType,"上传失败", m_filefrom);
        }
        reply->deleteLater();
    }
}

void Utility::uploadProgress(qint64 up, qint64 toal)
{
    if(up == toal){
        emit updateProgress(100, m_filefrom);
    }else{
        emit updateProgress(up*100/toal, m_filefrom);
    }
}

QString Utility::getGuid()
{
    return QUuid::createUuid().toString();
}

int Utility::getMessageId()
{
    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    int timeT = time.toTime_t();   //将当前时间转为时间戳
    return timeT;
}


bool Utility::b64dataToFile(const QString &fromData, const QString &savePath, const QString &myformat)
{

    QByteArray rc = QByteArray::fromBase64(fromData.toLatin1(), QByteArray::Base64Encoding);
    QByteArray rdc=qUncompress(rc);
    QImage img;
    img.loadFromData(rdc);
    bool ret = img.save(savePath);
    if(!ret)
        qDebug()<<" save iamge failed...";

    return ret;
}

void Utility::shootScreen()
{

//    QScreen *screen = QGuiApplication::primaryScreen();
//    qDebug()<<" screen..."<<screen;
//    QApplication::beep();
//    QPixmap originalPixmap = screen->grabWindow(0);
//    QSettings settings;
//    settings.setObjectName("settings");
//    QString user_id = settings.value("user_id").toString();
//    QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
//    const QString tempFile = writeDir.absolutePath()+"/" +user_id+"/"+Utility::getInstance()->getGuid()+".jpg";
//    originalPixmap.save(tempFile);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    if (print != NULL){
        bool ret = print();
        qDebug()<<"screenshot ret:"<<ret;
        const QMimeData *mimeData = clipboard->mimeData();
        if (mimeData->hasImage()) {
            QPixmap picture = qvariant_cast<QPixmap>(mimeData->imageData());
            const QString temp = RYImpl::getInstance()->m_picPath + Utility::getInstance()->getGuid()+".jpg";
            picture.save(temp);
            emit Utility::getInstance()->captureSuccessed(temp);
        }
        clipboard->clear();
    }
}

QString Utility::getFileInfo(QString path)
{
    QString filepath = path.replace("file:///","");
    QFileInfo fileinfo(filepath);
    return QString::number(fileinfo.size())+"|"+fileinfo.fileName();
}

QString unicodeToUtf8(const QString& unicode)
{
    QString result;
    int index = unicode.indexOf("0x");
    while (index != -1)
    {
        QString s1 = unicode.mid(index + 2, unicode.length()-2);
        qDebug()<<s1.toInt(0, 16);
        result.append(s1.toInt(0, 16));
        index = unicode.indexOf("0x", index+5);
    }
    return result;
}

QString Utility::getEmoji(const QString &code)
{
    QStringList strList = code.split(' ');
    QByteArray bty;
    foreach(QString str,strList){
        bool ok;
        int dec = str.toInt(&ok, 16);
        bty.append(dec);

    }
    return  QString::fromUtf8(bty);
}

void Utility::copy(const QString &text)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void Utility::quit()
{
    emit sigquit();
}

void Utility::checkUpdate(const QString &version, const QString &downloadpath, const QString &desc)
{
    qDebug()<<"version:"<<version;
    //    qDebug()<<"desc:"<<desc;
    QVersionNumber currentVersion = QVersionNumber::fromString(QtQuickControlsApplication::applicationVersion());
    QVersionNumber latestVersion = QVersionNumber::fromString(version);
    int compareRet = QVersionNumber::compare(currentVersion, latestVersion);
    bool isNeedUpdate = compareRet >= 0 ? false : true;
    qDebug()<<"isNeedUpdate"<< isNeedUpdate;
    if(isNeedUpdate)
    {
        qDebug()<<"currentVersion:"<<currentVersion;
        qDebug()<<"latestVersion:"<<latestVersion;
        updater->setDownloadUrl(downloadpath);
        updater->setLatestVersion(version);
        updater->setUpdateAvailable(true);
    }
}


