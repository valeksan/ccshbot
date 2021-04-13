import QtQuick 2.12

import ccbot.tasks 1.0

AuthForm {
    id: page

    authDataEmail.text: settings.email
    authDataPassword.text: settings.password

    authBtSignin.enabled: authDataEmail.length !== 0 && authDataPassword.length !== 0
    authBtSignin.onClicked: {
        ccbot.action(Task.Auth, [authDataEmail.text, authDataPassword.text]);
        if (flagAuthSave) {
            settings.email = authDataEmail.text;
            settings.password = authDataPassword.text;
        }
    }

    flagAuthSave.checked: settings.authSave
}
