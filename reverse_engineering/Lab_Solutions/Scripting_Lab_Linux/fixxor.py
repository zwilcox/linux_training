# comments xor strings, given and address and a key

#@category ACTP

# put this in one of your ghidra script directories

from ghidra.framework.model import DomainFile
from ghidra.framework.model import DomainFolder
from ghidra.program.model import symbol
from ghidra.program.model.address import Address
from ghidra.program.model.lang import LanguageCompilerSpecPair
from ghidra.program.model.listing import Program
from ghidra.util import Msg

from java.lang import IllegalArgumentException

# The presence of the AskScript.properties file in the same location (as AskScript.java) 
# allows for the following behavior:
#		- GUI: if applicable, auto-populates the input field with the value in the 
#			.properties file (the first	time that input	field appears)
#   	- Headless: uses the value in the .properties file for the variable assigned to the
#			corresponding askXxx() method in the GhidraScript.
try:
    addr = currentLocation.getRefAddress()
    if addr is None:
        addr = currentLocation.getAddress()
    #addr = askAddress("String Data Address", "enter address")
    key = askString("XOR key", "enter xor key").encode("ascii")
    count = askInt("String Len", "enter xor byte count")
    #print(addr)
    #print(key)
    #print(count)
    
    real_s = ""
    s = ""
    for i in range(count):
        byte = getByte(addr.add(i))
        c = chr(byte ^ ord(key[i%len(key)]))
        
        real_s += c
        if c.isalnum():
            s += c
        else:
            s += '_'    
    
    
    print(real_s)
    setEOLComment(addr, "\"" + real_s + "\" with key \"" + key + "\"")
    createLabel(addr, "XOR_STR_"+s, True, symbol.SourceType.ANALYSIS)

except IllegalArgumentException as error:
    Msg.warn(self, "Error during headless processing: " + error.toString())
