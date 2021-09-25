import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Page {
    property alias selectNikname: selectNikname
    property alias selectStreamId: selectStreamId
    property alias flickChatH: flickChatH
    property alias chatRepeaterH: chatRepeaterH
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
            anchors {
                fill: parent
                leftMargin: 5
                rightMargin: 5
            }

            spacing: 5
            ComboBox {
                id: selectNikname
                displayText: qsTr("Streamer") + ": " + currentText
                Layout.fillWidth: true
            }
            ComboBox {
                id: selectStreamId
                displayText: qsTr("Stream number") + ": " + currentText
                Layout.fillWidth: true
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
            id: flickChatH
            clip: true
            anchors.fill: parent
            contentWidth: parent.width
            contentHeight: chatRepeaterH.contentHeight
            boundsBehavior: Flickable.StopAtBounds

            TextEdit {
                id: chatRepeaterH
                width: flickChatH.width
                cursorVisible: false
                wrapMode: TextArea.WrapAnywhere
                textMargin: 5
                selectByMouse: true
                readOnly: true
                textFormat: TextEdit.RichText
            }

            ScrollBar.vertical: ScrollBar {
                anchors.right: flickChatH.right
                height: flickChatH.height
                width: 30
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;height:600;width:800}
}
##^##*/

