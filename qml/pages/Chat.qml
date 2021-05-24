import QtQuick 2.15
import QtWebSockets 1.15

import ccbot.tasks 1.0

ChatForm {
    id: page

    title: qsTr("Чат") + " " + properties.currentStreamId

    property bool baseOpenned: false

    property WebSocket client: null

    property int idleCount: 0
    property int connectCount: 0

    chatRepeater.font.family: properties.fontNameForChat
    chatRepeater.font.pointSize: properties.fontPointSizeForChat
    chatRepeater.color: properties.textColorForChat
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

    function idlCheck() {
        window.setTimeout(() => {
                              ++page.idleCount;
                              if(page.idleCount === 5) {
                                  page.idleCount = 0;
                                  client.active = false;
                                  --connectCount;
                              } else {
                                  if(properties.listenClients) {
                                    idlCheck();
                                  }
                              }
                          }, 1000);
    }

    WebSocketServer {
        id: server
        listen: connectCount === 0 && properties.listenClients
        port: properties.listenPort
        host: properties.listenHost
        onClientConnected: {
            console.log('Client add!');

            if(connectCount > 0) {
                webSocket.active = false;
            }

            ++connectCount;
            client = webSocket;
            client.statusChanged.connect(function(status) {
                console.log("status:", status)
                try {
                    switch(status) {
                    case WebSocket.Connecting:
                        console.log('Client connecting...')
                        break;
                    case WebSocket.Open:
                        console.log('Client connected!')
                        window.changeStatus("CCBot plugin connected", 3000, "yellow");
                        break;
                    case WebSocket.Closing:
                        console.log('Client closing...')
                        window.changeStatus("CCBot plugin disconnected", 3000, "yellow");
                        break;
                    case WebSocket.Closed:
                        console.log('Client disconnected!')
                        break;
                    case WebSocket.Error:
                        break;
                    }
                } catch(e) {
                    console.warn("Err connection:", e);
                }
            })
            client.onTextMessageReceived.connect(function(message) {

                page.idleCount = 0;

                if(!baseOpenned) {
                    console.warn("Base no oppened!");
                    return;
                }

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

                if (properties.currentStreamId !== streamId) {
                    properties.currentStreamId = streamId;
                    properties.flagLoadingChat = true;
                    chatRepeater.clear();
                    console.log("LoadChat")
                }

                if (diff <= properties.maxTimestampDiff) {
                    switch(type) {
                    case "chat_datagram":
                        let loading = properties.flagLoadingChat;
                        if (loading)
                            console.log("MergeChat")
                        ccbot.action(Task.MergeChat, [streamId, messages, loading]);
                        break;
                    }
                } else {
                    window.changeStatus("Ошибка обмена: временная метка пакета устарела", 1500, "red");
                }
            });
            page.idlCheck();
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
            //console.log("_1")
        }
        function onBaseOpenned(state) {
            page.baseOpenned = state;
        }
    }

    Connections {
        target: properties
        function onListenClientsChanged() {
            if(properties.listenClients === false) {
                page.idleCount = 0;
                if(page.client) {
                    try {
                        page.client.active = false;
                        --connectCount;
                    } catch(e) {

                    }
                }
            }
        }
    }

//    Component.onDestruction: {
//        server.listen = false;
//    }
}
