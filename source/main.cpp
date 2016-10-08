//#include <QGuiApplication>
//#include <QQmlApplicationEngine>

//int main(int argc, char *argv[])
//{
//    QGuiApplication app(argc, argv);

//   QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

//    return app.exec();
//}
#include<QGuiApplication>
#include<QQuickView>
#include<QColor>
#include<QQmlContext>
#include <QQmlApplicationEngine>
#include <QSettings>

#include "qtquickcontrolsapplication.h"
#include "documenthandler.h"
int main(int argc,char* argv[])
{
    QGuiApplication::setApplicationName("Gallery");
    QGuiApplication::setOrganizationName("QtProject");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QGuiApplication app(argc,argv);
    QtQuickControlsApplication app(argc, argv);
    app.setWindowIcon(QIcon("logo.ico"));

    qmlRegisterType<DocumentHandler>("org.lt.controls", 1, 0, "DocumentHandler");
//    QSettings settings;
//    QString style = QQuickStyle::name();
//    if (!style.isEmpty())
//        settings.setValue("style", style);
//    else
//        QQuickStyle::setStyle(settings.value("style").toString());

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
