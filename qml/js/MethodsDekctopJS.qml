import QtQuick 2.12
import QtQml 2.12

import Vip.Desktop 1.0

QtObject {

    // - для получения положение свободной области рабочего стола по X-координате
    function getDescktopX() {
        var rectDescktop = Desktop.availableGeometry();
        return rectDescktop.x;
    }

    // - для получения положение свободной области рабочего стола по Y-координате
    function getDescktopY() {
        var rectDescktop = Desktop.availableGeometry();
        return rectDescktop.y;
    }

    // - для получения размера свободной области экрана по вертикали
    function getDesktopAvailableHeight() {
        var rectDescktop = Desktop.availableGeometry();
        return rectDescktop.height;
    }

    // - для получения размера свободной области экрана по горизонтали
    function getDesktopAvailableWidth() {
        var rectDescktop = Desktop.availableGeometry();
        return rectDescktop.width;
    }

    function getHeightDecorations() {
        var rectDescktop = Desktop.availableGeometry();
        return Screen.height - rectDescktop.height;
    }

    function getPageHeight() {
        return appBody.height;
    }

    function getPageWidth() {
        return appBody.width;
    }
}
