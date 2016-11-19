import websocket
import urllib
import json
import time
import hashlib
import base64
import hmac
import os
import ssl

### TODO ###
# options, --test, --ping etc...
# log info/error/debug
# options avec timestamps
# factoriser les options send

class API(object):

    def __init__(self, keyFile):
        #websocket.enableTrace(True)
        self.ws_ = websocket.WebSocket(sslopt={"cert_reqs": ssl.CERT_NONE})
        #self.ws_.connect("wss://test.deribit.com/ws/api/v1/")
        self.ws_.connect("wss://www.deribit.com/ws/api/v1/")
        self.load_key(keyFile)
        self.id_ = 0

    def ping(self):
        print ("==========")
        print ("= PING ? =")
        print ("==========")
        t1 = time.time()
        self.send_public("ping")
        res = api.receive()
        t2 = time.time()
        if "result" in res and res["result"] == "pong":
            print ("==========")
            print ("= PONG ! =")
            print ("==========")
            print ("It took %.0fms rtt" % ((t2 - t1) * 1000))
        else:
            print ("ERROR:")
            print (res)

    def load_key(self, path):
        f = open(path, "r")
        self.key_ = f.readline().strip()
        self.secret_ = f.readline().strip()
        f.close()

    def send_public(self, action):
        req = { "id"    : self.id_,
                "action": "/api/v1/public/" + action,
            }
        req = json.dumps(req)
        self.ws_.send(req)
        self.id_ += 1

    def encode_signature(self, action):
        nonce = str(int(time.time() * 1000))
        sig = "_=%s&_ackey=%s&_acsec=%s&_action=%s" % (nonce, self.key_, self.secret_, action)
        h = hashlib.new("sha256", sig)
        sig = h.digest()
        sig = base64.b64encode(sig)
        return "%s.%s.%s" % (self.key_, nonce, sig)

    def send_private(self, action):
        signature = self.encode_signature("/api/v1/private/" + action)
        req = { "id"            : self.id_,
                "action"        : "/api/v1/private/" + action,
                "arguments"     : {},
                "sig"           : signature,
            }
        req = json.dumps(req, ensure_ascii=False)
        self.ws_.send(req)
        self.id_ += 1

    def receive(self):
        result = self.ws_.recv()
        return json.loads(result)

    def close(self):
        self.ws_.close()

if __name__ == "__main__":
    api = API("key.txt")
    
    api.ping()

    api.send_private("account")
    print (api.receive())

    api.close()
