import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Dialogs 1.2

import Qt.labs.settings 1.1

import "js"
import "pages"
import "dialogs"

ApplicationWindow {
    id: window

    property bool listenClients: false

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

    x: settings.x
    y: settings.y
    height: settings.height
    width: settings.width
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

//        TextField {
//            width: 150
//            text: settings.listenHost
//            validator: RegularExpressionValidator { regularExpression:/^(([0-9]{1,3}.){3}([0-9]{1,3})|localhost):\d+$/ }
//        }
//        SpinBox {
//            from: 1
//            to: 65535
//        }

            Label {
                text: stackView.currentItem.title
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                horizontalAlignment: "AlignHCenter"
            }

            ToolButton {
                id: toolButtonStartServer
                text: listenClients ? '\u23F9' : '\u23F5'
                font.pixelSize: Qt.application.font.pixelSize * 1.6
                width: 48
                Layout.alignment: Qt.AlignRight
                Material.foreground: listenClients ? "red" : "lightgreen"
                onClicked: {
                    if(!window.listenClients) {
                        window.changeStatus("Запуск сервера ...", 1500, "yellow");
                    } else {
                        window.changeStatus("Остановка сервера ...", 1500, "yellow");
                        properties.flagLoadingChat = true;
                    }
                    window.listenClients = !window.listenClients;
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
        property int height: 900
        property int width: 1000
        property int x: desktopMethods.getDescktopX()
        property int y: desktopMethods.getDescktopY()
        property string listenHost: 'localhost'
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

    onHeightChanged: {
        settings.height = height;
    }
    onWidthChanged: {
        settings.width = width;
    }
    onXChanged: {
        settings.x = x;
    }
    onYChanged: {
        settings.y = y;
    }

    Component.onDestruction: {
        //console.log("test_1")
    }
}
