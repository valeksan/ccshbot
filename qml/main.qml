import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import Qt.labs.settings 1.1

import "js"
import "pages"

ApplicationWindow {
    id: window

    x: settings.x
    y: settings.y
    height: settings.height
    width: settings.width
    minimumWidth: 600
    minimumHeight: 400

    color: "#1f1f2a"

    visibility: "Windowed"

    visible: true

    header: ToolBar {
        contentHeight: toolButton.implicitHeight
        background: Rectangle {
            color: "#121217"
        }

        ToolButton {
            id: toolButton
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
            anchors.centerIn: parent
        }
    }

    Drawer {
        id: drawer
        width: window.minimumWidth
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Auth process test view ...")
                width: parent.width
                onClicked: {
                    stackView.push("qrc:///qml/pages/AuthWaitForm.ui.qml")
                    drawer.close()
                }
            }
            MenuSeparator {
                width: parent.width
            }
            ItemDelegate {
                text: qsTr("Выход")
                width: parent.width
                onClicked: {
                    drawer.close()
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
    }

    // объект для хранения настроек программы, храним размер приложения и его положения на экране
    Settings {
        id: settings
        property int height: 900
        property int width: 1000
        property int x: desktopMethods.getDescktopX()
        property int y: desktopMethods.getDescktopY()
        property bool authSave: true
        property string email: ""
        property string password: ""
    }

    StackView {
        id: stackView
        initialItem: "qrc:///qml/pages/Auth.qml"
        anchors.fill: parent
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
}
