import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Page {
    title: qsTr("Авторизация")

    Column {
        height: 250
        width: 680
        anchors.centerIn: parent
        spacing: 15
        BusyIndicator {
            width: 60
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Label {
            text: "Подождите, идет процесс аунтификации ..."
            color: "#fdef00"
            font.pixelSize: 28
            horizontalAlignment: "AlignHCenter"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
