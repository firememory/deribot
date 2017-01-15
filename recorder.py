from api import API
from optparse import OptionParser
from time import sleep, time
import sys
from datetime import datetime
import os

class Recorder(object):
    def __init__(self, verbose=False):
        self.verbose = verbose

    def listAll(self):
        api = API("key.txt")
        api.send_public("getinstruments")
        res = api.receive()
        for instr in res["result"]:
            print "%s\t%s" % (instr["instrumentName"], instr["kind"])

    def writeUpdate(self, ts, update):
        instr = update["result"]["instrument"]
        if instr not in self.book.keys():
            self.book[instr] = {}

        recvTime = time()
        today = datetime.fromtimestamp(recvTime).strftime("%Y%m%d")
        if today != self.today:
            self.fout = self.getFileDesc(today)
        recvTime = int(recvTime * 100)

        for bidUpdate in update["result"]["bids"]:
            prcUpdate = bidUpdate["price"]
            qtyUpdate = bidUpdate["quantity"]
            if prcUpdate not in self.book[instr].keys() or self.book[instr][prcUpdate] != qtyUpdate:
                self.book[instr][prcUpdate] = qtyUpdate
                self.fout.write("%s;%s;%s;B;%s;%s\n" % (ts, recvTime, instr, qtyUpdate, prcUpdate))

        for askUpdate in update["result"]["asks"]:
            prcUpdate = askUpdate["price"]
            qtyUpdate = askUpdate["quantity"]
            if prcUpdate not in self.book[instr].keys() or self.book[instr][prcUpdate] != qtyUpdate:
                self.book[instr][prcUpdate] = qtyUpdate
                self.fout.write("%s;%s;%s;S;%s;%s\n" % (ts, recvTime, instr, qtyUpdate, prcUpdate))

        ltp = update["result"]["last"]
        if "ltp" in self.book[instr].keys() and self.book[instr]["ltp"] != ltp:
            self.fout.write("%s;%s;%s;T;%s;%s\n" % (ts, recvTime, instr, 0, ltp)) # qty unknown ?
        self.book[instr]["ltp"] = ltp

        self.fout.flush()

    def onOpen(self, api):
        print "Subscribing to updates of %s" % self.instr
        api.send_private("subscribe", {"instrument": [self.instr], "event": ["order_book", "trade", "user_order"]})

    def onMessage(self, message):
        if "notifications" in message.keys():
            for update in message["notifications"]:
                self.writeUpdate(message["msOut"], update)
        else:
            print "ERROR: not a notification: %s" % message

    def getFileDesc(self, date=None):
        if hasattr(self, "fout"):
            self.fout.close()
        self.today = date or datetime.now().strftime("%Y%m%d")
        fileName = "%s/quotes/deribit.%s.csv" % (os.getenv("HOME"), self.today)
        print "Opening %s" % fileName
        return open(fileName, "w+")

    def start(self, instr):
        self.fout = self.getFileDesc()
        self.book = {}
        self.instr = instr
        self.api = API("key.txt", self, self.verbose)


if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-i", "--instr",   action="store",      help="Instrument to record")
    parser.add_option("-l", "--listall", action="store_true", help="List all instruments")
    parser.add_option("-v", "--verbose", action="store_true", help="Verbose mode")
    (options, args) = parser.parse_args()
    if not options.instr and not options.listall:
        parser.error("--instr or --listall needed")

    r = Recorder(options.verbose)

    if options.listall:
        r.listAll()
    else:
        r.start(options.instr)
