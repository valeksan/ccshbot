import QtQuick 2.12
import QtWebSockets 1.15

import ccbot.tasks 1.0

ChatForm {
    id: page

    property bool clientConnected: false

    title: qsTr("Чат") + " " + properties.currentStreamId
    chatRepeater.font.family: settings.chatFont
    chatRepeater.font.pointSize: settings.chatFontPointSize
    chatRepeater.color: settings.chatTextColor
    chatRepeater.text: ""

    function chatAddText(msg) {
        var moveToBottom = flickChat.atYEnd;
        chatRepeater.append(msg);
        if (moveToBottom) {
            if (flickChat.contentHeight > flickChat.height) {
                flickChat.contentY = flickChat.contentHeight - flickChat.height;
            }
        }
    }

    WebSocketServer {
        id: server
        listen: !page.clientConnected && window.listenClients
        port:  settings.listenPort
        host: settings.listenHost === 'localhost' ? "127.0.0.1" : settings.listenHost
        onClientConnected: {
            console.log('Client connected!')
            webSocket.statusChanged.connect(function(status) {
                console.log("status:", status)
                try {
                    switch(status) {
                    case WebSocket.Connecting:
                        break;
                    case WebSocket.Open:
                        window.changeStatus("CCBot plugin connected", 3000, "yellow");
                        page.clientConnected = true;
                        break;
                    case WebSocket.Closing:
                        window.changeStatus("CCBot plugin disconnected", 3000, "yellow");
                        page.clientConnected = false;
                        window.listenClients = !window.listenClients;
                        window.setTimeout(() => { window.listenClients = !window.listenClients }, 100);
                        webSocket.active = false;
                        break;
                    case WebSocket.Closed:
                        break;
                    case WebSocket.Error:
                        break;
                    }
                } catch(e) {}
            })
            webSocket.onTextMessageReceived.connect(function(message) {
                let datagram;
                let type;
                let timestamp1;
                let streamId;
                let messages;
                try {
                    datagram = JSON.parse(message);
                    type = datagram['type'];
                    timestamp1 = datagram['timestamp'];
                    streamId = datagram['streamId'];
                    messages = JSON.stringify(datagram['messages']);
                } catch(err) {
                    console.warn(err);
                    window.changeStatus("Ошибка: " + err, 2000, "red");
                    return;
                }

                let timestamp2 = Math.floor(Date.now() / 1000);
                let diff = Math.abs(timestamp2 - timestamp1);

                if (properties.flagLoadingChat || properties.currentStreamId !== streamId) {
                    properties.currentStreamId = streamId;
                    chatRepeater.text = ""; //clear();
                    ccbot.action(Task.LoadChat, [streamId]);
                    return;
                }

                if(type === "chat_datagram" && diff < 2) {
                    ccbot.action(Task.MergeChat, [streamId, messages]);
                } else {
                    if(diff >= 2)
                        window.changeStatus("Ошибка обмена: временная метка пакета устарела", 800, "red");
                }
            });
        }
        onErrorStringChanged: {
            console.log('Server error', errorString)
            window.changeStatus("Server error: " + errorString, 2000, "red");
        }
    }

    Connections {
        target: ccbot
        function onShowChatMessage(message) {
            page.chatAddText(message);
            console.log("_1")
        }
        function onChatLoadCompleted(err) {
            if(err === 0) {
                properties.flagLoadingChat = false;
                console.log("_2")
            } else {
                console.warn("fail load chat!")
            }
        }
    }

//    Component.onDestruction: {
//        server.listen = false;
//    }
}
