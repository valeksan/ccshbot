import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Drawer {
    id: panel

    Column {
        anchors.fill: parent

        ItemDelegate {
            text: qsTr("Вид")
            width: parent.width
            onClicked: {
                stackView.push("qrc:///qml/pages/SettingsViewPage.qml");
                drawer.close();
            }
        }

        ItemDelegate {
            text: qsTr("Настройки")
            width: parent.width
            onClicked: {
                stackView.push("qrc:///qml/pages/SettingsPage.qml");
                drawer.close();
            }
        }

        MenuSeparator {
            width: parent.width
        }

        ItemDelegate {
            id: menuItemOpenLogDir
            visible: Qt.application.arguments.includes('--log') || Qt.application.arguments.includes('-l')
            text: qsTr("Логи")
            width: parent.width
            onClicked: {
                ccbot.openLogDir();
            }
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
