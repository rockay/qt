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
int main(int argc,char* argv[])
{
    QGuiApplication app(argc,argv);
    app.setWindowIcon(QIcon("logo.ico"));
//    QQuickView viwer;
//    //无边框，背景透明
//    viwer.setFlags(Qt::FramelessWindowHint);
//    viwer.setColor(QColor(Qt::transparent));
//    //加载qml，qml添加到资源文件中可避免qml暴露
//    viwer.setSource(QUrl("qrc:/qml/login.qml"));
//    viwer.show();
//    //将viewer设置为main.qml属性
//    viwer.rootContext()->setContextProperty("mainwindow",&viwer);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    return app.exec();
}
