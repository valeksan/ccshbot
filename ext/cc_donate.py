# coding=utf-8

import json
import re
import requests
from urllib.parse import unquote

import pyttsx3
engine = pyttsx3.init()

ru_voice_id = "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Speech\Voices\Tokens\TTS_MS_RU-RU_IRINA_11.0"
engine.setProperty('voice', ru_voice_id)

from websocket import create_connection

API_CC = "https://api.crazycash.tv/token"
API_ABLY = "https://solarcomms-rest.ably.io/keys/{0}/requestToken"
CHAT_URL = "wss://solarcomms-realtime.ably.io/?access_token={0}&format=json&echo=false&heartbeats=true&v=1.2&lib=js-web-1.2.4"
_url_user_re = re.compile(r"https?://(\w+\.)?crazycash\.tv/(?P<user_id>\d+)")
_url_stream_re = re.compile(r"https?://(\w+\.)?crazycash\.tv/view-stream/(?P<stream_id>\d+)")

json_token =requests.get(API_CC)
token = json.loads(json_token.text)
print(token["clientId"])

headers = {"User-Agent": 'Mozilla/5.0 (iPhone; CPU iPhone OS 10_3 like Mac OS X) AppleWebKitdc/602.1.50 (KHTML, like Gecko) CriOS/56.0.2924.75 Mobile/14E5239e Safari/602.1'}
ably_token = requests.post(API_ABLY.format(token["keyName"]), headers=headers, json=token)
ably_token = json.loads(ably_token.text)
print(ably_token["token"])

join_req = {"action": 10, "channel": "streams:173991"}

ws = create_connection(CHAT_URL.format(ably_token["token"]))


try:
    while True:
        r_msg = ws.recv()
        if not r_msg:
            break
        res = json.loads(r_msg)
        action = res.get("action")
        if action == 4:
            ws.send(json.dumps({"action": 10, "channel": "chat.out:173991", "msgSerial": 0}))
        elif action == 15:
            msg = res.get("messages")[0].get("data")
            if len(msg) > 0:
                msg = json.loads(msg)
                msg = msg.get("msg")
                print("{sender}: {text}".format(sender=msg.get("sender"),text=unquote(msg.get("text"))))
                if msg.get("flag") == 2:
                    engine.say("{sender} задонатил {amount} баксов. {phrase}".format(phrase=unquote(msg.get("payload").get('raw_text')),
                                                                            amount=msg.get("payload").get('amount'),
                                                                            sender=msg.get("sender")))

                engine.runAndWait()

except Exception as e:
    print(e)
finally:
    ws.close()