import QtQuick 2.15

import ccbot.tasks 1.0


SettingsPageForm {
    id: page

    property bool showOAuthToken: false

    title: qsTr("Настройки")

    btShowOAuthToken.text: "👁"
    btShowOAuthToken.onClicked: {
        showOAuthToken = !showOAuthToken;
    }

    cfgSocketHost.enabled: !properties.listenClients
    cfgSocketHost.validator: RegularExpressionValidator {
        regularExpression: /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
    }
    cfgSocketHost.onTextEdited: {
        console.log(cfgSocketHost.text)
        if(cfgSocketHost.acceptableInput) {
            console.log(cfgSocketHost.text)
            properties.listenHost = cfgSocketHost.text;
        }
    }

    cfgSocketPort.enabled: !properties.listenClients
    cfgSocketPort.inputMethodHints: Qt.ImhFormattedNumbersOnly
    cfgSocketPort.validator: IntValidator {
        bottom: 0;
        top: 65535;
    }
    cfgSocketPort.onTextEdited: {
        if(cfgSocketPort.acceptableInput) {
            properties.listenPort = parseInt(cfgSocketPort.text);
        }
    }

    cfgSocketTimestampDiff.value: properties.maxTimestampDiff;
    cfgSocketTimestampDiff.enabled: true
    cfgSocketTimestampDiff.onValueModified: {
        properties.maxTimestampDiff = cfgSocketTimestampDiff.value;
    }

    cfgSpeechkitFolderId.text: properties.speechkitFolderId
    cfgSpeechkitFolderId.onTextEdited: {
        properties.speechkitFolderId = cfgSpeechkitFolderId.text;
    }

    cfgSpeechkitOAuthToken.text: properties.speechkitOAuthToken
    cfgSpeechkitOAuthToken.echoMode: showOAuthToken ?
                                         TextInput.Normal : TextInput.Password
    cfgSpeechkitOAuthToken.onTextEdited: {
        properties.speechkitOAuthToken = cfgSpeechkitOAuthToken.text;
    }

    cfgSpeechkitVoice.textRole: "title"
    cfgSpeechkitVoice.model: [{
            "title": "Оксана (Женский, Русский)",
            "lang": "ru-RU",
            "voice": "oksana"
        }, {
            "title": "Oksana (Female, English)",
            "lang": "en-US",
            "voice": "oksana"
        }, {
            "title": "Филипп PR (Мужской, Русский)",
            "lang": "ru-RU",
            "voice": "filipp"
        }, {
            "title": "Алена PR (Женский, Русский)",
            "lang": "ru-RU",
            "voice": "alena"
        }, {
            "title": "Джейн (Женский, Русский)",
            "lang": "ru-RU",
            "voice": "jane"
        }, {
            "title": "Jane (Female, English)",
            "lang": "en-US",
            "voice": "jane"
        }, {
            "title": "Омаж (Женский, Русский)",
            "lang": "ru-RU",
            "voice": "omazh"
        }, {
            "title": "Omazh (Female, English)",
            "lang": "en-US",
            "voice": "omazh"
        }, {
            "title": "Захар (Мужской, Русский)",
            "lang": "ru-RU",
            "voice": "zahar"
        }, {
            "title": "Zahar (Male, English)",
            "lang": "en-US",
            "voice": "zahar"
        }, {
            "title": "Ермил (Мужской, Русский)",
            "lang": "ru-RU",
            "voice": "ermil"
        }, {
            "title": "Ermil (Male, English)",
            "lang": "en-US",
            "voice": "ermil"
        }, {
            "title": "Silaerkan (Kadın, Türk)",
            "lang": "tr-TR",
            "voice": "silaerkan"
        }, {
            "title": "Erkanyavas (Erkek, Türk)",
            "lang": "tr-TR",
            "voice": "erkanyavas"
        }, {
            "title": "Alyss (Female, English)",
            "lang": "en-US",
            "voice": "alyss"
        }, {
            "title": "Алиса (Женский, Русский)",
            "lang": "ru-RU",
            "voice": "alyss"
        }, {
            "title": "Nick (Male, English)",
            "lang": "en-US",
            "voice": "nick"
        }]
    cfgSpeechkitVoice.onActivated: {
        properties.speechkitLang = cfgSpeechkitVoice.model[index].lang;
        properties.speechkitVoice = cfgSpeechkitVoice.model[index].voice;
        if (properties.speechkitVoice === "jane" ||
                properties.speechkitVoice === "omazh" ||
                properties.speechkitVoice === "alyss")
        {
            panelSpeechkitEmotion.visible = true;
            if (properties.speechkitEmotion.length === 0) {
                cfgSpeechkitEmotion.currentIndex = 1;
            } else {
                for (let j = 0; j < cfgSpeechkitEmotion.model.length; j++) {
                    const emotionValue = cfgSpeechkitEmotion.model[j].emotion;
                    if (emotionValue === properties.speechkitEmotion) {
                        cfgSpeechkitEmotion.currentIndex = j;
                    }
                }
            }
        }
        else {
            panelSpeechkitEmotion.visible = false;
            properties.speechkitEmotion = "";
        }
    }

    cfgSpeechkitEmotion.textRole: "title"
    cfgSpeechkitEmotion.valueRole: "emotion"
    cfgSpeechkitEmotion.model: [{
            "title": "Раздраженный",
            "emotion": "evil"
        }, {
            "title": "Нейтральный",
            "emotion": "neutral"
        }, {
            "title": "Радостный",
            "emotion": "good"
        }]
    cfgSpeechkitEmotion.onActivated: {
        properties.speechkitEmotion = cfgSpeechkitEmotion.model[index].emotion;
    }
    btTestVoice.onClicked: {
        ccbot.action(Task.VoiceLoad, ["съешь еще этих мягких булочек"]);
    }

    cfgSpeechkitFormat.textRole: "title"
    cfgSpeechkitFormat.valueRole: "format"
    cfgSpeechkitFormat.model: [{
            "title": "OGG OPUS (*.ogg)",
            "format": "oggopus"
        }, {
            "title": "LPCM (*.wav)",
            "format": "lpcm"
        }]
    cfgSpeechkitFormat.onActivated: {
        if (cfgSpeechkitFormat.model[index].format === "lpcm") {
            properties.speechkitFormat = "lpcm";
            console.log("lpcm!")
        } else {
            console.log("_!")
            properties.speechkitFormat = "";
            properties.speechkitSampleRateHertz = "";
        }
    }

    panelSpeechkitSampleRateHertz.visible: properties.speechkitFormat === "lpcm"
    cfgSpeechkitSampleRateHertz.textRole: "title"
    cfgSpeechkitSampleRateHertz.valueRole: "rate"
    cfgSpeechkitSampleRateHertz.model: [{
            "title": "48кГц",
            "rate": ""
        },
        {
            "title": "16кГц",
            "rate": "16000"
        },
        {
            "title": "8кГц",
            "rate": "8000"
        }
    ]
    cfgSpeechkitSampleRateHertz.onActivated: {
        properties.speechkitSampleRateHertz =
                cfgSpeechkitSampleRateHertz.currentValue;
        console.log(properties.speechkitSampleRateHertz)
    }

    cfgSpeechkitSpeed.value: properties.speechkitSpeed.length === 0 ?
                                 1.0 : parseFloat(properties.speechkitSpeed)
    cfgSpeechkitSpeed.editable: true
    cfgSpeechkitSpeed.precision: 1
    cfgSpeechkitSpeed.from: 0.1
    cfgSpeechkitSpeed.to: 3.0
    cfgSpeechkitSpeed.step: 0.1
    cfgSpeechkitSpeed.enableSequenceGrid: true
    cfgSpeechkitSpeed.suffix: "x"
    cfgSpeechkitSpeed.onFinishEdit: {
        properties.speechkitSpeed = (number === 1.0 ?
                                         "" : number.toFixed(1));
    }

    testMsgTTS.text: "Напишите тут любое сообщение%"
    testMsgTTS.selectByMouse: true
    btTestTextMsgTTS.enabled: testMsgTTS.text.length > 0
    btTestTextMsgTTS.onClicked: {
        ccbot.action(Task.VoiceLoad, [testMsgTTS.text]);
    }

    //...

    focusEnder.onClicked: focusEnder.parent.forceActiveFocus()

    Component.onCompleted: {
        // server
        cfgSocketHost.text = properties.listenHost;
        cfgSocketPort.text = properties.listenPort.toString();

        // speechkit
        // - spinbox - for show current language
        for (let i = 0; i < cfgSpeechkitVoice.model.length; i++) {
            const voiceValue = cfgSpeechkitVoice.model[i].voice;
            const langValue = cfgSpeechkitVoice.model[i].lang;
            if (langValue === properties.speechkitLang &&
                    voiceValue === properties.speechkitVoice)
            {
                cfgSpeechkitVoice.currentIndex = i;
                break;
            }
        }

        // - for show current emotion
        if (properties.speechkitVoice === "jane" ||
                properties.speechkitVoice === "omazh" ||
                properties.speechkitVoice === "alyss")
        {
            panelSpeechkitEmotion.visible = true;
        } else {
            panelSpeechkitEmotion.visible = false;
        }
        for (let j = 0; j < cfgSpeechkitEmotion.model.length; j++) {
            const emotionValue = cfgSpeechkitEmotion.model[j].emotion;
            if (emotionValue === properties.speechkitEmotion) {
                cfgSpeechkitEmotion.currentIndex = j;
            }
        }

        // - for show format
        if (properties.speechkitFormat === "lpcm") {
        console.log(properties.speechkitFormat)
        console.log(cfgSpeechkitFormat.indexOfValue(properties.speechkitFormat))
        cfgSpeechkitFormat.currentIndex = cfgSpeechkitFormat.indexOfValue(properties.speechkitFormat);
        } else {
            cfgSpeechkitFormat.currentIndex = 0;
        }

        // - for show rate
        if (properties.speechkitFormat === "lpcm") {
            cfgSpeechkitSampleRateHertz.currentIndex =
                    cfgSpeechkitSampleRateHertz.indexOfValue(
                        properties.speechkitSampleRateHertz);
        }
    }
}
