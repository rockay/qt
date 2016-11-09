#include "lapplication.h"
#include <QMouseEvent>
#include <QDebug>

LApplication::LApplication(int &argc,char **argv):
    QApplication(argc,argv)
{

}

LApplication::~LApplication()
{

}

void LApplication::setWindowInstance(QWidget *wnd)
{
    widget = wnd;
}

bool LApplication::notify(QObject *obj, QEvent *e)
{
    const QMetaObject* objMeta = obj->metaObject();
    QString clName = objMeta->className();

    if(e->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        if(mouseEvent->buttons() == Qt::LeftButton)
            qDebug() << "left";
    }
    return QApplication::notify(obj,e);
}
