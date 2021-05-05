import QtQuick 2.12
import QtWebSockets 1.15

import ccbot.tasks 1.0

ChatForm {
    id: page

    property bool clientConnected: false

    property WebSocket ccbotPlugin

    WebSocketServer {
        id: server
        listen: !page.clientConnected && listenClients
        port: 3000
        onClientConnected: {
            console.log('Client connected!')
            ccbotPlugin = webSocket;
            ccbotPlugin.statusChanged.connect(function(status) {
                console.log("status:", status)
                page.clientConnected = (status === WebSocket.Open);
            })
            ccbotPlugin.onTextMessageReceived.connect(function(message) {
                //appendMessage(qsTr("Server received message: %1").arg(message));
                //webSocket.sendTextMessage(qsTr("Hello Client!"));
                //console.log('Server received message', message)
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
                    return;
                }

                let timestamp2 = Math.floor(Date.now() / 1000);
                let diff = Math.abs(timestamp2 - timestamp1);

                console.log("timestamp_diff", diff)

                if(type === "chat_datagram" && diff < 2) {
                    ccbot.action(Task.MergeChat, [streamId, messages]);
                }
            });
        }
        onErrorStringChanged: {
            //appendMessage(qsTr("Server error: %1").arg(errorString));
            console.log('Server error', errorString)
        }
    }
}
