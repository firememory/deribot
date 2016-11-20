from api import API
from optparse import OptionParser
from time import sleep

class Recorder(object):
    def __init__(self):
        self.api = API("key.txt")
        self.run = True
        self.book = {}

    def listAll(self):
        self.api.send_public("getinstruments")
        res = self.api.receive()
        for instr in res["result"]:
            print "%s\t%s" % (instr["instrumentName"], instr["kind"])

    def writeUpdate(self, ts, update):
        instr = update["result"]["instrument"]
        if instr not in self.book.keys():
            self.book[instr] = {}
        for bidUpdate in update["result"]["bids"]:
            prcUpdate = bidUpdate["price"]
            qtyUpdate = bidUpdate["quantity"]
            if prcUpdate not in self.book[instr].keys() or self.book[instr][prcUpdate] != qtyUpdate:
                self.book[instr][prcUpdate] = qtyUpdate
#                print instr, "B", prcUpdate, qtyUpdate
                self.fout.write("%s;%s;B;%s;%s\n" % (ts, instr, prcUpdate, qtyUpdate))
        for askUpdate in update["result"]["asks"]:
            prcUpdate = askUpdate["price"]
            qtyUpdate = askUpdate["quantity"]
            if prcUpdate not in self.book[instr].keys() or self.book[instr][prcUpdate] != qtyUpdate:
                self.book[instr][prcUpdate] = qtyUpdate
#                print instr, "S", prcUpdate, qtyUpdate
                self.fout.write("%s;%s;S;%s;%s\n" % (ts, instr, prcUpdate, qtyUpdate))

    def start(self, instr):
        print "Starting to record %s" % instr
        self.fout = open("/home/renaud/quotes/deribit.csv", "w")
        self.api.send_private("subscribe", {"instrument": [instr], "event": ["order_book", "trade", "user_order"]})
#        print self.api.receive()
        while self.run:
            res = self.api.receive()
            if "notifications" in res.keys():
                for update in res["notifications"]:
                    self.writeUpdate(res["msOut"], update)
        self.api.close()
        self.fout.close()

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-i", "--instr",   action="store",      help="Instrument to record")
    parser.add_option("-l", "--listall", action="store_true", help="List all instruments")
    (options, args) = parser.parse_args()
    if not options.instr and not options.listall:
        parser.error("--instr or --listall needed")

    r = Recorder()

    if options.listall:
        r.listAll()
    else:
        r.start(options.instr)
