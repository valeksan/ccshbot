import QtQuick 2.15

SettingsForm {
    id: page

    title: qsTr("Настройки")

    cfgSocketHost.enabled: !properties.listenClients
    cfgSocketPort.enabled: !properties.listenClients
    cfgSocketTimestampDiff.enabled: true

    cfgSocketHost.validator: RegularExpressionValidator {
        regularExpression: /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
    }
    cfgSocketPort.validator: IntValidator {
        bottom: 0;
        top: 65535;
    }

    cfgSocketPort.inputMethodHints: Qt.ImhFormattedNumbersOnly

    cfgSpeechkitFolderId.text: properties.speechkitFolderId
    cfgSpeechkitOAuthToken.text: properties.speechkitOAuthToken
    //...

    cfgSocketHost.onTextEdited: {
        console.log(cfgSocketHost.text)
        if(cfgSocketHost.acceptableInput) {
            console.log(cfgSocketHost.text)
            settings.listenHost = cfgSocketHost.text;
        }
    }

    cfgSocketPort.onTextEdited: {
        if(cfgSocketPort.acceptableInput) {
            settings.listenPort = parseInt(cfgSocketPort.text);
        }
    }

    cfgSocketTimestampDiff.onValueChanged: {
        settings.maxTimestampDiff = cfgSocketTimestampDiff.value;
    }

    cfgSpeechkitFolderId.onTextEdited: {
        properties.speechkitFolderId = cfgSpeechkitFolderId.text;
    }

    cfgSpeechkitOAuthToken.onTextEdited: {
        properties.speechkitOAuthToken = cfgSpeechkitOAuthToken.text;
    }

    focusEnder.onClicked: focusEnder.parent.forceActiveFocus()

    cfgSpeechkitVoice.onActivated: {
        properties.speechkitLang = cfgSpeechkitVoice.model[index].lang;
        properties.speechkitVoice = cfgSpeechkitVoice.model[index].voice;
    }

    Component.onCompleted: {
        cfgSocketHost.text = settings.listenHost;
        cfgSocketPort.text = settings.listenPort.toString();
        cfgSocketTimestampDiff.value = settings.maxTimestampDiff;
    }
}
