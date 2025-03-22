This is a piece of obfuscated code that can be cleared up with a few Ghidra scripts.

Note:
	Don't give out the source or the build steps for this piece of code.
	I used a piece from a CNO Usermode lab solution to xor the strings.

Obfuscations:

xor strings
	The script should track arguments passed to the dexor function, and handle those

self modifying code
	The script should be pointed at the self modifying region, and then change it
	It is just bswapping it's contents 8 bytes at a time	

General Overview
Badwhere is going to make copies of itself, the copies will have names like "~/.i/am/a/goodguy" defined in BINNAME0-5. Each one will find a random open terminal and start putting out colors to it.
You can fix it by running pkill badwhere.

It has a few xor-encrypted string, and also has a section where it self-modifies to do a bswap. These steps are visible in the makefile, using xorstring_preprocess.py and bswapsection.py

The scripts fixbswap.py and fixxor.py can be added to ghidra_scripts directory and run.

Run the fixbswaps script at the section causeloops. You will have to select the whole function, right click->clear bytes. Then run the script on that section, then disassembly again to see the proper bswap.

Run the xorstring section at any spot that has an xored string. The key is "ACTP"

I would make this lab a demo instead of having students spend time looking through ghidra scripting, there aren't any questions about it on the test and it is difficult to make significant progress in the amount of time given.
