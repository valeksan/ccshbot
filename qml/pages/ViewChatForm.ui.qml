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
    property alias inputMsg: inputMsg
    property alias btSendMsg: btSendMsg
    property alias btVoiceBalanceSpending: btVoiceBalanceSpending
    property alias rowLayout: rowLayout
    property alias btTest: btTest
    property alias chatRepeaterMA: chatRepeaterMA
    property alias contexMenuLoader1: contexMenuLoader1
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
            id: rowLayout
            anchors.fill: parent
            ToolButton {
                id: toolButtonStartServer
                Material.foreground: properties.listenClients ? "red" : "lightgreen"
                Layout.leftMargin: 10
            }
            ToolButton {
                id: btTest
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
                    }
                    RadioButton {
                        id: btVoiceType2
                        text: qsTr("Донаты")
                    }
                    RadioButton {
                        id: btVoiceBalanceSpending
                        text: qsTr("Баланс")
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
            anchors.bottomMargin: 64
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
                MouseArea {
                    id: chatRepeaterMA
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    hoverEnabled: true
                    Loader {
                        id: contexMenuLoader1
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                anchors.right: flickChat.right
                height: flickChat.height
                width: 30
            }
        }
        TextField {
            id: inputMsg
            anchors {
                left: parent.left
                right: parent.right
                top: flickChat.bottom
                bottom: parent.bottom
                rightMargin: 120
                topMargin: 7
                bottomMargin: 7
                leftMargin: 10
            }
            background: Rectangle {
                border.color: "gray"
                radius: 5
                color: "#121217"
            }
            leftInset: -10
        }
        Button {
            id: btSendMsg
            text: qsTr("Отправить")
            anchors {
                left: inputMsg.right
                right: parent.right
                top: flickChat.bottom
                bottom: parent.bottom
                rightMargin: 10
                topMargin: 2
                bottomMargin: 2
                leftMargin: 10
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:600;width:800}
}
##^##*/

