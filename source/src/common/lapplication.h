#ifndef LAPPLICATION_H
#define LAPPLICATION_H

#include <QApplication>

class LApplication :public QApplication
{
public:
     LApplication(int&argc,char **argv);
     ~LApplication();

     bool notify(QObject*, QEvent *);
     void setWindowInstance(QWidget*wnd);
private:
     QWidget *widget;
};

#endif // LAPPLICATION_H
