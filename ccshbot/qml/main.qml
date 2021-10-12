import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import QtQuick.Dialogs 1.2

import ccbot.enums 1.0

import "pages"
import "dialogs"
import "panels"

ApplicationWindow {
    id: window

    property alias appState: appStatusBar.appState

    function setTimeout(func, interval, ...params) {
        return setTimeoutComponent.createObject(window, { func, interval, params} );
    }

    Component {
        id: setTimeoutComponent
        Timer {
            property var func
            property var params
            running: true
            repeat: false
            onTriggered: {
                func(...params);
                destroy();
            }
        }
    }

    function changeStatus(text, ms = -1, color = "white") {
        appState.color = color;
        appState.text = text;
        if(ms >= 0)
            setTimeout(() => {
                appState.text = "";
            }, ms);
    }

    x: properties.windowX
    y: properties.windowY
    height: properties.windowHeight
    width: properties.windowWidth
    minimumWidth: 1024
    minimumHeight: 620

    color: "#1F1F2A"

    visible: properties.isActivated

    header: AppToolBar {
        id: appToolBar

        background: Rectangle {
            color: "#121217"
        }
    }

    AppLeftMenu {
        id: drawer
        width: window.minimumWidth / 2
        height: window.height
    }

    StackView {
        id: stackView
        initialItem:  Qt.resolvedUrl("qrc:///qml/pages/ViewChat.qml")
        anchors.fill: parent
    }

    footer: AppStatusBar {
        id: appStatusBar
        height: 60
        background: Rectangle {
            color: "#121217"
        }
    }


    Message {
        id: messageDlg
        anchors.centerIn: parent
    }
    About {
        id: aboutDlg
        anchors.centerIn: parent
    }
    Activation {
        id: activationDlg
        anchors.centerIn: parent
    }

    Connections {
        target: ccbot
        function onShowMessage(title, text, alert) {
            messageDlg.show(title, text, alert);
        }
        function onShowTrialDlg() {
            var component = Qt.createComponent("qrc:/qml/dialogs/Trial.qml");
            if (component.status == Component.Ready) {
                try {
                    var dlg = component.createObject();
                    dlg.onShowMainWindow.connect(function(){
                        window.visible = true;
                        dlg.destroy();
                    });
                    dlg.visible = true
                } catch(e){}
            }
        }
    }

    onHeightChanged: {
        properties.windowHeight = window.height;
    }

    onWidthChanged: {
        properties.windowWidth = window.width;
    }

    onXChanged: {
        properties.windowX = window.x;
    }

    onYChanged: {
        properties.windowY = window.y;
    }

    onClosing: {
        ccbot.saveSettings();
    }
}
