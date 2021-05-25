import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ToolBar {
    id: panel

    property alias appState: appState

    RowLayout {
        anchors.fill: parent

        Label {
            id: appState
            text: ""
            verticalAlignment: "AlignVCenter"
            horizontalAlignment: "AlignHCenter"
            Layout.fillWidth: true
            Layout.margins: 10
        }
    }
}
