import json
import urllib

import time
import hashlib
import hmac
import base64

import connection

class API(object):

    def __init__(self, keyFile):
        self.load_key(keyFile)
        self.uri = 'https://test.deribit.com/api'
        self.apiversion = 'v1'
        self.conn = None

    def load_key(self, path):
        f = open(path, "r")
        self.key = f.readline().strip()
        self.secret = f.readline().strip()

    def _query(self, urlpath, req = {}, headers = {}):
        url = self.uri + urlpath

        if self.conn is None:
            print "Establishing a new connection"
            self.conn = connection.Connection("test.deribit.com", timeout=3600) # socket opened for 1 hour

        print "URL=%s"%url
        ret = self.conn._request(url, req, headers)
        return json.loads(ret)
    
    def query_public(self, method, req = {}):
        urlpath = '/' + self.apiversion + '/public/' + method

        return self._query(urlpath, req)
    
    def query_private(self, method, req={}):
        urlpath = '/' + self.apiversion + '/private/' + method

#        req['nonce'] = int(1000*time.time())
#        postdata = urllib.urlencode(req)
#        message = urlpath + hashlib.sha256(str(req['nonce']) +
#                                           postdata).digest()
#        signature = hmac.new(base64.b64decode(self.secret),
#                             message, hashlib.sha512)
        nonce = str(int(1000*time.time()))
        header = "_=%s&_ackey=%s&_acsec=%s&_action=%s" % (nonce, self.key, self.secret, urlpath)
        header = hashlib.sha256(header)
        #_=1452237485895&_ackey=29mtdvvqV56&_acsec=BP2FEOFJLFENIYFBJI7PYWGFNPZOTRCE&_action=/api/v1/private/buy&instrument=BTC-15JAN16&price=500&quantity=1
        header = base64.b64encode(str(header))
        header = self.key + "." + nonce + "." + header
        print header

        headers = {
            'x-deribit-sig': header
        }

        return self._query(urlpath, req, headers)
