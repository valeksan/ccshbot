import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Page {
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
    }
}

/*##^##
Designer {
    D{i:0;height:600;width:800}
}
##^##*/

