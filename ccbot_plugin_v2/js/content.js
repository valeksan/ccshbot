"use strict";

let chatVoiceEnable = JSON.parse(localStorage.getItem('ChatVoiceEnable'));

function clickBtChatVoice() {
    chatVoiceEnable = !chatVoiceEnable;
    localStorage.setItem('cbVoiceEnable', chatVoiceEnable);
    if (chatVoiceEnable) {
        buttonChatVoiceEnable.className = buttonChatVoiceEnable.className.replace(/\bbtn--light-gray\b/g, "btn--yellow");
    } else {
        buttonChatVoiceEnable.className = buttonChatVoiceEnable.className.replace(/\bbtn--yellow\b/g, "btn--light-gray");
    }
}

const logo = document.querySelector("#app > div.viewport > div.top-bar.top-bar--authorized > div.top-bar__logo");

logo.innerHTML = `
<div style="overflow-x:auto;">
<table style="border-spacing:0;border:0px">
    <tr><td><div class="top-bar__logo"><a href="/" style="display:block;"><img src="https://crazycash.tv/img/logo.59ce0fd5.png" height="32px"></a></div></td></tr>
    <tr><td style="padding:5px;"><button id="btSpeekOn" style="height:24px;" data-v-1ce9bacd="" type="button" class="btn withdraw__tab btn--rounded btn--light-gray btn--md hoverable"><div data-v-1ce9bacd="" class="btn__inner">🗣</div></button></td></tr>
</table>
</div>`

const buttonChatVoiceEnable = document.getElementById("btSpeekOn");

buttonChatVoiceEnable.addEventListener('click', clickBtChatVoice, false);