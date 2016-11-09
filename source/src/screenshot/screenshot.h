#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "selectrect.h"
#include <windows.h>
/***  截图时显示的控件,主要包含三部分:
 ***1.显示全屏截图图像的label
 ***2.显示阴影部分的label
 ***3.显示高亮图像的label,(包含了:label、8个点、以及操作按钮)
 ***
 ***/

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")

class ScreenShot : public QLabel
{
    Q_OBJECT
public:
    explicit ScreenShot(QWidget *parent = 0);
    ~ScreenShot();
    enum ShotState //截图状态
    {
        READY    = 0, //准备开始截图
        SHOTTING = 1, //正在截图
        FINISHED = 2  //截图完毕
    };
    void getReadyToShot();//设置好所有的label准备截图
    ShotState state(){return State;}

protected:
    void mousePressEvent(QMouseEvent * event);
signals:
    void Finished(QPixmap,QString);
private:
//    Ui::ToolWidget *ui;

    QPoint oldCursorPos;
    QPoint dragPosition;
    ShotState State; //状态

    /***自动选择的区域***/
    QRect autoRect;
    QTimer *timer;       //定时更新当前鼠标所在点的窗口的信息

    /*显示图像*/
    QPixmap *fullScreenPixmap; //截取全屏的图像
    QPixmap *pixmap; //截取得到的图像(高亮显示的图像)
    QLabel *pixLabel; //显示高亮图像的label
    QLabel *label;  //显示出阴影效果的label
    QWidget *toolWidget; //选中完毕后弹出的操作按钮

    /*选择范围*/
    SelectRect *selectRect;
    QRect rect;

    void initToolWidget();
    void showToolWidget();

    void updateAutoPix(); //更新显示自动选择的区域
    void updatePixmap();  //更新高亮显示的图像
    void finishShot();    //截图完毕
    QRect getCursorWindowInfo();//获取鼠标所在位置的窗口的信息(使用windowsAPI)

private slots:
    void slotAutoPixSelect();
    void slotRectChanged(QRect);

    void slotSaveBtnClick(); //保存截图
    void slotCloseBtnClick();
    void slotOkbtnClick();

    void slotUpdateCusorWindowInfo(); //定时更新当前鼠标所在点的窗口的信息
};

#endif // SCREENSHOT_H
