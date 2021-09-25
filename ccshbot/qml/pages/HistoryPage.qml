import QtQuick 2.15

HistoryPageForm {
    id: page

    title: qsTr("History")

    function chatAddText(msg) {
        var moveToBottom = flickChatH.atYEnd;
        chatRepeaterH.append(msg);
        if (moveToBottom) {
            if (flickChatH.contentHeight > flickChatH.height) {
                flickChatH.contentY = flickChatH.contentHeight - flickChatH.height;
            }
        }
    }

    chatRepeaterH.font.family: properties.fontNameForChat
    chatRepeaterH.font.pointSize: properties.fontPointSizeForChat
    chatRepeaterH.color: properties.textColorForChat
    chatRepeaterH.text: ""

    selectNikname.model: ccbot.getModelAvaibleHistoryNiknames()
    selectNikname.onActivated: {
        selectStreamId.model = ccbot.getModelAvaibleHistoryStreamsByNikname(selectNikname.currentText);
    }

    selectStreamId.onActivated: {
        chatRepeaterH.clear();
        ccbot.displayChatHistory(selectNikname.currentText, selectStreamId.currentText);
    }

    Connections {
        target: ccbot
        function onShowHistoryMessage(message) {
            page.chatAddText(message);
        }
    }

    Component.onCompleted: {
        //
    }
}
