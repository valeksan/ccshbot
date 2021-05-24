#-------------------------------------------------
#   CrazyCash Bot
#-------------------------------------------------
TEMPLATE = app
TARGET  = ccshbot
VERSION = 0.5.0-0

QT += core gui
QT += qml
QT += quick
QT += quickcontrols2
QT += network
QT += websockets
QT += sql
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

equals(QT_VERSION, 6) {
   QT += core5compat
}

CONFIG += c++17
#QMAKE_CXXFLAGS += -std=c++17
#CONFIG += use_gold_linker

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += 'VERSION_STRING=\\\"$$VERSION\\\"'

SOURCES += \
        ccbot.cpp \
        main.cpp

HEADERS += \
    ccbot.h \
    ccbotengine.h \
    core.h \
    desktop.h \
    enums.h \
    messagedata.h \
    metadata.h \
    misc.h \
    properties.h

RESOURCES += \
    qml.qrc

TRANSLATIONS += \
    ccshbot_en_US.ts

DISTFILES += \
    LICENSE \
    README.md \
    qml/controls/NumBox.qml \
    qml/controls/NumBoxForm.ui.qml \
    qml/dialogs/About.qml \
    qml/dialogs/Message.qml \
    qml/js/MethodsDekctopJS.qml \
    qml/main.qml \
    qml/pages/Chat.qml \
    qml/pages/ChatForm.ui.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/SettingsPageForm.ui.qml \
    qtquickcontrols2.conf

## параметры линковщика
unix:!android: {
    QMAKE_LFLAGS += -no-pie
    QMAKE_CXXFLAGS += "-fno-sized-deallocation"
    QMAKE_CXXFLAGS += -std=c++17
#    static {
#        QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
#        QMAKE_LFLAGS += -Wl,--disable-new-dtags
#    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


