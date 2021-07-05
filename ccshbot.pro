#-------------------------------------------------
#   CrazyCash Bot
#-------------------------------------------------
TEMPLATE = app
TARGET  = ccshbot
VERSION = 0.5.6-0

QT += core gui
QT += qml
QT += quick
QT += quickcontrols2
QT += network
QT += websockets
QT += sql
QT += multimedia
QT += svg

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
        ccbot_private.cpp \
        logmaker.cpp \
        main.cpp

HEADERS += \
    ccbot.h \
    ccbot_private.h \
    core.h \
    enums.h \
    logmaker.h \
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
    app.ico \
    app.svg \
    qml/controls/NumBox.qml \
    qml/controls/NumBoxForm.ui.qml \
    qml/dialogs/About.qml \
    qml/dialogs/Message.qml \
    qml/main.qml \
    qml/pages/HistoryPage.qml \
    qml/pages/HistoryPageForm.ui.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/SettingsPageForm.ui.qml \
    qml/pages/SettingsViewPage.qml \
    qml/pages/SettingsViewPageForm.ui.qml \
    qml/pages/ViewChat.qml \
    qml/pages/ViewChatForm.ui.qml \
    qml/panels/AppLeftMenu.qml \
    qml/panels/AppStatusBar.qml \
    qml/panels/AppToolBar.qml \
    qtquickcontrols2.conf

win32: RC_ICONS += app.ico

## параметры линковщика
unix:!android: {
    QMAKE_LFLAGS += -no-pie
    QMAKE_CXXFLAGS += "-fno-sized-deallocation"
    QMAKE_CXXFLAGS += -std=c++17
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
#QML_DESIGNER_IMPORT_PATH = #$$PWD/qml

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


