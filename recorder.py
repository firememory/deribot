from api import API
from optparse import OptionParser
from time import sleep, time
#from sys.stdout import flush
import sys

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
        print "update=%s"%update
        instr = update["result"]["instrument"]
        if instr not in self.book.keys():
            self.book[instr] = {}
        recvTime = time() * 100
        for bidUpdate in update["result"]["bids"]:
            prcUpdate = bidUpdate["price"]
            qtyUpdate = bidUpdate["quantity"]
            if prcUpdate not in self.book[instr].keys() or self.book[instr][prcUpdate] != qtyUpdate:
                self.book[instr][prcUpdate] = qtyUpdate
#                print instr, "B", prcUpdate, qtyUpdate
                self.fout.write("%s;%s;%s;B;%s;%s\n" % (ts, recvTime, instr, prcUpdate, qtyUpdate))
        for askUpdate in update["result"]["asks"]:
            prcUpdate = askUpdate["price"]
            qtyUpdate = askUpdate["quantity"]
            if prcUpdate not in self.book[instr].keys() or self.book[instr][prcUpdate] != qtyUpdate:
                self.book[instr][prcUpdate] = qtyUpdate
#                print instr, "S", prcUpdate, qtyUpdate
                self.fout.write("%s;%s;%s;S;%s;%s\n" % (ts, recvTime, instr, prcUpdate, qtyUpdate))
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

    def start(self, instr):
        self.fout = open("/home/renaud/quotes/deribit.csv", "w+")
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
