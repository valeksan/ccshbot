import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Page {
    property alias chatRepeater: chatRepeater
    property alias flickChat: flickChat
    property alias btVoiceOff: btVoiceOff
    property alias btVoiceType2: btVoiceType2
    property alias btVoiceAll: btVoiceAll
    property alias toolButtonStartServer: toolButtonStartServer
    antialiasing: true

    ToolBar {
        id: navigateBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 1
        }
        height: 50
        background: Rectangle {
            color: "#121217"
        }

        RowLayout {
            anchors.fill: parent
            ToolButton {
                id: toolButtonStartServer
                Material.foreground: properties.listenClients ? "red" : "lightgreen"
                Layout.leftMargin: 10
            }
            GroupBox {
                Layout.fillHeight: true
                Layout.margins: 5
                Layout.alignment: Qt.AlignRight
                RowLayout {
                    anchors.verticalCenter: parent.verticalCenter
                    Label {
                        text: qsTr("Озвучка")
                        color: "#FFF200"
                        font.bold: true
                    }
                    RadioButton {
                        id: btVoiceOff
                        text: qsTr("Отключена")
                        checked: true
                    }
                    RadioButton {
                        id: btVoiceType2
                        text: qsTr("Донаты")
                    }
                    RadioButton {
                        id: btVoiceAll
                        text: qsTr("Все")
                    }
                }
            }
        }
    }

    Pane {
        anchors {
            top: navigateBar.bottom
            margins: 20
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        Material.elevation: 6

        Flickable {
            id: flickChat
            clip: true
            anchors.fill: parent
            contentWidth: parent.width
            contentHeight: chatRepeater.contentHeight
            boundsBehavior: Flickable.StopAtBounds

            TextEdit {
                id: chatRepeater
                width: flickChat.width
                cursorVisible: false
                wrapMode: TextArea.WrapAnywhere
                textMargin: 5
                selectByMouse: true
                readOnly: true
                textFormat: TextEdit.RichText
            }

            ScrollBar.vertical: ScrollBar {
                anchors.right: flickChat.right
                height: flickChat.height
                width: 30
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:600;width:800}
}
##^##*/

