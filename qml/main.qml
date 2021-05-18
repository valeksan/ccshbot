import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Dialogs 1.2

import Qt.labs.settings 1.1

import ccbot.tasks 1.0

import "js"
import "pages"
import "dialogs"

ApplicationWindow {
    id: window

    function setTimeout(func, interval, ...params) {
        return setTimeoutComponent.createObject(window, { func, interval, params} );
    }

    Component {
        id: setTimeoutComponent
        Timer {
            property var func
            property var params
            running: true
            repeat: false
            onTriggered: {
                func(...params);
                destroy();
            }
        }
    }

    function changeStatus(text, ms = -1, color = "white") {
        appState.color = color;
        appState.text = text;
        if(ms >= 0)
            setTimeout(() => {
                appState.text = "";
            }, ms);
    }

    x: settings._x
    y: settings._y
    height: settings._height
    width: settings._width
    minimumWidth: 640
    minimumHeight: 620

    color: "#1f1f2a"

    visibility: "Windowed"

    visible: true

    header: ToolBar {
        contentHeight: toolButtonMenu.implicitHeight
        background: Rectangle {
            color: "#121217"
        }

        RowLayout {
            anchors.fill: parent
            ToolButton {
                id: toolButtonMenu
                text: stackView.depth > 1 ? "\u25C0" : "\u2630"
                font.pixelSize: Qt.application.font.pixelSize * 1.6
                onClicked: {
                    if (stackView.depth > 1) {
                        stackView.pop()
                    } else {
                        drawer.open()
                    }
                }
            }

            Label {
                text: stackView.currentItem.title
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                horizontalAlignment: "AlignHCenter"
            }
            GroupBox {
                Layout.fillHeight: true
                Layout.margins: 5
                RowLayout {
                    anchors.verticalCenter: parent.verticalCenter
                    Label {
                        text: qsTr("Озвучка")
                        color: "#fff200"
                        font.bold: true
                    }
                    RadioButton {
                        text: qsTr("Отключена")
                        checked: true
                        onToggled: {
                            properties.flagAnalyseVoiceAllMsgType0 = false;
                            properties.flagAnalyseVoiceAllMsgType2 = false;
                        }
                    }
                    RadioButton {
                        text: qsTr("Донаты")
                        onToggled: {
                            properties.flagAnalyseVoiceAllMsgType0 = false;
                            properties.flagAnalyseVoiceAllMsgType2 = true;
                        }
                    }
                    RadioButton {
                        text: qsTr("Все")
                        onToggled: {
                            properties.flagAnalyseVoiceAllMsgType0 = true;
                            properties.flagAnalyseVoiceAllMsgType2 = true;
                        }
                    }
                }
            }

            ToolButton {
                id: toolButtonStartServer
                text: properties.listenClients ? '\u23F9' : '\u23F5'
                font.pixelSize: Qt.application.font.pixelSize * 1.6
                width: 48
                Layout.alignment: Qt.AlignRight
                Material.foreground: properties.listenClients ? "red" : "lightgreen"
                onClicked: {
                    if(!properties.listenClients) {
                        window.changeStatus("Запуск сервера ...", 1500, "yellow");
                        properties.flagLoadingChat = true;
                    } else {
                        window.changeStatus("Остановка сервера ...", 1500, "yellow");
                    }
                    properties.listenClients = !properties.listenClients;
                }
            }

        }
    }

    Drawer {
        id: drawer
        width: window.minimumWidth / 2
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Test message dialog (normal) ...")
                width: parent.width
                onClicked: {
                    messageDlg.show("Соообщение", "Здесь будет сообщение");
                    drawer.close();
                }
            }
            ItemDelegate {
                text: qsTr("Test message dialog (critical) ...")
                width: parent.width
                onClicked: {
                    messageDlg.show("Соообщение", "Здесь будет сообщение", true);
                    drawer.close();
                }
            }
            ItemDelegate {
                text: qsTr("Test get iam-token ...")
                width: parent.width
                onClicked: {
                    ccbot.action(Task.VoiceLoad, ["съешь еще этих мягких булочек"]);
                    drawer.close();
                }
            }

            MenuSeparator {
                width: parent.width
            }

            ItemDelegate {
                text: qsTr("Настройки")
                width: parent.width
                onClicked: {
                    stackView.push("qrc:///qml/pages/Settings.qml");
                    drawer.close();
                }
            }

            MenuSeparator {
                width: parent.width
            }

            ItemDelegate {
                text: qsTr("О программе")
                width: parent.width
                onClicked: {
                    drawer.close();
                    aboutDlg.open();
                }
            }

            MenuSeparator {
                width: parent.width
            }

            ItemDelegate {
                text: qsTr("Выход")
                width: parent.width
                onClicked: {
                    drawer.close();
                    Qt.quit();
                }
            }
        }
    }

    footer: ToolBar {
        height: 60
        background: Rectangle {
            color: "#121217"
        }
        RowLayout {
            anchors.fill: parent

            Label {
                id: appState
                text: ""
                verticalAlignment: "AlignVCenter"
                horizontalAlignment: "AlignHCenter"
                Layout.fillWidth: true
                Layout.margins: 10
            }
        }
    }

    // объект для хранения настроек программы, храним размер приложения и его положения на экране
    Settings {
        id: settings
        property int _height: 900
        property int _width: 1000
        property int _x: desktopMethods.getDescktopX()
        property int _y: desktopMethods.getDescktopY()
        property string listenHost: "127.0.0.1"
        property int listenPort: 3000
        property string chatFont: "Arial"
        property int chatFontPointSize: 12
        property color chatTextColor: "#bfc7d0"
    }

    StackView {
        id: stackView
        initialItem: "qrc:///qml/pages/Chat.qml"
        anchors.fill: parent
    }

    // диалоговые окна
    Message {
        id: messageDlg
        anchors.centerIn: parent
    }
    About {
        id: aboutDlg
        anchors.centerIn: parent
    }

    // вспомогательные JS-методы: для позиционирования окна
    MethodsDekctopJS {
        id: desktopMethods
    }

    Connections {
        target: ccbot
        function onShowMessage(title, text, alert) {
            messageDlg.show(title,text,alert);
        }
    }

    onHeightChanged: {
        settings._height = window.height;
    }
    onWidthChanged: {
        settings._width = window.width;
    }
    onXChanged: {
        settings._x = window.x;
    }
    onYChanged: {
        settings._y = window.y;
    }

    Component.onDestruction: {
        //console.log("test_1")
    }
}
