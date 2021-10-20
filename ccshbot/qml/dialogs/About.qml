import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Dialog {
    id: dialog
    title: qsTr("About")
    modal: true
    width: 400

    property bool activation: properties.isActivated

    Column {
        Label {
            id: codename
            textFormat: Label.RichText
            text: Qt.application.name + " v" + Qt.application.version + "<span style='color:white'>" + " build " + releaseDate + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: qsTr("Activation") + ": " + (dialog.activation ? qsTr("Activated") + ", " + qsTr("expiry in ") + ccbot.getEndActivationDate() : "<span style='color:red'>" + qsTr("Not activated") + "</span>")
        }
        Row {
            spacing: 5
            Button {
                text: qsTr("Activation")
                visible: !activation
                onClicked: {
                    activationDlg.visible = true
                }
            }
            Button {
                text: qsTr("Remove key")
                visible: activation
                onClicked: {
                    properties.actKey = "";
                    properties.isActivated = false;
                }
            }
        }
    }

    standardButtons: Dialog.Ok
}
