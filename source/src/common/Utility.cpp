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
        screenshot = (DLLFuncd)mylib.resolve("PrScrn");
        if (screenshot == NULL)
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


bool Utility::uploadMaterial(QString url, QString filePath, QString materialType, int filefrom, QString messageid)
{
    // filefrom 0表示来自会话 1表示来自云上传
    crtUploadType = materialType;
    filePath = filePath.replace("file:///","");
    QFileInfo fileinfo(filePath);
    QString fileName = fileinfo.fileName();
    QString ext =  getFileExt(filePath);
    if(fileinfo.size() > 20 * 1024 * 1024){ // 20M
        emit uploadMaterialRet(1, crtUploadType, "文件过大", filefrom, messageid);
        return false;
    }
    QString cth = "";
    if(ext == "JPG" || ext == "BMP" || ext == "GIF"
            || ext == "JPEG" || ext == "ICO" || ext == "PNG") {
        cth = "image/"+ext;
    }else if(ext == "PDF"){
        cth = "application/"+ext;
    }else{
        emit uploadMaterialRet(2, crtUploadType, tr("文件格式不支持"), filefrom, messageid);
        return false;
    }

    qDebug()<<" Utility::uploadfile path:" <<filePath;
    qDebug()<<" Utility::uploadfile name: " <<fileName<<ext;

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
    currentUploadsID.insert(reply,messageid);
    currentUploadsFrom.insert(reply,filefrom);
    connect(reply,&QNetworkReply::uploadProgress,this, &Utility::uploadProgress);
    return true;
}

void Utility::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug()<<"http request failed."<<reply->errorString()<<endl;
        emit uploadMaterialRet(1, crtUploadType, reply->errorString(), currentUploadsFrom.value(reply), currentUploadsID.value(reply));
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
                emit uploadMaterialRet(0, crtUploadType,obj.value("file_info").toString(), currentUploadsFrom.value(reply), currentUploadsID.value(reply));
            }else{  // 错误码
                qDebug()<<"errorcode:"<<obj.value("errorcode").toInt()<<" m_filefrom:"<<currentUploadsFrom.value(reply);
                emit uploadMaterialRet(1, crtUploadType,tr("error code：%1").arg(obj.value("errorcode").toInt()), currentUploadsFrom.value(reply),  currentUploadsID.value(reply));
            }
        }else{
            qFatal("====上传失败 error message:%s",error.error);
            emit uploadMaterialRet(1, crtUploadType,"上传失败", currentUploadsFrom.value(reply), currentUploadsID.value(reply));
        }
        reply->close();
        reply->deleteLater();
        currentUploadsFrom.remove(reply);
        currentUploadsID.remove(reply);
    }
}

void Utility::uploadProgress(qint64 up, qint64 toal)
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if(up == toal){
        emit updateProgress(100, currentUploadsFrom.value(reply),currentUploadsID.value(reply));
    }else{
        emit updateProgress(up*100/toal, currentUploadsFrom.value(reply), currentUploadsID.value(reply));
    }
}

QString Utility::getGuid()
{
    return QUuid::createUuid().toString().replace("{","").replace("}","");
}

int Utility::getMessageId()
{
    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    int timeT = time.toMSecsSinceEpoch();   //将当前时间转为时间戳
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
    if (screenshot != NULL){
        int ret = screenshot(0);
        qDebug()<<"screenshot ret:"<<ret;
        if(ret==1){
            const QMimeData *mimeData = clipboard->mimeData();
            if (mimeData->hasImage()) {
                QPixmap picture = qvariant_cast<QPixmap>(mimeData->imageData());
                const QString temp = RYImpl::getInstance()->m_picPath + Utility::getInstance()->getGuid()+".jpg";
                picture.save(temp);
                emit Utility::getInstance()->captureSuccessed(temp);
            }
        }
        clipboard->clear();
    }
}

QString Utility::getFileExt(const QString &path)
{
    QFileInfo fileinfo(path);
    QString supportMulti = "tar.gz|";
    QString file_ext = fileinfo.completeSuffix().toLower();
    if(file_ext.indexOf(".")>=0){
        if(supportMulti.indexOf(file_ext)<0) // 不在支持带点的后缀名的列表中
        {
            file_ext = file_ext.right(file_ext.length()-file_ext.lastIndexOf(".")-1);
        }
    }
    return file_ext.toUpper();
}

QString Utility::getFileInfo(const QString &path)
{
    QString temp = path;
    QString filepath = temp.replace("file:///","");
    QFileInfo fileinfo(filepath);
    return QString::number(fileinfo.size())+"|"+fileinfo.fileName();
}

QString Utility::getFileFullInfo(const QString &path)
{
    // ext|mod|size|filename|fileurl file_mold：文件类型，1 表示图片类型，2 表示 PDF 类型
    QString temp = path;
    QString file_ext = getFileExt(temp);
    QString filepath = temp.replace("file:///","");
    QFileInfo fileinfo(filepath);
    int file_mode = 0;
    if(file_ext == "JPG" || file_ext == "BMP" || file_ext == "GIF"
            || file_ext == "JPEG" || file_ext == "ICO" || file_ext == "PNG") {
        file_mode = 1;
    }else if(file_ext == "PDF"){
        file_mode = 2;

    }
    QString retStr;
    retStr.append(file_ext);
    retStr.append("|");
    retStr.append(QString::number(file_mode));
    retStr.append("|");
    retStr.append(QString::number(fileinfo.size()));
    retStr.append("|");
    retStr.append(fileinfo.fileName());
    retStr.append("|");
    retStr.append(path);
    return retStr;
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

void Utility::releseAccount()
{
    qDebug()<<"ReleaseMutex"<<m_hutex;
    if(m_hutex != NULL){
        CloseHandle(m_hutex);
        m_hutex  =  NULL;
    }
}

bool Utility::checkIdIsLogin(const QString &userid)
{
    LPCWSTR key = (const wchar_t*) userid.utf16();
    HANDLE temp  =  CreateMutex(NULL, FALSE,  key);
    if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)  {
        CloseHandle(temp);
        temp  =  NULL;
        return  true;
    }
    else{
        m_hutex = temp;
        return false;
    }
}

bool Utility::isFileExist(const QString &path)
{
    if(QFile::exists(path)){
        return true;
    }
    return false;
}


