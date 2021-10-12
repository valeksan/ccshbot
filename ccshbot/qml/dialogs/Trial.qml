import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.11

Window {
    id: dialog
    title: qsTr("Waiting before launch (trial)")
    width: 500
    height: 400

    flags: Qt.Tool
    x: window.x + (window.width - dialog.width)/2
    y: window.y + (window.height - dialog.height)/2

    color: window.color

    property int showSec

    signal showMainWindow();

    function decrementShowSec() {
        --showSec;
        if (showSec != 0) {
            window.setTimeout(() => { decrementShowSec() }, 1000);
        } else {
            showMainWindow();
            dialog.close();
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: parent.width - 40
        height: parent.height - 30
        border.width: 3
        border.color: "yellow"
        color: "transparent"
        Label {
            anchors.fill: parent
            verticalAlignment: "AlignVCenter"
            horizontalAlignment: "AlignHCenter"
            color: "yellow"
            text: dialog.showSec
            font.pixelSize: 50
        }
    }

    onVisibleChanged: {
        dialog.showSec = 5;
        window.setTimeout(() => {
                              try {
                                decrementShowSec();
                              } catch(e){}
                          }, 1000);
    }
}
