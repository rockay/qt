#include "screenshot.h"
#include <QFileDialog>
#include <QDateTime>
#include "global.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QPainter>
#include <QBuffer>
#include <QApplication>
ScreenShot::ScreenShot(QWidget *parent) :
    QLabel(parent)
{
    setWindowFlags(Qt::WindowStaysOnTopHint);  //使窗口置顶
    showFullScreen();

    /*显示阴影的label*/
    label = new QLabel(this);
    label->resize(size());
    label->show();

    /*显示高亮图像的部分*/
    pixLabel = new QLabel(this);
    pixLabel->hide();

    /*更新自动显示的区域的定时器*/
    timer = new QTimer(this);
    timer->setInterval(300);
    connect(timer,SIGNAL(timeout()),this,SLOT(slotUpdateCusorWindowInfo()));

    /***选择范围***/
    selectRect = new SelectRect;
    selectRect->setParent(this);
    connect(selectRect,SIGNAL(rectChanged(QRect)),this,SLOT(slotRectChanged(QRect)));
    connect(selectRect,SIGNAL(releaseWithoutMove()),this,SLOT(slotAutoPixSelect()));

    initToolWidget();
    hide();

    State = FINISHED;
}

ScreenShot::~ScreenShot()
{

}

void ScreenShot::initToolWidget()
{
//    ui = new Ui::ToolWidget;
//    toolWidget = new QWidget(this);
//    ui->setupUi(toolWidget);
//    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(slotSaveBtnClick()));
//    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(slotCloseBtnClick()));
//    connect(ui->okButton,SIGNAL(clicked()),this,SLOT(slotOkbtnClick()));
}

void ScreenShot::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (event->type() == QEvent::MouseButtonDblClick)
        {
            if (State == SHOTTING)
            {
                slotOkbtnClick();
            }
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (State == READY)
        {
            finishShot();
        }
        else
        {
            finishShot();
            getReadyToShot();
        }
    }
}

void ScreenShot::getReadyToShot()
{
    autoRect.setTopLeft(QPoint(-100,-100));
    autoRect.setSize(QSize(0,0));
    rect.setTopLeft(QPoint(-100,-100));
    rect.setSize(QSize(0,0));

    QPixmap shotPix = QPixmap::grabWindow(QApplication::desktop()->winId());
    fullScreenPixmap = new QPixmap;
    *fullScreenPixmap = shotPix.copy();
    this->setPixmap(*fullScreenPixmap);

    QPixmap pix = QPixmap(size());
    pix.fill(QColor(25,25,25,127));  //用蓝色填充这个pixmap  且透明度为127 (255表示不透明)
    label->setPixmap(pix);
    label->show();

    pixmap = new QPixmap;
    oldCursorPos = QPoint(-100,-100);
    selectRect->getReadyToSelect();

    State = READY;
    showFullScreen();
    setCursor(Qt::CrossCursor);
    slotUpdateCusorWindowInfo();
    timer->start();
}

void ScreenShot::finishShot()
{
    State = FINISHED;
    pixLabel->clear(); //显示高亮图像的label
    pixLabel->hide();
    this->clear();
    this->hide();
    label->clear();
    label->hide();
    toolWidget->hide();

    delete fullScreenPixmap;
    fullScreenPixmap = NULL;
    delete pixmap;
    pixmap = NULL;

    selectRect->clear();
}

void ScreenShot::updateAutoPix()
{
    if (State != READY) return;
    int x = autoRect.x();
    int y = autoRect.y();
    int borderWidth = 2; //边框的宽度
    int width = autoRect.width();
    int height= autoRect.height();
    QPixmap autoPix = fullScreenPixmap->copy(x,y,width,height);
    QSize totalSize = QSize(width + borderWidth*2,height + borderWidth *2);
    QPixmap pix  = QPixmap(totalSize);
    pix.fill(Qt::yellow);
    QPainter painter(&pix);
    painter.drawPixmap(borderWidth,borderWidth,autoPix);

    if (x <= 0) //区域超出左边
    {
        painter.setPen(QPen(Qt::yellow,borderWidth*2,Qt::SolidLine,Qt::RoundCap));
        painter.drawLine(QPoint(borderWidth,0),QPoint(borderWidth,height));
    }
    if (y <= 0) //区域超出上边
    {
        painter.setPen(QPen(Qt::yellow,borderWidth*2,Qt::SolidLine,Qt::RoundCap));
        painter.drawLine(QPoint(0,borderWidth),QPoint(width,borderWidth));
    }
    if ((x + width) >= fullScreenPixmap->width()) //区域超出右边
    {
        painter.setPen(QPen(Qt::yellow,borderWidth*2,Qt::SolidLine,Qt::RoundCap));
        painter.drawLine(QPoint(width+borderWidth,0),QPoint(width+borderWidth,height));
    }
    if ((y + height) >= fullScreenPixmap->height()) //区域超出下边
    {
        painter.setPen(QPen(Qt::yellow,borderWidth*2,Qt::SolidLine,Qt::RoundCap));
        painter.drawLine(QPoint(0,height+borderWidth),QPoint(width,height+borderWidth));
    }

    pixLabel->setPixmap(pix);
    pixLabel->resize(totalSize);
    pixLabel->move(x-borderWidth,y-borderWidth);
    pixLabel->show();
}

void ScreenShot::updatePixmap()
{
    int x = rect.left();
    int y = rect.top();
    int width = rect.width();
    int height= rect.height();
    *pixmap = fullScreenPixmap->copy(x,y,width,height);
    pixLabel->setPixmap(*pixmap);
    pixLabel->resize(rect.size());
    pixLabel->move(rect.topLeft());
    pixLabel->show();
    showToolWidget();
}

void ScreenShot::showToolWidget()
{
    int x = rect.x();
    int y = rect.y() + rect.height();
//    if ( (y + toolWidget->height()) > this->height())  y = y - toolWidget->height();
//    if (rect.width() > toolWidget->width()) x = x + rect.width() - toolWidget->width();
//    toolWidget->move(x,y);
//    toolWidget->show();
}

void ScreenShot::slotAutoPixSelect()
{
    State = SHOTTING;
    timer->stop();
    rect = autoRect;
    updatePixmap();
    selectRect->setRect(rect);
}

void ScreenShot::slotRectChanged(QRect re)
{
    State = SHOTTING;
    timer->stop();
    rect = re;
    updatePixmap();
}

void ScreenShot::slotSaveBtnClick()
{
    QDateTime dateTime = QDateTime::currentDateTime();//获取系统现在的时间
    QString fileName = "屏幕截图" + dateTime.toString("yyyyMMddhhmmss") + ".png"; //设置显示格式

    QString filePath = QFileDialog::getSaveFileName(
        this, "保存图像",
            fileName,//初始目录
         "PNG图像(*.png);;所有文件(*)");
    if (!filePath.isEmpty())
    {
        pixmap->save(filePath,"PNG"); //保存成PNG图片
        slotOkbtnClick();
    }
}

void ScreenShot::slotCloseBtnClick()
{
    finishShot();
}

void ScreenShot::slotOkbtnClick()
{
    hide();

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap->save(&buffer,"PNG");
    buffer.close();
    qint64 datasize = bytes.size();
    bytes.clear();

    QDateTime dateTime = QDateTime::currentDateTime();//获取系统现在的时间
    QString fileName = "屏幕截图" + dateTime.toString("yyyyMMddhhmmss") + ".png\n"; //设置显示格式
    QString str = dateTime.toString("日期: yyyy年MM月dd日(ddd)\n时间: hh:mm:ss"); //设置显示格式
    str = fileName + str + "\n起点: " + QString("Point(%1,%2)").arg(rect.x()).arg(rect.y());
    str = str + QString("\n尺寸: %1 x %2").arg(rect.width()).arg(rect.height());
    str = str + QString("\n大小: %1 KB").arg(datasize/1024.0);

    emit Finished(*pixmap,str);
    finishShot();
}

QRect ScreenShot::getCursorWindowInfo()
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height= 0;

    POINT curPoint; //鼠标坐标
    GetCursorPos(&curPoint); //获取鼠标的坐标
    setWindowOpacity(0);
    HWND mainWindow = WindowFromPoint(curPoint); //获取鼠标所在点的窗口句柄
    setWindowOpacity(1);

    RECT mainWindowRect; //主窗口的位置信息
    //mainWindowRect.left=mainWindowRect.right=mainWindowRect.bottom=mainWindowRect.top = 0;
    if (GetWindowRect(mainWindow,&mainWindowRect))
    {
        /***主窗口的客户区***/
        RECT mainwindowClientRect;  //窗口的客户区(即是不包含标题栏和状态栏的部分)
        GetClientRect(mainWindow,&mainwindowClientRect);

        /***主窗口客服区的起点***/
        POINT mcPoint;
        mcPoint.x = 0; //这里的初始化不能能少,否则会出错
        mcPoint.y = 0;
        ClientToScreen(mainWindow,&mcPoint);

        x = mcPoint.x;
        y = mcPoint.y;
        width = mainwindowClientRect.right - mainwindowClientRect.left;
        height= mainwindowClientRect.bottom - mainwindowClientRect.top;
    }
    if (curPoint.x<x||curPoint.x>(x+width)||curPoint.y<y||curPoint.y>(y+height))
    {
        x = mainWindowRect.left;
        y = mainWindowRect.top;
        width = mainWindowRect.right - mainWindowRect.left;
        height= mainWindowRect.bottom- mainWindowRect.top;
    }
    if (x < 0) //区域超出左边
    {
        width = width + x;
        x = 0;
    }
    if (y < 0) //区域超出上边
    {
        height = height + y;
        y = 0;
    }
    if ((x + width) > fullScreenPixmap->width()) //区域超出右边
    {
        width = fullScreenPixmap->width() - x;
    }
    if ((y + height) > fullScreenPixmap->height()) //区域超出下边
    {
        height = fullScreenPixmap->height() - y;
    }

    QRect rect;
    rect.setX(x);
    rect.setY(y);
    rect.setWidth(width);
    rect.setHeight(height);

    return rect;
}

void ScreenShot::slotUpdateCusorWindowInfo()
{
    if (State != READY) return;
    if (oldCursorPos != QCursor::pos())
    {
        oldCursorPos = QCursor::pos();
        QRect re = getCursorWindowInfo();
        if(re != autoRect)
        {//此次的区域与原来的不一样才进行更新
            autoRect = re;
            updateAutoPix();
        }
    }
}
