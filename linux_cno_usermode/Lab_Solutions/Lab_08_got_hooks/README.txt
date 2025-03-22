Sample output from successful solution:

[student@localhost got_hooks]$ ./srv
Starting up...
Injected lib running...
Found main image base address at 0x########
Found GOT at 0x########
Debugging enabled!
Attempting to hook GOT entries...
Couldn't find symbol
Couldn't find symbol
Pfffttt... I ain't the real debuglib
I sent a message 0 times
Pfffttt... I ain't the real debuglib
Pfffttt... I ain't the real debuglib
Pfffttt... I ain't the real debuglib
Attempting to hook GOT entries...
Overwrote GOT entry at 0x########
Overwrote GOT entry at 0x########
Pfffttt... I ain't the real debuglib
Someone is sending data:
000: 5468 6973 2069 7320 7375 7065 7220 7365  This.is.super.se
010: 6372 6574 2064 6174 612e 204e 6f20 6f6e  cret.data..No.on
020: 6520 7368 6f75 6c64 2073 6565 2069 7421  e.should.see.it.
030: 0a00                                     ..

[Hooked] I sent a message 1 times
Pfffttt... I ain't the real debuglib
Pfffttt... I ain't the real debuglib
Someone is sending data:
000: 5468 6973 2069 7320 7375 7065 7220 7365  This.is.super.se
010: 6372 6574 2064 6174 612e 204e 6f20 6f6e  cret.data..No.on
020: 6520 7368 6f75 6c64 2073 6565 2069 7421  e.should.see.it.
030: 0a00                                     ..

[Hooked] I sent a message 2 times
