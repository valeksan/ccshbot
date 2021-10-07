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
            text: qsTr("Name") + ": <span style='color:white'>" + Qt.application.name + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: qsTr("Version") + ": <span style='color:white'>" + Qt.application.version + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: qsTr("Build Date") + ": <span style='color:white'>" + releaseDate + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: qsTr("Activation") + ": " + (dialog.activation ? "<span style='color:white'>" + qsTr("Activated") + "</span>" : "<span style='color:red'>" + qsTr("Not activated") + "</span>")
            color: "yellow"
        }
        Button {
            text: "Activation"
            visible: !activation
            onClicked: {
                activationDlg.visible = true
            }
        }
    }

    standardButtons: Dialog.Ok
}
