import QtQuick 2.15
import QtQuick.Controls 2.15

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
            "title": "Омаж (Женский, Русский)",
            "lang": "ru-RU",
            "voice": "omazh"
        }, {
            "title": "Захар (Мужской, Русский)",
            "lang": "ru-RU",
            "voice": "zahar"
        }, {
            "title": "Ермил (Мужской, Русский)",
            "lang": "ru-RU",
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
            "title": "OGG OPUS",
            "format": "oggopus"
        }, {
            "title": "LPCM",
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

    cfgSpeechkitSymbolPrice.value: properties.speechKitPriceBySymbol
    cfgSpeechkitSymbolPrice.editable: true
    cfgSpeechkitSymbolPrice.precision: 6
    cfgSpeechkitSymbolPrice.from: 0.0
    cfgSpeechkitSymbolPrice.step: 0.001
    cfgSpeechkitSymbolPrice.suffix: " $"
    cfgSpeechkitSymbolPrice.onFinishEdit: {
        properties.speechKitPriceBySymbol = number;
    }

    cfgBoxUserStartBonusBalance.value: properties.boxUserStartingBalance
    cfgBoxUserStartBonusBalance.editable: true
    cfgBoxUserStartBonusBalance.precision: 5
    cfgBoxUserStartBonusBalance.from: 0.0
    cfgBoxUserStartBonusBalance.step: 0.1
    cfgBoxUserStartBonusBalance.suffix: " $"
    cfgBoxUserStartBonusBalance.onFinishEdit: {
        properties.boxUserStartingBalance = number;
    }

    testMsgTTS.text: "Здравствуйте, меня зовут AlexNek, и сегодня я раскажу 4 способа, 4 способа... И сегодня я вам раскажу 4 способа, как запустить стрим на Crazy Cash."
    testMsgTTS.selectByMouse: true
    btTestTextMsgTTS.enabled: testMsgTTS.text.length > 0
    btTestTextMsgTTS.onClicked: {
        ccbot.action(Task.VoiceLoad, [testMsgTTS.text]);
    }

    function addRepKeywordPair() {
        ccbot.addWordPairToReplaceForVoice(replaceKeyword.text,
                                           replaceWord.text);
        replaceWord.clear();
        updateRepPairModels();
    }

    function updateRepPairModels() {
        const currentIndex = lvRepKeywords.currentIndex;
        //console.log(currentIndex)
        try {
            let jdata = JSON.parse(ccbot.getWordPairListInJson());
            lvRepKeywords.model = jdata;
            if (currentIndex !== -1 && currentIndex < jdata.length) {
                lvRepWords.model = jdata[currentIndex]["r"];
                lvRepWords.currentIndex = -1;
            } else {
                lvRepKeywords.currentIndex = -1;
            }
        } catch(e) {
            console.warn("Error parsing! Uncorrect json data!", e);
        }
    }

    btAddRepKeyword.enabled: replaceKeyword.text.length > 0
    btAddRepKeyword.onClicked: {
        addRepKeywordPair();
    }

    replaceKeyword.onAccepted: {
        if (replaceKeyword.text.length === 0) {
            return;
        }
        replaceWord.forceActiveFocus();
        replaceWord.selectAll();
    }
    replaceWord.onAccepted: {
        if (replaceKeyword.text.length === 0) {
            return;
        }
        addRepKeywordPair();
    }

    lvRepKeywords.delegate: compLvkeywordDelegate
    lvRepWords.delegate: compLvWordsDelegate

    btRemoveRepKeyword.enabled: lvRepKeywords.currentIndex !== -1
                                || lvRepWords.currentIndex !== -1
    btRemoveRepKeyword.onClicked: {
        if (lvRepWords.currentIndex !== -1) {
            const keyword = lvRepKeywords.model[lvRepKeywords.currentIndex]["w"];
            if (lvRepWords.model.length === 1) {
                ccbot.removeRepKeywordForVoice(keyword);
                lvRepWords.model = [];
                updateRepPairModels();
                return;
            }
            const word = lvRepWords.model[lvRepWords.currentIndex];
            ccbot.removeRepWordForVoice(keyword, word);
            updateRepPairModels();
        } else if (lvRepKeywords.currentIndex !== -1) {
            const keyword = lvRepKeywords.model[lvRepKeywords.currentIndex]["w"];
            ccbot.removeRepKeywordForVoice(keyword);
            lvRepWords.model = [];
            updateRepPairModels();
        }
    }

    btRepWordDown.visible: lvRepKeywords.currentIndex !== -1
    btRepWordDown.enabled: lvRepKeywords.currentIndex < lvRepKeywords.model.length-1
                           && lvRepKeywords.currentIndex >= 0
                           && lvRepWords.currentIndex === -1
    btRepWordDown.onClicked: {
        ccbot.downSwapRepKeywordForVoice(lvRepKeywords.currentIndex);
        updateRepPairModels();
    }

    btRepWordUp.visible: lvRepKeywords.currentIndex !== -1
    btRepWordUp.enabled: lvRepKeywords.currentIndex > 0
                         && lvRepWords.currentIndex === -1
    btRepWordUp.onClicked: {
        ccbot.upSwapRepKeywordForVoice(lvRepKeywords.currentIndex);
        updateRepPairModels();
    }

    focusEnder.onClicked: focusEnder.parent.forceActiveFocus()

    Component {
        id: compLvkeywordDelegate
        ItemDelegate {
            property var view: ListView.view
            property int itemIndex: index
            text: modelData.w
            width: parent.width
            height: 30
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (view.currentIndex === itemIndex) {
                        view.currentIndex = -1;
                        lvRepWords.model = [];
                    } else {
                        view.currentIndex = itemIndex;
                        lvRepWords.model = lvRepKeywords.model[itemIndex]["r"];
                        lvRepWords.currentIndex = -1;
                    }
                }
            }
        }
    }
    Component {
        id: compLvWordsDelegate
        Item {
            id: rootCompLvWordsDelegate
            property var view: ListView.view
            property int itemIndex: index
            property bool editMode: false

            function editWord(oldWord, newWord) {
                const keyword = lvRepKeywords.model[lvRepKeywords.currentIndex]["w"];
                ccbot.editRepitWordForVoice(keyword, oldWord, newWord);
            }

            width: parent.width
            height: 30
            Label {
                visible: !editMode
                anchors.fill: parent
                anchors.rightMargin: btEditRepWord.width + 5
                anchors.leftMargin: 15
                verticalAlignment: "AlignVCenter"
                text: modelData.length > 0 ? modelData : qsTr("[ <i>текст отсутствует</i> ]")
            }
            MouseArea {
                anchors.fill: parent
                anchors.rightMargin: 35
                enabled: !editMode
                onClicked: {
                    if (view.currentIndex === itemIndex)
                        view.currentIndex = -1;
                    else
                        view.currentIndex = itemIndex;
                }
                onDoubleClicked: {
                    editMode = true;
                }
            }
            Button {
                id: btEditRepWord
                visible: !editMode
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                text: "\u270e" // ✎
                height: 30
                width: 30
                onClicked: {
                    editMode = true;
                }
            }
            Row {
                visible: editMode
                anchors.fill: parent
                anchors.rightMargin: 5
                spacing: 5
                Rectangle {
                    border.color: "gray"
                    radius: 5
                    z: 10
                    color: "#121217"
                    width: parent.width - 70
                    height: parent.height
                    TextInput {
                        id: editWord
                        anchors.fill: parent
                        anchors.leftMargin: 15
                        text: modelData
                        color: "white"
                        onAccepted: {
                            rootCompLvWordsDelegate.editWord(modelData, editWord.text);
                            editMode = false;
                            updateRepPairModels();

                        }
                        verticalAlignment: "AlignVCenter"
                        Keys.onPressed: {
                            if (event.key === Qt.Key_Escape) {
                                editMode = false;
                                event.accepted = true;
                            }
                            if (event.key === Qt.Key_Enter) {
                                rootCompLvWordsDelegate.editWord(modelData, editWord.text);
                                editMode = false;
                                event.accepted = true;
                            }
                        }
                    }
                }
                Button {
                    id: btAcceptEditWord
                    text: "\u2714" // ✔
                    height: 30
                    width: 30
                    onClicked: {
                        rootCompLvWordsDelegate.editWord(modelData, editWord.text);
                        editMode = false;
                        updateRepPairModels();
                    }
                }
                Button {
                    id: btCancelEditWord
                    text: "\u2718" // ✘
                    height: 30
                    width: 30
                    onClicked: {
                        editMode = false;
                    }
                }
            }
        }
    }

    cfgBoxUserByRegisterOnFlag0.onCheckedChanged: {
        properties.boxDefaultOnFlag0 = cfgBoxUserByRegisterOnFlag0.checked;
    }

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

        updateRepPairModels();

        cfgBoxUserByRegisterOnFlag0.checked = properties.boxDefaultOnFlag0;
    }
}
