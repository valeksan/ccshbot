import QtQuick 2.12

AuthForm {
    id: page

    authDataEmail.text: settings.email
    authDataPassword.text: settings.password

    authBtSignin.enabled: authDataEmail.length !== 0 && authDataPassword.length !== 0
    authBtSignin.onClicked: {
        //
    }

    flagAuthSave.checked: settings.authSave
}
