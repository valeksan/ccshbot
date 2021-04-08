import QtQuick 2.12
import QtQuick.Controls 2.12
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
