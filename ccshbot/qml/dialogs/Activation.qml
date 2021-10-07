import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Dialog {
    id: dialog
    title: qsTr("Activation")
    modal: true
    width: 840

    Column {
        Label {
            text: qsTr("Registration key")
            color: "yellow"
        }
        Row {
            spacing: 10
            TextArea {
                id: regKeyHw
                //textFormat: Label.RichText
                width: dialog.width - 160
                wrapMode: "WrapAnywhere"
                readOnly: true
                selectByMouse: true
                text: ccbot.getRegistrationCode()
            }
            Button {
                width: 100
                text: qsTr("Copy")
                onClicked: {
                    regKeyHw.selectAll()
                    regKeyHw.copy();
                    regKeyHw.deselect();
                }
            }
        }
        Label {
            text: qsTr("Activation key")
            color: "yellow"
        }
        Row {
            spacing: 10
            TextArea {
                id: actKeyNew
                //textFormat: Label.RichText
                width: dialog.width - 160
                wrapMode: "WrapAnywhere"
                selectByMouse: true
                text: properties.actKey
                placeholderText: "XXXXXXXX-XXXXXXX-XXXXXXX-XXXXXXX-XXXXXXX-XXXXXXX-XXXXXXX-XXXXXXX-XXXXXXX"
            }
            Button {
                width: 100
                text: qsTr("Paste")
                onClicked: {
                    actKeyNew.text = clipboard.pasteFromClipboard();
                }
            }
        }
        TextEdit {
            id: clipboard
            visible: false
            function pasteFromClipboard() {
                clipboard.selectAll();
                clipboard.paste();
                return clipboard.text;
            }
        }
    }

    onAccepted: {
        properties.actKey = actKeyNew.text.replace(/\s/g, '');
    }

    standardButtons: Dialog.Ok
}
