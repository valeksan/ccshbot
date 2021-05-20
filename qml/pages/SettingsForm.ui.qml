import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.12

Page {

    property alias cfgSocketHost: cfgSocketHost
    property alias cfgSocketPort: cfgSocketPort
    property alias cfgSocketTimestampDiff: cfgSocketTimestampDiff
    property alias cfgSpeechkitFolderId: cfgSpeechkitFolderId
    property alias cfgSpeechkitOAuthToken: cfgSpeechkitOAuthToken

    Pane {
        anchors.fill: parent
        anchors.margins: 20
        Material.elevation: 6
        antialiasing: true

        Flickable {
            id: flickSettings
            clip: true
            anchors.fill: parent
            anchors.margins: 15
            contentWidth: contentSettings.width
            contentHeight: contentSettings.height
            ColumnLayout {
                id: contentSettings
                width: flickSettings.width
                spacing: 15
                Label {
                    text: qsTr("Настройка сервера")
                    Layout.fillWidth: true
                    horizontalAlignment: "AlignLeft"
                    font.bold: true
                    font.pixelSize: 20
                    Material.foreground: Material.Yellow
                }
                RowLayout {
                    spacing: 20
                    Layout.fillWidth: true
                    RowLayout {
                        spacing: 10
                        Label {
                            text: qsTr("Хост:")
                            font.pixelSize: cfgSocketHost.font.pixelSize
                        }
                        TextField {
                            id: cfgSocketHost
                            width: 150
                        }
                    }
                    RowLayout {
                        spacing: 10
                        Label {
                            text: qsTr("Порт:")
                            font.pixelSize: cfgSocketPort.font.pixelSize
                        }
                        TextField {
                            id: cfgSocketPort
                            width: 80
                            Layout.maximumWidth: width
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Label {
                        text: qsTr("Времнное запаздывание пакета не польше (сек):")
                        font.pixelSize: cfgSocketTimestampDiff.font.pixelSize
                    }
                    SpinBox {
                        id: cfgSocketTimestampDiff
                        width: 150
                        Layout.maximumWidth: width
                        from: 0
                        to: 2147483647
                    }
                }
                Label {
                    text: qsTr("Настройка TTS: Yandex.Cloud SpeechKit")
                    Layout.fillWidth: true
                    horizontalAlignment: "AlignLeft"
                    font.bold: true
                    font.pixelSize: 20
                    Material.foreground: Material.Yellow
                }
                GroupBox {
                    title: qsTr("Данные для авторизации на сервисе")
                    ColumnLayout {
                        RowLayout {
                            spacing: 10
                            Label {
                                text: qsTr("OAuth Token:")
                                width: 100
                                Layout.minimumWidth: width
                            }
                            TextField {
                                id: cfgSpeechkitOAuthToken
                                width: 400
                                Layout.minimumWidth: width
                            }
                        }
                        RowLayout {
                            spacing: 10
                            Label {
                                text: qsTr("Folder ID:")
                                width: 100
                                Layout.minimumWidth: width
                            }
                            TextField {
                                id: cfgSpeechkitFolderId
                                width: 400
                                Layout.minimumWidth: width
                            }
                        }
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

