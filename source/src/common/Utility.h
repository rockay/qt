#ifndef UTILITY_H
#define UTILITY_H

#include <QObject>
#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileInfo>
#include "Updater.h"

typedef int(*DLLFuncd)(int);

class Utility :  public QObject
{
    Q_OBJECT

public:
    explicit Utility(QObject *parent = 0);

    static Utility* getInstance()
    {
        if(m_instance==NULL)
            m_instance = new Utility;
        return m_instance;
    }


signals:
    void errorMessage(QString message);
    void decodingStarted();
    void decodingFinished(bool succeeded);
    void tagFound(QString idScanned);
    void getFinished(QString path);
    void uploadMaterialRet(int retCode, QString type, QString retMsg,int filefrom,QString messageid);
    void updateProgress(int percent,int filefrom, QString messageid="");
    void captureSuccessed(const QString &path);
    void sigquit();
    void sigshow(bool ischange);

public slots:
    QVariant getPictureBase64(QString path);
    bool uploadMaterial(QString url, QString filePath, QString materialType, int filefrom,QString messageid);
    void replyFinished(QNetworkReply *reply);
    void uploadProgress(qint64 up, qint64 toal);
    QString getGuid();
    int getMessageId();

    bool b64dataToFile(const QString &fromData, const QString &savePath, const QString &myformat="base64");

    void shootScreen();
    QString getFileInfo(const QString &path);
    QString getFileFullInfo(const QString &path);
    QString getFileExt(const QString &path);
    QString getEmoji(const QString &code);
    void copy(const QString &text);
    void quit();
    void checkUpdate(const QString& version, const QString& downloadpath, const QString& desc);

    bool checkIdIsLogin(const QString &userid); // 判断是否已经登录过
private:
    QNetworkAccessManager networkAccessManager;
    QString crtUploadType;
    int m_filefrom;

    UpdaterHttp *updater;
    DLLFuncd screenshot;
    QMap<QNetworkReply *,QString> currentUploads;


    static Utility* m_instance;
};

#endif // UTILITY_H
