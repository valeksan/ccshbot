import QtQuick 2.12
import QtQuick.Controls 2.12
import QtWebEngine 1.8
import QtQml 2.12
import QtQuick.Layouts 1.12

import Qt.labs.settings 1.1

import "js"

ApplicationWindow {
    id: applicationWindow

    x: settings.x
    y: settings.y
    height: settings.height
    width: settings.width

    visibility: "Windowed"

    visible: true

    RowLayout {
        spacing: 5
        anchors.fill: parent
        Pane {
            Layout.fillHeight: true
            Layout.minimumWidth: (parent.width-5)/2
            WebEngineView {
                id: browser
                function getSource(){
                    var js = "document.documentElement.outerHTML";
                    browser.runJavaScript(js, function(result){textHtml.text = result;});
                }
                anchors.fill: parent
                url: "https://crazycash.tv/login"
                onLoadingChanged: {
                    if(loadRequest.status === WebEngineView.LoadSucceededStatus) {
                        browser.getSource();
                    }
                }
                onNavigationRequested: {
                    console.log("nav req")
                }
            }
        }
        Pane {
            Layout.fillHeight: true
            Layout.minimumWidth: (parent.width-5)/2
            Flickable {
                clip: true
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick
                ScrollBar.vertical: ScrollBar {}
                ScrollBar.horizontal: null
                TextArea.flickable: TextArea {
                    id: textHtml
                    readOnly: true
                    wrapMode: TextArea.WordWrap
                }
            }
        }
    }

    footer: RowLayout {
        height: 60
        Button {
            text: "Clear"
            onClicked: {
                textHtml.clear();
            }
        }
    }

    // объект для хранения настроек программы, храним размер приложения и его положения на экране
    Settings {
        id: settings
        property int height: 900
        property int width: 1000
        property int x: desktopMethods.getDescktopX()
        property int y: desktopMethods.getDescktopY()
    }

    // вспомогательные JS-методы: для позиционирования окна
    MethodsDekctopJS {
        id: desktopMethods
    }

    onHeightChanged: {
        settings.height = height;
    }
    onWidthChanged: {
        settings.width = width;
    }
    onXChanged: {
        settings.x = x;
    }
    onYChanged: {
        settings.y = y;
    }
}
