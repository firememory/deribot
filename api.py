import websocket
import urllib
import json
import time
import hashlib
import base64
import hmac
import os
import ssl
import threading
from time import sleep

### TODO ###
# options, --test, --ping etc...
# log info/error/debug
# options avec timestamps
# factoriser les options send
# getId() with a reset to 0

KEEP_ALIVE_TIMEOUT = 10

class API(object):

    def __init__(self, keyFile, callbackObject=None, verbose=False):
        print "Connecting"
        websocket.enableTrace(verbose)
        self.ws_ = websocket.WebSocketApp("wss://www.deribit.com/ws/api/v1/",
                                          on_open = self.on_open,
                                          on_message = self.on_message,
                                          on_error = self.on_error,
                                          on_close = self.on_close)
        self.load_key(keyFile)
        self.id_ = 0
        self.verbose = verbose

        if callbackObject != None: # loop mode
            print "Launching main loop"
            self.callbackObject = callbackObject
            self.isRunning = True
            self.ws_.run_forever(sslopt={"cert_reqs": ssl.CERT_NONE})
        else:
            self.ws_ = websocket.WebSocket(sslopt={"cert_reqs": ssl.CERT_NONE})
            self.ws_.connect("wss://www.deribit.com/ws/api/v1/")

    def on_open(self, ws):
        print "Starting keep alive thread"
        self.keepAliveThread = threading.Thread(target=self.keepAlive)
        self.keepAliveThread.start()

        if self.callbackObject:
            self.callbackObject.onOpen(self)

    def on_message(self, ws, message):
        message = json.loads(message)
        if self.verbose:
            print message
        if "result" in message.keys() and message["result"] == "pong":
            pongTime = time.time()
            print ("PONG ! It took %.1fms rtt" % ((pongTime - self.pingTime) * 1000))
        else:
            if self.callbackObject:
                self.callbackObject.onMessage(message)


    def on_close(self, ws):
        print "on_close called"
        self.isRunning = False
        self.keepAliveThread.join()

    def on_error(self, ws, error):
        print "ERROR: %s" % error

    def keepAlive(self):
        while self.isRunning:
            self.ping()
            for i in range(0, KEEP_ALIVE_TIMEOUT):
                if not self.isRunning:
                    break
                sleep(1)

    def ping(self):
        print ("PING ?")
        self.pingTime = time.time()
        self.send_public("ping")

    def load_key(self, path):
        f = open(path, "r")
        self.key_ = f.readline().strip()
        self.secret_ = f.readline().strip()
        f.close()

    def send_public(self, action, arguments = {}):
        req = { "id"            : self.id_,
                "action"        : "/api/v1/public/" + action,
                "arguments"     : arguments,
        }
        req = json.dumps(req)
        if self.verbose:
            print req
        self.ws_.send(req)
        self.id_ += 1

    def encode_signature(self, action, arguments):
        nonce = str(int(time.time() * 1000))
        sig = "_=%s&_ackey=%s&_acsec=%s&_action=%s" % (nonce, self.key_, self.secret_, action)
        for key in sorted(arguments.keys()):
            sig += "&%s=%s" % (key, "".join(arguments[key]))
        h = hashlib.new("sha256", sig)
        sig = h.digest()
        sig = base64.b64encode(sig)
        return "%s.%s.%s" % (self.key_, nonce, sig)

    def send_private(self, action, arguments = {}):
        signature = self.encode_signature("/api/v1/private/" + action, arguments)
        req = { "id"            : self.id_,
                "action"        : "/api/v1/private/" + action,
                "arguments"     : arguments,
                "sig"           : signature,
            }
        req = json.dumps(req, ensure_ascii=False)
        if self.verbose:
            print req
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

    print "=========="
    api.send_private("account")
    for key, value in api.receive()["result"].iteritems():
        print "%s : %s" % (key, value)

    api.close()
