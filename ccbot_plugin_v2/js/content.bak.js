"use strict";

function trim(x) {
    return String(x).replace(/^\s+|\s+$/gm,'');
}

function convertNikColorFormat(text) {
    const args = text.split(',');
    if(args.length < 3) 
        return "";
    const r_hex = parseInt(args[0]).toString(16).padStart(2, '0');
    const g_hex = parseInt(args[1]).toString(16).padStart(2, '0');
    const b_hex = parseInt(args[2]).toString(16).padStart(2, '0');
    return `#${r_hex}${g_hex}${b_hex}`;
}

function parseChat() {
    
    const currentPageUrl = window.location.href;

    const selectorStreamId = RegExp(/https:\/\/crazycash.tv\/view-stream\/(\d{1,})/).exec(currentPageUrl);
    
    const streamId = ((selectorStreamId !== null) ? String(selectorStreamId[1]) : "");
    console.log("__streamId", streamId)

    const flagIsCorrectPage = (streamId.length > 0);
    
    if(flagIsCorrectPage) {

        const chatDomArray = document.querySelectorAll('div.msg') || []
        //console.log('__CHAT_DOM_ARR:', chatDomArray)
        
        if(chatDomArray.length > 0) {
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
                } catch(e) { msgFull = ""; console.log("__err_msg_full_init", e)}

                try {
                    type = parseInt(obj.classList[1].split('_')[1])
                } catch(e) { type = -1; console.log("__err_type_init", e) }

                try {
                    if(type === 4) {
                        msg = msgFull;
                    } else if(type === 2) {
                        msg = msgFull.substr(msgFull.indexOf("!") + 2);
                    } else {
                        msg = msgFull.substr(msgFull.indexOf(':') + 2);
                        if(type === 3) {
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
                    if(type === 3 && isBan) {
                        sender = msgFull.substr(msgFull.indexOf(':') + 2).split(' ')[0]
                    } else {
                        sender = obj.firstChild.innerText.split(':')[0] || ""
                    }
                } catch(e) { sender = ""; console.log("__err_sender_init", e) }

                try {
                    msgHtml = String(obj.children[1].innerHTML);
                } catch(e) { msgHtml = ""; console.log("__err_msg_html_init", e)}

                try {
                    if(type === 2) {
                        pay = parseFloat(parseFloat(RegExp(/\$(\d{1,}\.{0,1}\d{0,2})!/u).exec(msgHtml)[1]).toFixed(2));
                    } else {
                        pay = 0.0
                    }
                } catch(e) { pay = 0.0; console.log("__err_pay_init", e)}

                try {
                    if(sender === "moderator") {
                        nikStyle = "#ff0000";
                    } else if(type === 1) {
                        nikStyle = "#fff200";
                    } else {
                        nikStyle = RegExp(/style=\"color:\s{0,1}rgb\((\d{1,3},\s{0,1}\d{1,3},\s{0,1}\d{1,3})\);\"/).exec(obj.innerHTML)[1];
                        nikStyle = convertNikColorFormat(nikStyle.replace(/\s/g,''));
                    } 
                } catch(e) { nikStyle = ""; console.log("__err_nikstyle_init", e)}

                return { 
                    "type": type,
                    "sender": sender,
                    "nik_color": nikStyle,
                    "msg": msg,
                    "pay": pay
                }
            });
            console.log('__CHAT_OBJ_ARR:', chatObjArray)

            // Pack to Set
            let lastTimestamp = Math.floor(Date.now() / 1000);

            let sendData = {
                type: "chat_datagram",
                streamId: streamId,
                timestamp: lastTimestamp,
                messages: chatObjArray
            }

            console.log("__chat_datagram", sendData)

            //if(sendData.messages.length > 0)
            //chrome.runtime.sendMessage( { type:"sendChatToServer", data: JSON.stringify(sendData) } );
            
            // ... End Parse

            return sendData;
        }

    }
}

function initScript() {
    chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
        switch(message.type) {
            case "getDataCC":
                console.log("__message: ", message);
                let sendData = parseChat();
                sendResponse( { data:JSON.stringify(sendData), empty: (sendData.messages.length === 0) } );
                return true;
            default:
                console.error("__Unrecognised message: ", message);
        }
        return false;
    });
}

const sleep = ms => {
    return new Promise(resolve => {
        setTimeout(() => resolve(), ms);
    });
};

if( document.readyState !== 'loading' ) {
    console.log( 'document is already ready, just execute code here' );
    sleep(4000).then(() => initScript())
} else {
    document.addEventListener('DOMContentLoaded', function () {
        console.log( 'document was not ready, place code here' );
        sleep(4000).then(() => initScript())
    });
}