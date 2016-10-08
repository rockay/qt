TEMPLATE = app

QT += qml quick quickcontrols2
!no_desktop: QT += widgets
CONFIG += c++11

SOURCES += main.cpp \
    documenthandler.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

RC_FILE = icon.rc

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    documenthandler.h \
    qtquickcontrolsapplication.h

