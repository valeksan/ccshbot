import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: dialog
    title: qsTr("О программе")
    modal: true
    width: 360

    Column {
        Label {
            text: "Наименование: " + Qt.application.name
        }
        Label {
            text: "Версия: " + Qt.application.version
        }
        Label {
            text: "Дата сборки: " + releaseDate
        }
    }

    standardButtons: Dialog.Ok
}
