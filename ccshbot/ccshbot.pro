#-------------------------------------------------
#   CrazyCash Bot
#-------------------------------------------------
# Needs for work:
# OpenSSL: https://slproweb.com/products/Win32OpenSSL.html (win)
# Audio codecs: https://www.codecguide.com/download_k-lite_codec_pack_standard.htm (win)

TEMPLATE = app
TARGET  = ccshbot
VERSION = 0.6.0
ORGANIZATION = valeksan-soft

QT += core gui          # + LGPLv3
QT += qml               # + LGPLv3
QT += quick             # + LGPLv3
QT += quickcontrols2    # + LGPLv3
QT += network           # + LGPLv3
QT += websockets        # + LGPLv3
QT += sql               # + LGPLv3
QT += multimedia        # + LGPLv3
QT += svg               # + LGPLv3
QT += texttospeech      # + LGPLv3

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

equals(QT_VERSION, 6) {
   QT += core5compat
}

CONFIG += c++17

SOURCES += \
    ccbot.cpp \
    ccbot_private.cpp \
    console.cpp \
    logmaker.cpp \
    main.cpp \
    speechkit_tts.cpp \
    ttsmanager.cpp

HEADERS += \
    ccbot.h \
    ccbot_private.h \
    console.h \
    core.h \
    enums.h \
    logmaker.h \
    messagedata.h \
    misc.h \
    properties.h \
    speechkit_tts.h \
    ttsmanager.h

RESOURCES += \
    qml.qrc

DISTFILES += \
    qml/controls/NumBox.qml \
    qml/controls/NumBoxForm.ui.qml \
    qml/dialogs/About.qml \
    qml/dialogs/Activation.qml \
    qml/dialogs/Message.qml \
    qml/dialogs/Trial.qml \
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

# Defines init
#DEFINES += ENABLE_USE_ENUM_NAMESPACES
DEFINES += 'APP_VERSION=\\\"$$VERSION\\\"'
DEFINES += 'APP_NAME=\\\"$$TARGET\\\"'
DEFINES += 'ORGANIZATION=\\\"$$ORGANIZATION\\\"'
DEFINES += DISABLE_CHECK_LICENSE_KEY

# Linking init
win32 {
    RC_ICONS = app.ico 
}
win32:mingw:!contains(DEFINES, DISABLE_CHECK_LICENSE_KEY) {
    LIBS += -L"$$PWD/../libs/" -lhid
    message("MINGW_INIT")
}
win32:msvc*:!contains(DEFINES, DISABLE_CHECK_LICENSE_KEY) {
    LIBS += -L"$$PWD/../libs/" -llibhid
    message("MSVC_INIT")
}
linux {
    QMAKE_LFLAGS += -no-pie
    QMAKE_CXXFLAGS += "-fno-sized-deallocation"
}
linux:!contains(DEFINES, DISABLE_CHECK_LICENSE_KEY) {
    LIBS += -L"$$PWD/../libs/" -llibhid
}
macx {
    #...
}

!contains(DEFINES, DISABLE_CHECK_LICENSE_KEY) {
    include($$PWD/../Qt-Secret/src/Qt-Secret.pri)
    INCLUDEPATH += $$PWD/../libhid
    DEPENDPATH += $$PWD/../libhid
    SOURCES += cicero.cpp
    HEADERS += cicero.h
}

# Additional import path used to resolve QML modules in Qt Creator's code model
#QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
#QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
