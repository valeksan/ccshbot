import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.12

Page {
    property alias cfgSocketHost: cfgSocketHost
    property alias cfgSocketPort: cfgSocketPort
    Pane {
        anchors.fill: parent
        anchors.margins: 20
        Material.elevation: 6
        antialiasing: true

        ColumnLayout {
            anchors.fill: parent
            GroupBox {
                title: qsTr("Сеть")
                RowLayout {
                    TextField {
                        id: cfgSocketHost
                        width: 150
                        //text: settings.listenHost
                    }
                    SpinBox {
                        id: cfgSocketPort
                        from: 0
                        to: 65535
                        //value: settings.listenHost
                    }
                }
            }
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

