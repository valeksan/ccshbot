import QtQuick 2.15
import QtWebSockets 1.15

import ccbot.tasks 1.0

ViewChatForm {
    id: page

    title: qsTr("Чат") + getExtTitle()

    property bool baseOpenned: ccbot.isOpenedDB()

    property WebSocket client: null

    property int idleCount: 0
    property int connectCount: 0

    chatRepeater.font.family: properties.fontNameForChat
    chatRepeater.font.pointSize: properties.fontPointSizeForChat
    chatRepeater.color: properties.textColorForChat
    chatRepeater.text: ""

    function getExtTitle() {
        if (properties.currentStreamId.length > 0)
            return ` ${properties.currentStreamId} <span style="color:yellow">${properties.currentStreamerNikname}</span>`;

        return "";
    }

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

            if (properties.flagLogging) {
                ccbot.addToLog(`Notification. Client connected!`);
            }

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
                        if (properties.flagLogging) {
                            ccbot.addToLog(`Error client socket connection. ${client.errorString()}.`);
                        }
                        break;
                    }
                } catch(e) {
                    console.warn("Err connection:", e);
                    if (properties.flagLogging) {
                        ccbot.addToLog(`Error socket client connect. ${e}`);
                    }
                }
            })
            client.onTextMessageReceived.connect(function(message) {

                page.idleCount = 0;

                let datagram;
                let type;
                let timestamp1;
                let streamId;
                let streamerName;
                let messages;
                try {
                    datagram = JSON.parse(message);
                    type = datagram['type'];
                    timestamp1 = datagram['timestamp'];
                    streamId = datagram['streamId'];
                    streamerName = datagram['streamerName'];
                    messages = JSON.stringify(datagram['messages']);
                } catch (err) {
                    console.warn(err);
                    window.changeStatus("Ошибка: " + err, 2000, "red");
                    if (properties.flagLogging) {
                        ccbot.addToLog(`Error socket client read message. ${err}`);
                    }
                    return;
                }

                let timestamp2 = Math.floor(Date.now() / 1000);
                let diff = Math.abs(timestamp2 - timestamp1);

                if (!streamId) {
                    window.changeStatus("Ошибка, номер стрима не определен!", 2000, "red");
                    return;
                }

                if (properties.currentStreamId !== streamId) {
                    properties.currentStreamId = streamId;
                    properties.currentStreamerNikname = streamerName;
                    properties.flagLoadingChat = true;
                    chatRepeater.clear();
                    console.log("LoadChat")
                    ccbot.closeDB();
                    ccbot.openDB(`${streamerName}.db`);
                    if (properties.flagLogging) {
                        ccbot.addToLog(`Notification. Load chat ${properties.currentStreamId} with streamer ${streamerName}.`);
                    }
                }

                if (!baseOpenned) {
                    ccbot.openDB(`${streamerName}.db`);
                }

                if (diff <= properties.maxTimestampDiff) {
                    switch(type) {
                    case "chat_datagram":
                        let loading = properties.flagLoadingChat;
                        if (loading)
                            console.log("Loading chat ...")
                        ccbot.action(Task.MergeChat, [streamId, messages, loading]);
                        break;
                    }
                } else {
                    window.changeStatus("Ошибка обмена: временная метка пакета устарела", 1500, "red");
                    if (properties.flagLogging) {
                        ccbot.addToLog(`Error. Discrepancy of timestamp diff(${diff})!`);
                    }
                }
            });
            page.idlCheck();
        }

        onErrorStringChanged: {
            console.log('Server error', errorString)
            window.changeStatus("Server error: " + errorString, 2000, "red");
            if (properties.flagLogging) {
                ccbot.addToLog(`Error socket server. ${errorString}`);
            }
        }
    }

    btVoiceOff.onToggled: {
        properties.flagAnalyseVoiceAllMsgType0 = false;
        properties.flagAnalyseVoiceAllMsgType2 = false;
    }

    btVoiceType2.onToggled: {
        properties.flagAnalyseVoiceAllMsgType0 = false;
        properties.flagAnalyseVoiceAllMsgType2 = true;
    }

    btVoiceAll.onToggled: {
        properties.flagAnalyseVoiceAllMsgType0 = true;
        properties.flagAnalyseVoiceAllMsgType2 = true;
    }

    toolButtonStartServer.text: properties.listenClients ? qsTr("Отключиться") : qsTr("Читать чат")
    toolButtonStartServer.onClicked: {
        if (!properties.listenClients) {
            window.changeStatus("Запуск сервера ...",
                                1500, "yellow")
            properties.flagLoadingChat = true
        } else {
            window.changeStatus("Остановка сервера ...",
                                1500, "yellow")
        }
        properties.listenClients = !properties.listenClients
    }

    btSendMsg.enabled: client !== null && inputMsg.text.length > 0
    btSendMsg.onClicked: {
        let sendObj = { "type":"message", "text":inputMsg.text };
        client.sendTextMessage(JSON.stringify(sendObj));
        inputMsg.clear();
    }

    inputMsg.onAccepted: {
        if (inputMsg.text.length === 0)
            return;
        if (client !== null) {
            let sendObj = { "type":"message", "text":inputMsg.text };
            client.sendTextMessage(JSON.stringify(sendObj));
            inputMsg.clear();
        }
    }

    Connections {
        target: ccbot
        function onShowChatMessage(message) {
            page.chatAddText(message);
        }
    }

    Connections {
        target: properties
        function onListenClientsChanged() {
            if (properties.listenClients === false) {
                page.idleCount = 0;
                if (page.client) {
                    try {
                        page.client.active = false;
                        --connectCount;
                        if (properties.flagLogging) {
                            ccbot.addToLog(`Notification. Client disconnected!`);
                        }
                    } catch(e) {
                        if (properties.flagLogging) {
                            ccbot.addToLog(`Error client socket. ${e}`);
                        }
                    }
                    page.client = null;
                }
                ccbot.closeDB();
            }
        }
    }
}
