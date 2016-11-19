from api import API

d = API("key.txt")

d.send_public("getinstruments")
for instr in sorted(d.receive()["result"]):
    print instr
