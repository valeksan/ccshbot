#-------------------------------------------------
#   CrazyCash Bot
#-------------------------------------------------
TEMPLATE = app
TARGET  = ccsh-bot
VERSION = 0.0.0-0

QT += core gui
QT += qml
QT += quick
QT += quickcontrols2
QT += network
QT += webengine
QT += webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

equals(QT_VERSION, 6) {
   QT += core5compat
}

CONFIG += c++17
CONFIG += use_gold_linker

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += 'VERSION_STRING=\\\"$$VERSION\\\"'
DEFINES += DEBUG_SAVE_HTML_TO_TMP

SOURCES += \
        crazycashapi.cpp \
        main.cpp

HEADERS += \
    core.h \
    crazycashapi.h \
    desktop.h \
    metadata.h \
    misc.h

RESOURCES += \
    qml.qrc

TRANSLATIONS += \
    ccshbot_en_US.ts

DISTFILES += \
    README.md

## параметры линковщика
unix:!android: {
    QMAKE_LFLAGS += -no-pie
    QMAKE_CXXFLAGS += "-fno-sized-deallocation"
    QMAKE_CXXFLAGS += -std=c++17
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


