#ifndef UTILITY_H
#define UTILITY_H

#include <QObject>
#ifdef Q_OS_ANDROID
#include "ebpcamera.h"
#include <QZXing/QZXing.h>
#endif
#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileInfo>


class Utility :  public QObject
{
    Q_OBJECT

public:
    explicit Utility(QObject *parent = 0);

signals:
    void errorMessage(QString message);
    void decodingStarted();
    void decodingFinished(bool succeeded);
    void tagFound(QString idScanned);
    void getFinished(QString path);
    void uploadMaterialRet(int retCode, QString type, QString retMsg);
    void updateProgress(QString percent);
public slots:
    QVariant getPictureBase64(QString path);
    void uploadMaterial(QString url, QString filePath, QString materialType);
    void replyFinished(QNetworkReply *reply);
    void uploadProgress(qint64 up, qint64 toal);

private:
    QNetworkAccessManager networkAccessManager;
    QString crtUploadType;
};

#endif // UTILITY_H
