TEMPLATE = app

QT += qml quick quickcontrols2 network sql multimedia multimediawidgets gui
!contains(sql-drivers, sqlite): QTPLUGIN += qsqlite
!no_desktop: QT += widgets
CONFIG += c++11 localize_deployment
INCLUDEPATH += ./src ./src/sql ./src/common ./src/screenshot ./src/MyGlobalShortCut/ ./src/network ./3rd-party/update
#QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'true\'\" #以管理员运行
#QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,\"5.01\" #VS2013 在XP运行

SOURCES += main.cpp \
    ./src/common/documenthandler.cpp \
    ./src/common/ryimpl.cpp \
    ./src/common/Utility.cpp \
    ./src/common/textballoon.cpp \
    ./src/common/systemtray.cpp \
    ./src/sql/sqlconversationmodel.cpp \
    ./src/sql/sqlcontactmodel.cpp \
    ./src/sql/qtdatabase.cpp \
    ./src/sql/sqlfriendmodel.cpp \
    ./src/sql/sqlgroupmodel.cpp \
    ./src/sql/sqlgroupmembermodel.cpp \
    ./src/screenshot/screenshot.cpp \
    ./src/screenshot/selectrect.cpp \
    ./src/screenshot/pushpoint.cpp \
    ./src/MyGlobalShortCut/MyGlobalShortCut.cpp \
    ./src/MyGlobalShortCut/MyWinEventFilter.cpp \
    ./src/network/downloadmanager.cpp \
    3rd-party/update/Downloader.cpp \
    3rd-party/update/Updater.cpp \
    src/sql/tthread.cpp \
    src/sql/tconversationthread.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

RC_FILE = icon.rc

# Default rules for deployment.
include(deployment.pri)
#include ($$PWD/3rd-party/QSimpleUpdater/QSimpleUpdater.pri)

HEADERS += \
    ./src/qtquickcontrolsapplication.h \
    ./src/rcsdk.h \
    ./src/common/documenthandler.h \
    ./src/common/ryimpl.h \
    ./src/common/Utility.h \
    ./src/common/textballoon.h \
    ./src/common/systemtray.h \
    ./src/sql/sqlconversationmodel.h \
    ./src/sql/sqlcontactmodel.h \
    ./src/sql/qtdatabase.h \
    ./src/sql/sqlfriendmodel.h \
    ./src/sql/sqlgroupmodel.h \
    ./src/sql/sqlgroupmembermodel.h \
    ./src/screenshot/screenshot.h \
    ./src/screenshot/selectrect.h \
    ./src/screenshot/global.h \
    ./src/screenshotpushpoint.h \
    ./src/screenshot/pushpoint.h \
    ./src/MyGlobalShortCut/MyGlobalShortCut.h \
    ./src/MyGlobalShortCut/MyWinEventFilter.h \
    ./src/network/downloadmanager.h \
    src/common/lmouselistner.h \
    3rd-party/update/Downloader.h \
    3rd-party/update/Updater.h \
    src/sql/tthread.h \
    src/sql/tconversationthread.h

FORMS += \
    3rd-party/update/Downloader.ui

