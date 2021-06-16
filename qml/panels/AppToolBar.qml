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
    }
}
