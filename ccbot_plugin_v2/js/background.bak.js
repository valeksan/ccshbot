"use strict";

const DEF_TIMEOUT_REPEAT = 1500;
const DEF_HOST = 'localhost';
const DEF_PORT = 3000;

console.log('__BACKGROUND PLUGIN IS RUNNING')

let server = null;

let workerEnabled = false;
let flagConnection = false;

async function connect(host, port) {
    return new Promise(function(resolve, reject) {

        server = new WebSocket(`ws://${host}:${port}`);

        server.onopen = function() {
            flagConnection = true;
            resolve(server);
        };

        server.onerror = function(err) {
            reject(err);
        };

        server.onclose = function(evt) {
            flagConnection = false;
            server = null;
        };
    });
}

function disconnect() {
    if(flagConnection) {
        try {
            server.close();
            //server = null;
        } catch(e) {
            //server = null;
        }
        flagConnection = false;
    }
}

function getCbVoiceEnable() {
    return new Promise(function(resolve, reject) {
        chrome.storage.sync.get('cbVoiceEnable', function(item) {
            if (chrome.runtime.lastError) {
                console.error(chrome.runtime.lastError.message);
                reject(chrome.runtime.lastError.message);
            } else {
                //console.log("__get", item)
                resolve(item.cbVoiceEnable);
            }
        });
    });
}

function getChatContentCC() {
    chrome.tabs.query({active: true, currentWindow: true}, function(tabs){
        chrome.tabs.sendMessage(tabs[0].id, { type: "getDataCC" }, function(response) { 
            try {
                if(!response.empty) {
                    server.send(response.data);
                }
            } catch(e) { console.log("Fail response:",e)}
        });  
    });
}

async function doRepeatGetDataCC() {
    if(workerEnabled) {
        // console.log('__new_timeout_start:')
        //alert("REPEAT ON")
        if(flagConnection) {
            //chrome.runtime.sendMessage( { type:"getDataCC" } );
            getChatContentCC();
        } else {
            connect(DEF_HOST, DEF_PORT)
            .then(result => console.log('__connection_ok', result))
            .catch((err) => {
                console.log('__connection_err', err);
            })
        }
        setTimeout(doRepeatGetDataCC, DEF_TIMEOUT_REPEAT);
    } else {
        disconnect();
        //alert("REPEAT OFF")
        // console.error('__new_timeout_start_fail:')
    }
}

chrome.runtime.onMessage.addListener (
    function(request, sender, sendResponse) {
        console.log(sender.tab ?
                  "from a content script:" + sender.tab.url :
                  "from the extension");
        switch(request.type) {
            case "worker":
                if (request.state) {
                    if(!workerEnabled) {
                        workerEnabled = true;
                        doRepeatGetDataCC();
                    }
                    //alert(request.state);
                    //..
                } else {
                    workerEnabled = false;
                    //alert(request.state);
                    disconnect();
                }
                //sendResponse( {type: "worker", state: request.state} );
                break;
            // case "sendChatToServer":
            //     //if(flagConnection) {
            //         alert(request.data)
            //         server.send(request.data);
            //     //}
            //     break;
        }

    }
);


//alert("parseChatAlarm On Start BG");
getCbVoiceEnable().then((result) => {
    workerEnabled = result;
    if(workerEnabled) {
        doRepeatGetDataCC();
    }
})





































// #######################################################################################

// chrome.runtime.onInstalled.addListener(function() {
//     chrome.declarativeContent.onPageChanged.removeRules(undefined, function() {
//       chrome.declarativeContent.onPageChanged.addRules(
//         [
//           {
//             conditions: [
//               new chrome.declarativeContent.PageStateMatcher(
//                 {
//                   pageUrl: { hostEquals: ".crazycash.tv" }
//                 }
//               )
//             ],
//             actions: [
//               new chrome.declarativeContent.ShowPageAction()
//             ]
//           }
//         ]
//       )
//     })
// });

// "page_action": {
//     "default_popup": "popup.html",
//     "default_icon": {
//         "48": "icon48i.png"
//     }
// },

// chrome.runtime.onMessage.addListener(
//   function(request, sender, sendResponse) {
//     // read `newIconPath` from request and read `tab.id` from sender
//     console.log("__test", request)
//     chrome.browserAction.setIcon({
//         path: request.path
//     });
// });


// ,
//             "run_at": "document_end"
