import QtQuick 2.15

SettingsForm {
    title: qsTr("Настройки")

    cfgSocketHost.validator: RegularExpressionValidator {
        regularExpression: /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
    }

//    cfgSocketPort.up.indicator.visible: false
//    cfgSocketPort.down.indicator.visible: false
    cfgSocketPort.editable: true

    //...

    cfgSocketHost.onTextEdited: {
        console.log(cfgSocketHost.text)
        if(cfgSocketHost.acceptableInput) {
            console.log(cfgSocketHost.text)
            settings.listenHost = cfgSocketHost.text;
        }
    }

    cfgSocketPort.onValueChanged: {
        settings.listenPort = cfgSocketPort.value;
    }

    Component.onCompleted: {
        cfgSocketHost.text = settings.listenHost;
        cfgSocketPort.value = settings.listenPort;

//        cfgSocketPort.up.indicator.destroy();
//        cfgSocketPort.down.indicator.destroy();
    }
}
