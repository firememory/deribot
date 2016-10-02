from connection import Connection
from api import API

d = API("key.txt")

instrs = d.query_public("getinstruments")["result"]
#for instr in sorted(instrs):
#    print "%s\t%s" % (instr["instrumentName"], instr["kind"])
#d.load_key('kraken.key')

print d.query_private("account")
