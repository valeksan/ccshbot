import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import "../controls"

Page {
    id: page
    property alias fontSelectSize: fontSelectSize
    property alias fontSelector: fontSelector
    property alias fontSelectTest: fontSelectTest

    Pane {
        anchors.fill: parent
        anchors.margins: 20
        antialiasing: true

        Material.elevation: 6

        Flickable {
            id: flickSettings

            clip: true
            contentWidth: contentSettings.width
            contentHeight: contentSettings.height

            anchors.fill: parent
            anchors.margins: 15

            MouseArea {
                id: focusEnder
                anchors.fill: parent
                propagateComposedEvents: true
            }

            ColumnLayout {
                id: contentSettings
                width: flickSettings.width
                spacing: 15
                Label {
                    text: qsTr("Настройка отображения чата")
                    Layout.fillWidth: true
                    horizontalAlignment: "AlignLeft"
                    font.bold: true
                    font.pixelSize: 20
                    Material.foreground: Material.Yellow
                }
                RowLayout {
                    spacing: 20
                    Layout.fillWidth: true
                    ColumnLayout {
                        spacing: 5
                        Label {
                            text: qsTr("Название шрифта")
                        }
                        ComboBox {
                            id: fontSelector
                            width: 300
                            Layout.minimumWidth: width
                            Layout.minimumHeight: 46
                        }
                    }
                    ColumnLayout {
                        spacing: 5
                        Label {
                            text: qsTr("Размер")
                        }
                        NumBox {
                            id: fontSelectSize
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 46
                        }
                    }
                }
                GroupBox {
                    title: qsTr("Отображение")
                    Layout.minimumWidth: 300
                    Label {
                        id: fontSelectTest
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;height:600;width:800}
}
##^##*/

