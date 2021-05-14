import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

Page {
    property alias chatRepeater: chatRepeater
    property alias flickChat: flickChat

    Pane {
        anchors.fill: parent
        anchors.margins: 20
        Material.elevation: 6
        antialiasing: true

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
                //contentItem.opacity: 1
                height: flickChat.height
                width: 30
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

