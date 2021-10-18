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
            text: Qt.application.name + Qt.application.version
            color: "yellow"
        }
//        Label {
//            textFormat: Label.RichText
//            text: qsTr("Version") + ": <span style='color:white'>" + Qt.application.version + "</span>"
//            color: "yellow"
//        }
        Label {
            textFormat: Label.RichText
            text: qsTr("Build Date") + ": <span style='color:white'>" + releaseDate + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: qsTr("Activation") + ": " + (dialog.activation ? "<span style='color:white'>" + qsTr("Activated") + ", " + qsTr("expiry in ") + ccbot.getEndActivationDate() + "</span>" : "<span style='color:red'>" + qsTr("Not activated") + "</span>")
            color: "yellow"
        }
//        Label {
//            textFormat: Label.RichText
//            visible: activation
//            text: qsTr("expiry in ") + ccbot.getEndActivationDate();
//        }

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
