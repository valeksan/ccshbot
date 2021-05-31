import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ToolBar {
    id: panel

    contentHeight: toolButtonMenu.implicitHeight

    RowLayout {
        anchors.fill: parent

        ToolButton {
            id: toolButtonMenu
            text: stackView.depth > 1 ? "◀" : "☰"
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
            textFormat: Text.RichText
        }

        GroupBox {
            Layout.fillHeight: true
            Layout.margins: 5
            RowLayout {
                anchors.verticalCenter: parent.verticalCenter
                Label {
                    text: qsTr("Озвучка")
                    color: "#FFF200"
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
            text: properties.listenClients ? "<b>⏹</b>" : "<b>⏵</b>"
            font.pixelSize: Qt.application.font.pixelSize * 1.8
            width: 48
            Layout.alignment: Qt.AlignRight
            Material.foreground: properties.listenClients ?
                                     "red" : "lightgreen"
            onClicked: {
                if(!properties.listenClients) {
                    window.changeStatus("Запуск сервера ...", 1500, "yellow");
                    properties.flagLoadingChat = true;
                } else {
                    window.changeStatus(
                                "Остановка сервера ...", 1500, "yellow");
                }
                properties.listenClients = !properties.listenClients;
            }
        }
    }
}
