import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebSockets 1.15

import ccbot.enums 1.0

ViewChatForm {
    id: page

    title: qsTr("Chat") + getExtTitle()

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
        //if (chatRepeater)
        chatRepeater.append(msg);
        if (moveToBottom) {
            if (flickChat.contentHeight > flickChat.height) {
                flickChat.contentY = flickChat.contentHeight - flickChat.height;
            }
        }
    }

    chatRepeater.onLinkActivated: {
        if (inputMsg.length === 0) {
            inputMsg.text = link + ", ";
        } else {
            inputMsg.text = inputMsg.text + link;
        }
        inputMsg.cursorPosition = inputMsg.text.length;
        inputMsg.forceActiveFocus();
    }

    function idlCheck() {
        window.setTimeout(() => {
                              if (chatRepeater.text.length > 0)
                                ++page.idleCount;
                              if (page.idleCount === 10) {
                                page.idleCount = 0;
                                client.active = false;
                                --connectCount;
                              } else {
                                  if (properties.listenClients) {
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
                    window.changeStatus(qsTr("Error") + ": " + err, 2000, "red");
                    if (properties.flagLogging) {
                        ccbot.addToLog(`Error socket client read message. ${err}`);
                    }
                    return;
                }

                let timestamp2 = Math.floor(Date.now() / 1000);
                let diff = Math.abs(timestamp2 - timestamp1);

                if (!streamId) {
                    window.changeStatus(qsTr("Error, stream number not defined!"), 2000, "red");
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
                    window.changeStatus(qsTr("Exchange error: package timestamp is out of date"), 1500, "red");
                    if (properties.flagLogging) {
                        ccbot.addToLog(`Error. Discrepancy of timestamp diff(${diff})!`);
                    }
                }
            });
            page.idlCheck();
        }

        onErrorStringChanged: {
            console.log('Server error', errorString)
            window.changeStatus(qsTr("Server error") + ": " + errorString, 2000, "red");
            if (properties.flagLogging) {
                ccbot.addToLog(`Error socket server. ${errorString}`);
            }
        }
    }

    btVoiceOff.onToggled: {
        properties.speakOptionReasonType = SpeakReason.DisableAll;
    }

    btVoiceType2.onToggled: {
        properties.speakOptionReasonType = SpeakReason.Donation;
    }

    btVoiceBalanceSpending.onToggled: {
        properties.speakOptionReasonType = SpeakReason.BalanceSpending;
    }

    btVoiceAll.onToggled: {
        properties.speakOptionReasonType = SpeakReason.EnableAll;
    }

    btTest.text: "test"
    btTest.visible: false
    btTest.enabled: chatRepeater.length > 0
    btTest.onClicked: {
        let text = chatRepeater.getFormattedText(0, 255)
        console.log("text:", text)
    }

    toolButtonStartServer.text: properties.listenClients ? qsTr("Disconnect") : qsTr("Read chat")
    toolButtonStartServer.onClicked: {
        if (!properties.listenClients) {
            window.changeStatus(qsTr("Server start ..."),
                                1500, "yellow");
            properties.flagLoadingChat = true;
        } else {
            window.changeStatus(qsTr("Stopping the server ..."),
                                1500, "yellow");
            chatRepeater.clear();
            try {
                if (client.active) {
                    client.active = false;
                }
            } catch(e){}
            properties.currentStreamId = "";
            properties.currentStreamerNikname = "";
        }
        properties.listenClients = !properties.listenClients
    }

    function sendMessage() {
        if (inputMsg.text.length === 0)
            return;
        if (inputMsg.text[0] === '!') {
            console.log("cmd:", inputMsg.text);
            ccbot.exec(inputMsg.text);
            inputMsg.clear();
            return;
        }
        if (client != null) {
            let sendObj = { "type":"message", "text":inputMsg.text };
            client.sendTextMessage(JSON.stringify(sendObj));
            inputMsg.clear();
        }
    }
    function sendMessageAuto(text) {
        if (client != null) {
            let sendObj = { "type":"message", "text":text };
            client.sendTextMessage(JSON.stringify(sendObj));
        }
    }

    btSendMsg.enabled: client != null && inputMsg.text.length > 0
    btSendMsg.onClicked: {
        sendMessage();
    }

    inputMsg.selectByMouse: true
    inputMsg.onAccepted: {
        sendMessage();
    }

    focus: true
    Keys.onUpPressed: {
        let cmd = ccbot.keyUpCommand();
        inputMsg.text = cmd;
    }
    Keys.onDownPressed: {
        let cmd = ccbot.keyDownCommand();
        inputMsg.text = cmd;
    }

    Connections {
        target: ccbot

        function onShowChatMessage(message) {
            page.chatAddText(message);
        }

        function onShowChatNotification(info) {
            page.chatAddText(info);
        }

        function onSendChatMessage(text) {
            sendMessageAuto(text);
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

    Component.onCompleted: {
        switch(properties.speakOptionReasonType) {
        case SpeakReason.DisableAll:
            btVoiceOff.checked = true;
            break;
        case SpeakReason.EnableAll:
            btVoiceAll.checked = true;
            break;
        case SpeakReason.Donation:
            btVoiceType2.checked = true;
            break;
        case SpeakReason.BalanceSpending:
            btVoiceBalanceSpending.checked = true;
            break;
        default:
            btVoiceOff.checked = true;
            break;
        }
    }
}
