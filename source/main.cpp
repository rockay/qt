
#include<QGuiApplication>
#include<QQuickView>
#include<QColor>
#include<QQmlContext>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QSystemTrayIcon>
#include <winreg.h>

#include "qtquickcontrolsapplication.h"
#include "documenthandler.h"
#include "textballoon.h"
#include "ryimpl.h"
#include "Utility.h"
#include "sqlconversationmodel.h"
#include "sqlcontactmodel.h"
#include "sqlfriendmodel.h"
#include "sqlgroupmodel.h"
#include "sqlgroupmembermodel.h"
#include "qtdatabase.h"
#include "screenshot.h"
#include "MyGlobalShortCut.h"
#include "systemtray.h"
#include "downloadmanager.h"
#include "lmouselistner.h"
#include "filesavedialog.h"
#include "fileopendialog.h"

#pragma comment(lib,"AdvAPI32.lib")

//void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
// {
//    // 加锁
//    static QMutex mutex;
//    mutex.lock();
//    QByteArray localMsg = msg.toUtf8();
//    QString strMsg("");
//    switch(type)
//    {
//    case QtDebugMsg:
//        strMsg = QString("Debug:");
//        break;
//    case QtWarningMsg:
//        strMsg = QString("Warning:");
//        break;
//    case QtCriticalMsg:
//        strMsg = QString("Critical:");
//        break;
//    case QtFatalMsg:
//        strMsg = QString("Fatal:");
//        break;
//    }

//    // 设置输出信息格式
//    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
//    QString strMessage = QString("%5 Message:%1 File:%2  Line:%3  Function:%4  ")
//            .arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);

//    // 输出信息至文件中（读写、追加形式）
//    QFile file(strDateTime.mid(0,10)+".log");
//    file.open(QIODevice::ReadWrite | QIODevice::Append);
//    QTextStream stream(&file);
//    stream << strMessage << "\r\n";
//    file.flush();
//    file.close();

//    // 解锁
//    mutex.unlock();
// }

int main(int argc,char* argv[])
{
//    qInstallMessageHandler(myMessageOutput);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QtQuickControlsApplication app(argc, argv);
    app.setApplicationName("quantu");
    app.setOrganizationName("yiduotech");
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setApplicationVersion ("1.0.2");
    app.setWindowIcon(QIcon("logo.ico"));
    app.setQuitOnLastWindowClosed(false);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug()<<("I couldn't detect any system tray on this system.");
        return 1;
    }

    QSettings settings;
    settings.setObjectName("settings");
    QString user_id = settings.value("user_id").toString();

//    autoStart(true);
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
//    QSettings *reg=new QSettings("HKEY_LOCAL_MACHINE//SOFTWARE//Microsoft/Windows/CurrentVersion/Run",QSettings::NativeFormat);
    //开机自动运行
    QString autostartPath = QApplication::applicationFilePath().replace("/","\\");
    reg.setValue("quantu", autostartPath);

    QQmlApplicationEngine engine;

    // 先初始化下融云
    RYImpl::getInstance()->initPath(user_id);

    // 数据库
    QTDataBase::getInstance()->initDB(user_id);
    engine.rootContext()->setContextProperty("dbControl", QTDataBase::getInstance());

    //数据库
    qmlRegisterType<SqlContactModel>("org.lt.db", 1, 0, "SqlContactModel");
    qmlRegisterType<SqlConversationModel>("org.lt.db", 1, 0, "SqlConversationModel");

    //自定义文本框
    qmlRegisterType<DocumentHandler>("org.lt.controls", 1, 1, "DocumentHandler");
    qmlRegisterType<TextBalloon>("org.lt.controls", 1, 1, "TextBalloon");

    // 自定义保存、打开dialog
    qmlRegisterType<FileOpenDialog>("org.lt.controls", 1, 1, "FileOpenDialog");
    qmlRegisterType<FileSaveDialog>("org.lt.controls", 1, 1, "FileSaveDialog");

    // 截图
//    ScreenShot screenshot;
//    engine.rootContext()->setContextProperty("screenControl", &screenshot);
    MyGlobalShortCut *shortcut = new MyGlobalShortCut("Alt+Q",0); // 热键
    Utility *unit = Utility::getInstance();
    QObject::connect(shortcut,SIGNAL(activated()), unit,SLOT(shootScreen()));


    // 融云相关
    engine.rootContext()->setContextProperty("ryControl", RYImpl::getInstance());

    // 通用
    engine.rootContext()->setContextProperty("utilityControl", unit);

    // 下载
    engine.rootContext()->setContextProperty("networkControl", DownloadManager::getInstance());

    // 托盘
    SystemTray stray;
    engine.rootContext()->setContextProperty("systrayControl", &stray);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));


    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
