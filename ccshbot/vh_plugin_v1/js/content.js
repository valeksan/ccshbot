"use strict";

/*!
* My plugin
*/

console.log('__CONTENT PLUGIN IS RUNNING')

const DEF_TIMEOUT_REPEAT = 1500;
const DEF_HOST = 'localhost';
const DEF_PORT = 3000;

let chatVoiceEnable = false;
let flagConnection = false;
let flagFullscrBtInserted = false;

let cfgTimeoutRepeat = DEF_TIMEOUT_REPEAT;
let cfgHost = DEF_HOST;
let cfgPort = DEF_PORT;

let server = null;

let debug = true;

initCfg();

function trim(x) {
    return String(x).replace(/^\s+|\s+$/gm,'');
}
function trim2(str) {
    return str.replace(/\s+/g, ' ').trim();
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

async function connect(host, port) {
    return new Promise(function(resolve, reject) {

        server = new WebSocket(`wss://${host}:${port}`);

        server.onopen = function() {
            flagConnection = true;
            //ledConnect.style = "color:green";
            //ledConnect.innerHTML = "Online";
            resolve(server);
        };

        server.onerror = function(err) {
            reject(err);
        };

        server.onclose = function(evt) {
            flagConnection = false;
            //ledConnect.style = "color:red";
            //ledConnect.innerHTML = "Offline";
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
        } catch(e) {
            console.log("__fail_close_server", e);
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
    const currentPageUrl = window.location.href;
    const selectorStreamId = RegExp(/https:\/\/viewhub.cc\/stream\/(\w{1,})/).exec(currentPageUrl);
    const streamId = ((selectorStreamId !== null) ? String(selectorStreamId[1]) : "");
    const flagIsCorrectPage = (streamId.length > 0);

    if (!flagIsCorrectPage)
        return;
    
    chatVoiceEnable = !chatVoiceEnable;

    if (chatVoiceEnable) {
        doRepeatGetDataCC();
        //buttonChatVoiceEnable.className = buttonChatVoiceEnable.className.replace(/\bbtn--light-gray\b/g, "btn--yellow");
        buttonChatVoiceEnable.setAttribute("style", "height: 35px; max-width: 170px;  background-color:green");
    } else {
        //buttonChatVoiceEnable.className = buttonChatVoiceEnable.className.replace(/\bbtn--yellow\b/g, "btn--light-gray");
        buttonChatVoiceEnable.setAttribute("style", "height: 35px; max-width: 170px;  background-color:transparent");
        if (flagConnection)
            disconnectBot();
    }
}

function insertAfter(referenceNode, newNode) {
    referenceNode.parentNode.insertBefore(newNode, referenceNode.nextSibling);
}

var toolbar;
var toolbarSpacer;

console.log("____TEST", toolbarSpacer)

var colorBtSpeekOn;

// .. insert button
var buttonSpeekOn;
// var buttonSpeekOn = document.createElement("span");
// buttonSpeekOn.setAttribute("data-v-1f7428c2", "");
// buttonSpeekOn.setAttribute("class","v-bage v-bage--left v-bage--overlap theme--dark");
// buttonSpeekOn.innerHTML = `<button id="btSpeekOn" data-v-1f7428c2="" type="button" class="text-capitalize v-btn v-btn--outlined theme--dark v-size--x-large" style="height: 35px; max-width: 170px;  background-color:${colorBtSpeekOn}">🗣</button>`;
//insertAfter(toolbarSpacer, buttonSpeekOn);

var buttonChatVoiceEnable;
// const buttonChatVoiceEnable = document.getElementById("btSpeekOn");
// const ledConnect = document.getElementById("ledConnect");

// buttonChatVoiceEnable.addEventListener('click', clickBtChatVoice, false);

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
    const selectorStreamId = RegExp(/https:\/\/viewhub.cc\/stream\/(\w{1,})/).exec(currentPageUrl);
    const streamId = ((selectorStreamId !== null) ? String(selectorStreamId[1]) : "");
    const flagIsCorrectPage = (streamId.length > 0);

    //console.log("__streamId", streamId);
    
    if (flagIsCorrectPage) {
        const chatDomArray = document.querySelectorAll('#chat_panel > div.v-navigation-drawer__content > div') || [];
        //const streamerNameDomElement = document.querySelector("div.stream__header__info__user__nickname");
        const streamerName = streamId;

        // if (debug) {
        //     console.log("DBG_CHAT_MSGS",chatDomArray);
        // }
        
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
                    const resultMsg = document.evaluate("div/div/div/div/span[2]/text()", obj, null, XPathResult.STRING_TYPE, null).stringValue; 
                    //console.log("TMP_MSG", trim(resultMsg))
                    msgFull = resultMsg || "";
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
                    if (type === 4) {
                        msgHtml = String(obj.children[0].innerHTML);
                    } else {
                        msgHtml = String(obj.children[1].innerHTML);
                    }
                } catch(e) { 
                    msgHtml = ""; 
                    console.log("__err_msg_html_init", e);
                }

                try {
                    if (type === 3 && isBan) {
                        sender = msgFull.substr(msgFull.indexOf(':') + 2).split(' ')[0].replace(/\s+/g, '');
                    } else if (type === 4) {
                        sender = RegExp(/>(\w{0,})</).exec(msgHtml)[1];
                    } else {
                        sender = obj.firstChild.innerText.split(':')[0].replace(/\s+/g, '') || "";
                    }
                } catch(e) { 
                    sender = ""; 
                    console.log("__err_sender_init", e); 
                }

                try {
                    // <span data-v-2b3ae6f3="" class="amber--text"> отправил донат 1$ </span>
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
                    } else if(type === 4) {
                        nikStyle = "";
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

            // Pack to Set
            let lastTimestamp = Math.floor(Date.now() / 1000);
            let streamerNameValue = streamerName;

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

function run() {
    console.log('Run script');
    // ... logic that needs DOM

    toolbar = document.querySelector("#app > div.v-application--wrap > header > div");
    toolbarSpacer = document.querySelector("#app>div.v-application--wrap>header>div>div.spacer");

    console.log("____TEST", toolbarSpacer)

    colorBtSpeekOn = chatVoiceEnable ? "green" : "transparent";

    buttonSpeekOn = document.createElement("span");
    buttonSpeekOn.setAttribute("data-v-1f7428c2", "");
    buttonSpeekOn.setAttribute("class","v-bage v-bage--left v-bage--overlap theme--dark");
    buttonSpeekOn.innerHTML = `<button id="btSpeekOn" data-v-1f7428c2="" type="button" class="text-capitalize v-btn v-btn--outlined theme--dark v-size--x-large" style="height: 35px; max-width: 170px;  background-color:${colorBtSpeekOn}">🗣</button>`;
    insertAfter(toolbarSpacer, buttonSpeekOn);

    buttonChatVoiceEnable = document.getElementById("btSpeekOn");
    buttonChatVoiceEnable.addEventListener('click', clickBtChatVoice, false);

    if (chatVoiceEnable) {
        doRepeatGetDataCC();
    }
}

console.log('The DOM is loaded');

//document.addEventListener("DOMContentLoaded", run);
sleep(2000).then(run);