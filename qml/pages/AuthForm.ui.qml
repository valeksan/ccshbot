import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

Page {
    property alias authDataEmail: authDataEmail
    property alias authDataPassword: authDataPassword
    property alias authBtSignin: authBtSignin
    property alias flagAuthSave: flagAuthSave
    title: qsTr("Авторизация")

    Pane {
        anchors.centerIn: parent
        width: 640
        height: 480
        Material.elevation: 6
        antialiasing: true
        Label {
            text: "Войдите в свою учетную запись"
            color: "#fdef00"
            font.pixelSize: 28
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            horizontalAlignment: "AlignHCenter"
        }
        ColumnLayout {
            spacing: 25
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 15
            width: 250
            ColumnLayout {
                spacing: 5
                Label {
                    text: qsTr("Электронная почта")
                    Layout.fillWidth: true
                    font.pixelSize: 19
                }
                TextField {
                    id: authDataEmail
                    Layout.fillWidth: true
                    font.pixelSize: 19
                }
            }
            ColumnLayout {
                spacing: 5
                Label {
                    text: qsTr("Пароль")
                    Layout.fillWidth: true
                    font.pixelSize: 19
                }
                TextField {
                    id: authDataPassword
                    Layout.fillWidth: true
                    font.pixelSize: 19
                    echoMode: TextInput.Password
                }
            }
            ColumnLayout {
                spacing: 5
                Button {
                    id: authBtSignin
                    Layout.fillWidth: true
                    text: qsTr("Войти")
                }
                CheckBox {
                    id: flagAuthSave
                    text: qsTr("Запомнить")
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

