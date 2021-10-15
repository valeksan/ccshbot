import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Drawer {
    id: panel

    Column {
        anchors.fill: parent

        ItemDelegate {
            text: qsTr("Hide menu")
            width: parent.width
            onClicked: {
                drawer.close();
            }
        }

        MenuSeparator {
            width: parent.width
        }

        ItemDelegate {
            text: qsTr("History")
            width: parent.width
            onClicked: {
                stackView.push(Qt.resolvedUrl("qrc:///qml/pages/HistoryPage.qml"));
                drawer.close();
            }
        }

        MenuSeparator {
            width: parent.width
        }

        ItemDelegate {
            text: qsTr("View")
            width: parent.width
            onClicked: {
                stackView.push(Qt.resolvedUrl("qrc:///qml/pages/SettingsViewPage.qml"));
                drawer.close();
            }
        }

        ItemDelegate {
            text: qsTr("Settings")
            width: parent.width
            onClicked: {
                stackView.push(Qt.resolvedUrl("qrc:///qml/pages/SettingsPage.qml"));
                drawer.close();
            }
        }

        MenuSeparator {
            width: parent.width
        }

        ItemDelegate {
            id: menuItemOpenLogDir
            visible: Qt.application.arguments.includes('--log') || Qt.application.arguments.includes('-l')
            text: qsTr("Logs")
            width: parent.width
            onClicked: {
                ccbot.openLogDir();
            }
        }

        ItemDelegate {
            text: qsTr("About")
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
            text: qsTr("Exit")
            width: parent.width
            onClicked: {
                drawer.close();
                window.close();
            }
        }
    }
}
