import QtQuick 2.12
import QtQuick.Controls 2.12

Dialog {
    id: dialog

    title: ""
    property string text: ""
    property bool alert: false

    modal: true
    width: 360

    Column {
        Label {
            text: dialog.text
            color: dialog.alert ? "pink" : "white"
        }
    }

    standardButtons: Dialog.Ok

    function show(title, text, alert) {

        dialog.alert = alert || false;
        dialog.title = title || "";
        dialog.text = text || "";

        open();
    }
}
