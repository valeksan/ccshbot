"use strict";

console.log('__POPUP IS RUNNING')

var cbEnable = false;

window.addEventListener('load', (event) => {
    const checked = JSON.parse(localStorage.getItem('cbVoiceEnable'));
    document.getElementById("cbVoiceEnable").checked = checked;
    document.getElementById("popup").style.display = "block";
});

document.getElementById("cbVoiceEnable").addEventListener("click", () => {
    const checkbox = document.getElementById('cbVoiceEnable');
    let _checked = checkbox.checked;
    chrome.storage.sync.set({"cbVoiceEnable":_checked});
    localStorage.setItem('cbVoiceEnable', _checked);
    chrome.runtime.sendMessage( { type:"worker", state: _checked } )
});

// chrome.runtime.onMessage.addListener (
//     function(request, sender, sendResponse) {
//         console.log(sender.tab ?
//                   "from a content script:" + sender.tab.url :
//                   "from the extension");
//         switch(request.type) {
//             case "getDataCC":
//                 alert("getDataCC");
//                 break;
//         }
//     }
// );


