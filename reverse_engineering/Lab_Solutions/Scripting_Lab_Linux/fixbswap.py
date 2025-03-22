# fixes bswaps
#@category ACTP

# put this in one of your ghidra script directories

from ghidra.framework.model import DomainFile
from ghidra.framework.model import DomainFolder
from ghidra.program.model import symbol
from ghidra.program.model import mem
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
    saddr = askAddress("Swapped Data Start", "enter address")
    eaddr = askAddress("Swapped Data End", "enter address")

    
    dif = eaddr.subtract(saddr) + 1
    for o in range(0, dif, 8):
        if (o+8) > dif:
            break
        
        b0 = getByte(saddr.add(o + 0))
        b1 = getByte(saddr.add(o + 1))
        b2 = getByte(saddr.add(o + 2))
        b3 = getByte(saddr.add(o + 3))
        b4 = getByte(saddr.add(o + 4))
        b5 = getByte(saddr.add(o + 5))
        b6 = getByte(saddr.add(o + 6))
        b7 = getByte(saddr.add(o + 7))

        setByte(saddr.add(o + 0), b7)
        setByte(saddr.add(o + 1), b6)
        setByte(saddr.add(o + 2), b5)
        setByte(saddr.add(o + 3), b4)
        setByte(saddr.add(o + 4), b3)
        setByte(saddr.add(o + 5), b2)
        setByte(saddr.add(o + 6), b1)
        setByte(saddr.add(o + 7), b0)

    print("Swapped successfully, make sure you nop out the code that was supposed to do this at runtime if exporting this back out")

except mem.MemoryAccessException as error:
    Msg.Warn(self, "Unable to Access memory there, did clear the code bytes?: " + error.ToString())
except IllegalArgumentException as error:
    Msg.warn(self, "Error during headless processing: " + error.toString())
