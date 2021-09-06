import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Dialog {
    id: dialog
    title: qsTr("Активация")
    modal: true
    width: 600

    Column {
        Row {
            TextArea {
                textFormat: Label.RichText
                width: dialog.width-160
                wrapMode: "WrapAnywhere"
                readOnly: true
                selectByMouse: true
                text: "Регистрационный ключ: <span style='color:white'>" + ccbot.getRegistrationCode() + "</span>"
                color: "yellow"
            }
            Button {
                width: 120
                text: "Скопировать"
            }
        }
    }

    standardButtons: Dialog.Ok
}
