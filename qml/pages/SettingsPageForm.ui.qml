import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import "../controls"

Page {
    id: page
    property alias cfgSocketHost: cfgSocketHost
    property alias cfgSocketPort: cfgSocketPort
    property alias cfgSocketTimestampDiff: cfgSocketTimestampDiff
    property alias cfgSpeechkitFolderId: cfgSpeechkitFolderId
    property alias cfgSpeechkitOAuthToken: cfgSpeechkitOAuthToken
    property alias focusEnder: focusEnder
    property alias cfgSpeechkitVoice: cfgSpeechkitVoice
    property alias panelSpeechkitEmotion: panelSpeechkitEmotion
    property alias cfgSpeechkitEmotion: cfgSpeechkitEmotion
    property alias btShowOAuthToken: btShowOAuthToken
    property alias cfgSpeechkitFormat: cfgSpeechkitFormat
    property alias cfgSpeechkitSampleRateHertz: cfgSpeechkitSampleRateHertz
    property alias panelSpeechkitSampleRateHertz: panelSpeechkitSampleRateHertz
    property alias cfgSpeechkitSpeed: cfgSpeechkitSpeed
    property alias btTestVoice: btTestVoice

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
                    Layout.topMargin: 15
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
                                echoMode: TextInput.Password
                            }
                            Button {
                                id: btShowOAuthToken
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
                RowLayout {
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Label {
                            text: qsTr("Голос (по умолчанию)")
                        }
                        ComboBox {
                            id: cfgSpeechkitVoice
                            width: 400
                            Layout.minimumWidth: width
                        }
                    }
                    ColumnLayout {
                        id: panelSpeechkitEmotion
                        Layout.fillWidth: true
                        spacing: 5
                        visible: false
                        Label {
                            text: qsTr("Эмоция (по умолчанию)")
                        }
                        ComboBox {
                            id: cfgSpeechkitEmotion
                            width: 400
                            Layout.minimumWidth: width
                        }
                    }
                    Button {
                        id: btTestVoice
                        text: qsTr("Тест")
                        Layout.alignment: Qt.AlignBottom
                    }
                }
                RowLayout {
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Label {
                            text: qsTr("Формат синтезируемого аудио")
                        }
                        ComboBox {
                            id: cfgSpeechkitFormat
                            width: 400
                            Layout.minimumWidth: width
                        }
                    }
                    ColumnLayout {
                        id: panelSpeechkitSampleRateHertz
                        Layout.fillWidth: true
                        spacing: 5
                        Label {
                            text: qsTr("Частота дискретизации")
                        }
                        ComboBox {
                            id: cfgSpeechkitSampleRateHertz
                            width: 400
                            Layout.minimumWidth: width
                        }
                    }
                }
                RowLayout {
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Label {
                            text: qsTr("Темп")
                        }
                        NumBox {
                            id: cfgSpeechkitSpeed
                            width: 150
                            Layout.minimumWidth: width
                            Layout.minimumHeight: height
                        }
                    }
                }
            }
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:600;width:800}
}
##^##*/

