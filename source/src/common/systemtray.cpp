
#include "systemtray.h"
#include "ryimpl.h"
#include "Utility.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QMenu>
#include <QDebug>
# pragma execution_character_set("utf-8")

//! [0]
SystemTray::SystemTray()
{
    timer = new QTimer(this);
    timer->setInterval(500);
    m_idx = 0;
    connect(timer,SIGNAL(timeout()),this,SLOT(changeIcon()));

    createActions();
    createTrayIcon();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &SystemTray::iconActivated);
    connect(RYImpl::getInstance(), &RYImpl::recccvMsg, this, &SystemTray::rcvMsg);
    trayIcon->show();
}
//! [0]

//! [1]
void SystemTray::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
}
//! [1]


//! [3]
void SystemTray::setIcon(int index)
{
    if(index==0)
        trayIcon->setIcon(QIcon(":/images/qt-logo.png"));
    else
        trayIcon->setIcon(QIcon(""));
}
//! [3]
void SystemTray::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
    trayIcon->showMessage("消息提示", "你有新消息", icon, 2 * 1000);
}

void SystemTray::stopFlash()
{
    timer->stop();
    trayIcon->setIcon(QIcon(":/images/qt-logo.png"));
}
//! [4]
void SystemTray::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        showApp();
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}
//! [4]



void SystemTray::changeIcon()
{
    if(m_idx==1)
        m_idx = 0;
    else
       m_idx = 1;
    setIcon(m_idx);
}

void SystemTray::quitApp()
{
    timer->stop();
    trayIcon->setIcon(QIcon(":/images/qt-logo.png"));
    RYImpl::getInstance()->disconnect();
    QCoreApplication::quit();
}

void SystemTray::showApp()
{
    if(timer->isActive())
        emit Utility::getInstance()->sigshow(true);
    else
        emit Utility::getInstance()->sigshow(false);
    timer->stop();
    trayIcon->setIcon(QIcon(":/images/qt-logo.png"));
}

void SystemTray::rcvMsg(const QString &userid)
{
    qDebug()<<"change icon...";
    timer->start();
}
//! [6]


void SystemTray::createActions()
{
    minimizeAction = new QAction(QString::fromUtf8("显示"), this);
    connect(minimizeAction, &QAction::triggered, this, &SystemTray::showApp);
    quitAction = new QAction(QString::fromUtf8("退出"), this);
    connect(quitAction, &QAction::triggered, this, &SystemTray::quitApp);
}

void SystemTray::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/qt-logo.png"));
}

#endif
