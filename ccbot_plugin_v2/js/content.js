"use strict";
console.log('__CONTENT PLUGIN IS RUNNING')

const DEF_TIMEOUT_REPEAT = 1500;
const DEF_HOST = 'localhost';
const DEF_PORT = 3000;

const chatVoiceEnableCfgObj = localStorage.getItem('ChatVoiceEnableCfg')
let chatVoiceEnableCfg;

if (chatVoiceEnableCfgObj)
    chatVoiceEnableCfg = JSON.parse(chatVoiceEnableCfgObj);
else
    chatVoiceEnableCfg = new Array();

console.log("__chatVoiceEnableCfg", chatVoiceEnableCfg);
let chatVoiceEnable = false;
let flagConnection = false;

let cfgTimeoutRepeat = DEF_TIMEOUT_REPEAT;
let cfgHost = DEF_HOST;
let cfgPort = DEF_PORT;

let server = null;

let debug = true;

//debugSetCfg();
initCfg();

function trim(x) {
    return String(x).replace(/^\s+|\s+$/gm,'');
}

const sleep = ms => {
    return new Promise(resolve => {
        setTimeout(() => resolve(), ms);
    });
};

function convertNikColorFormat(text) {
    const args = text.split(',');
    if(args.length < 3) 
        return "";
    const r_hex = parseInt(args[0]).toString(16).padStart(2, '0');
    const g_hex = parseInt(args[1]).toString(16).padStart(2, '0');
    const b_hex = parseInt(args[2]).toString(16).padStart(2, '0');
    return `#${r_hex}${g_hex}${b_hex}`;
}

function sendMessageToChat(text) {
    const input = document.querySelector("#app > div.viewport > div.content > div > div > div > div.stream__layout__left.stream__layout__left--no-padding > div.stream__layout__chat.stream__layout__chat--rtl > div > form > div > div > input");
    const submit = document.querySelector("#app > div.viewport > div.content > div > div > div > div.stream__layout__left.stream__layout__left--no-padding > div.stream__layout__chat.stream__layout__chat--rtl > div > form > button");
    input.value = text;
    
    input.dispatchEvent(new Event('input', { bubbles: true }));

    sleep(200).then(() => {
        submit.click();
    })
}

function processingMessagesFromServer(message) {
    try {
        switch(message.type) {
        case "message":
            sendMessageToChat(message.text);
            break;
        }
    } catch(e) {
        console.log(e);
    }
}

async function connect(host, port) {
    return new Promise(function(resolve, reject) {

        server = new WebSocket(`ws://${host}:${port}`);

        server.onopen = function() {
            flagConnection = true;
            ledConnect.style = "color:green";
            ledConnect.innerHTML = "Online";
            resolve(server);
        };

        server.onerror = function(err) {
            reject(err);
        };

        server.onclose = function(evt) {
            flagConnection = false;
            //server = null;
            ledConnect.style = "color:red";
            ledConnect.innerHTML = "Offline";
        };

        server.onmessage = function(event) {
            const message = JSON.parse(event.data);
            console.log("__send_msg", message);
            processingMessagesFromServer(message);
        }
    });
}

function disconnectBot() {
    if(flagConnection) {
        try {
            server.close();
            //server = null;
        } catch(e) {
            //server = null;
        }
        flagConnection = false;
        ledConnect.style = "color:red";
        ledConnect.innerHTML = "Offline";
    }
}

function debugSetCfg() {
    chrome.storage.local.set({"CfgTimeoutRepeat": DEF_TIMEOUT_REPEAT});
    chrome.storage.local.set({"CfgHost": DEF_HOST});
    chrome.storage.local.set({"CfgPort": DEF_PORT});
}

function initCfg() {
    chrome.storage.local.get(['CfgTimeoutRepeat','CfgHost','CfgPort'], function(item) {
        if (chrome.runtime.lastError) {
            console.error(chrome.runtime.lastError.message);
            reject(chrome.runtime.lastError.message);
        } else {
            console.log("__get", item)
            resolve(item);
        }
    });
}

function clickBtChatVoice() {
    //chatVoiceEnable = !chatVoiceEnable;

    const currentPageUrl = window.location.href;
    const selectorStreamId = RegExp(/https:\/\/crazycash.tv\/view-stream\/(\d{1,})/).exec(currentPageUrl);
    const streamId = ((selectorStreamId !== null) ? String(selectorStreamId[1]) : "");
    const flagIsCorrectPage = (streamId.length > 0);

    if (!flagIsCorrectPage)
        return;

    let chatVoiceEnableFountIndex = -1;
    for (let i = 0; i < chatVoiceEnableCfg.length; i++) {
        if (chatVoiceEnableCfg[i]["id"] === streamId) {
            chatVoiceEnableFountIndex = i;
            chatVoiceEnable = chatVoiceEnableCfg[i]["chatVoiceEnable"];
            chatVoiceEnable = !chatVoiceEnable;
            chatVoiceEnableCfg[i]["chatVoiceEnable"] = chatVoiceEnable;
            break;
        }
    }
    if (chatVoiceEnableFountIndex === -1) {
        chatVoiceEnable = true;
        const btChatVoiceObj = {"id":streamId,"chatVoiceEnable":chatVoiceEnable};
        chatVoiceEnableCfg.push(btChatVoiceObj);
    }

    localStorage.setItem('ChatVoiceEnable', chatVoiceEnableCfg);
    if (chatVoiceEnable) {
        doRepeatGetDataCC();
        buttonChatVoiceEnable.className = buttonChatVoiceEnable.className.replace(/\bbtn--light-gray\b/g, "btn--yellow");
    } else {
        buttonChatVoiceEnable.className = buttonChatVoiceEnable.className.replace(/\bbtn--yellow\b/g, "btn--light-gray");
        if (flagConnection)
            disconnectBot();
    }
}

const logo = document.querySelector("#app > div.viewport > div.top-bar.top-bar--authorized > div.top-bar__logo");

const classBtSpeekOn = chatVoiceEnable ? "btn--yellow" : "btn--light-gray";
const classLedConnect = flagConnection ? "led-green" : "led-red";

logo.innerHTML = `
<!--<div style="overflow-x:auto;">-->
<table style="border-spacing:0;border:0px;width:150px;">
    <tr>
        <td colspan=3><div class="top-bar__logo"><a href="/" style="display:block;"><img src="https://crazycash.tv/img/logo.59ce0fd5.png" height="32px"></a></div></td></tr>
    <tr>
        <td style="padding:5px;width:40px;"><button id="btSpeekOn" style="height:24px;width:30px;" data-v-1ce9bacd="" type="button" class="btn withdraw__tab btn--rounded ${classBtSpeekOn} btn--md hoverable"><div data-v-1ce9bacd="" class="btn__inner">🗣</div></button></td>
        <td style="padding:5px;width:40px;"><span id="ledConnect" style="color:red">Offline</span></td>
        <td></td>
    </tr>
</table>
<!--</div>-->`

const buttonChatVoiceEnable = document.getElementById("btSpeekOn");
const ledConnect = document.getElementById("ledConnect");

buttonChatVoiceEnable.addEventListener('click', clickBtChatVoice, false);

async function doRepeatGetDataCC() {
    if (chatVoiceEnable) {
        console.log('__new_timeout_start:')
        if (flagConnection) {
            const chatData = getChatContentCC();
            server.send(JSON.stringify(chatData));
        } else {
            if (debug) 
                getChatContentCC();
            await connect(cfgHost, cfgPort)
            .then(result => {
                console.log('__connection_ok', result);
            })
            .catch((err) => {
                console.log('__connection_err', err);
            })
        }
        setTimeout(doRepeatGetDataCC, cfgTimeoutRepeat);
    } else {
        disconnectBot();
        console.error('__new_timeout_start_fail:')
    }
}

function getChatContentCC() {
    const currentPageUrl = window.location.href;
    const selectorStreamId = RegExp(/https:\/\/crazycash.tv\/view-stream\/(\d{1,})/).exec(currentPageUrl);
    const streamId = ((selectorStreamId !== null) ? String(selectorStreamId[1]) : "");
    const flagIsCorrectPage = (streamId.length > 0);

    console.log("__streamId", streamId);
    
    if (flagIsCorrectPage) {
        const chatDomArray = document.querySelectorAll('div.msg') || [];

        const streamerNameDomElement = document.querySelector("div.stream__header__info__user__nickname");

        //console.log('__streamerNameDomElement:', streamerNameDomElement.innerText)
        
        if (chatDomArray.length > 0) {
            //... Parse ...
            const chatObjArray = Array.from(chatDomArray).map(obj => {
                let type;
                let sender;
                let msgFull;
                let msgHtml;
                let msg;
                let pay;
                let nikStyle = "";
                let isBan = true;

                try {
                    msgFull = obj.innerText || "";
                } catch(e) { 
                    msgFull = ""; 
                    console.log("__err_msg_full_init", e);
                }

                try {
                    type = parseInt(obj.classList[1].split('_')[1]);
                } catch(e) { 
                    type = -1; 
                    console.log("__err_type_init", e); 
                }

                try {
                    if (type === 4) {
                        msg = msgFull;
                    } else if (type === 2) {
                        msg = msgFull.substr(msgFull.indexOf("!") + 2);
                    } else {
                        msg = msgFull.substr(msgFull.indexOf(':') + 2);
                        if (type === 3) {
                            let values = RegExp(/\s(\d{1,})\s/).exec(msg);
                            if(values.length > 0) {
                                isBan = false;
                                type = 1;
                            } 
                        }
                    }
                } catch(e) { 
                    msg = ""; 
                    console.log("__err_msg_init", e);
                }

                try {
                    if (type === 3 && isBan) {
                        sender = msgFull.substr(msgFull.indexOf(':') + 2).split(' ')[0];
                    } else {
                        sender = obj.firstChild.innerText.split(':')[0] || "";
                    }
                } catch(e) { 
                    sender = ""; 
                    console.log("__err_sender_init", e); 
                }

                try {
                    msgHtml = String(obj.children[1].innerHTML);
                } catch(e) { 
                    msgHtml = ""; 
                    console.log("__err_msg_html_init", e);
                }

                try {
                    if (type === 2) {
                        pay = parseFloat(parseFloat(RegExp(/\$(\d{1,}\.{0,1}\d{0,2})!/u).exec(msgHtml)[1]).toFixed(2));
                    } else {
                        pay = 0.0;
                    }
                } catch(e) { 
                    pay = 0.0; 
                    console.log("__err_pay_init", e);
                }

                try {
                    if (sender === "moderator") {
                        nikStyle = "#ff0000";
                    } else if(type === 1) {
                        nikStyle = "#fff200";
                    } else {
                        nikStyle = RegExp(/style=\"color:\s{0,1}rgb\((\d{1,3},\s{0,1}\d{1,3},\s{0,1}\d{1,3})\);\"/).exec(obj.innerHTML)[1];
                        nikStyle = convertNikColorFormat(nikStyle.replace(/\s/g,''));
                    } 
                } catch(e) { 
                    nikStyle = ""; 
                    console.log("__err_nikstyle_init", e);
                }

                return { 
                    "type": type,
                    "sender": sender,
                    "nik_color": nikStyle,
                    "msg": msg,
                    "pay": pay
                }
            });
            //console.log('__CHAT_OBJ_ARR:', chatObjArray);

            // Pack to Set
            let lastTimestamp = Math.floor(Date.now() / 1000);
            let streamerNameValue = streamerNameDomElement.innerText;

            let sendData = {
                type: "chat_datagram",
                streamId: streamId,
                timestamp: lastTimestamp,
                streamerName: streamerNameValue,
                messages: chatObjArray
            };

            console.log("__chat_datagram", sendData);
            
            // ... End Parse

            return sendData;
        }
    }
    return { type: "empty" };
}

if (chatVoiceEnable) {
    doRepeatGetDataCC();
}