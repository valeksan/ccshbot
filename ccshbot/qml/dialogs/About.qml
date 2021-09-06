import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Dialog {
    id: dialog
    title: qsTr("О программе")
    modal: true
    width: 400

    property bool activation: ccbot.verifyActivation()

    Column {
        Label {
            id: codename
            textFormat: Label.RichText
            text: "Наименование: <span style='color:white'>" + Qt.application.name + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: "Версия: <span style='color:white'>" + Qt.application.version + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: "Дата сборки: <span style='color:white'>" + releaseDate + "</span>"
            color: "yellow"
        }
        Label {
            textFormat: Label.RichText
            text: "Активация: " + (activation ? "<span style='color:green'>Ок</span>" : "<span style='color:red'>Не активировано</span>")
            color: "yellow"
        }
        Button {
            text: "Активация"
            visible: !activation
            onClicked: {
                activationDlg.visible = true
            }
        }
    }

    standardButtons: Dialog.Ok
}
