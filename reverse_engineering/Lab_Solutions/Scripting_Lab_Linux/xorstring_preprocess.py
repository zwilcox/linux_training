#!/usr/bin/python3
import sys
import codecs

def xorstrings(intxt):
    # dumb search for xorstr calls, replacing str argument with xord version
    xorkey = b"\x41\x43\x54\x50"

    searchstr = "xorstr"    
    j = 0

    while True:
        j = intxt.find(searchstr, j)
        if j == -1:
            break
            
        j += len(searchstr)
        
        if intxt[j] != '(':
            print("Parsing Error! Please no uses of xorstr without an immediate parameters")
            return -1

        j += 1
        
        if intxt[j] != '"':
            lnum = intxt[:j].count('\n')
            print(f"skipping use/declaration of xorstr that starts with \"{intxt[j:j+9]}...\"")
            continue

        j += 1
        k = j

        ksearch = True
        while ksearch:
            k = intxt.find('"', k)
            if k == -1:
                print("Unable to find end of string!")
                exit(-1)
            
            if intxt[k-1] != '\\':
                ksearch = False
            else:
                k = k+1

        xorme = intxt[j:k]
        if len(xorme) == 0:
            continue

        print(f"xoring string \"{xorme}\"")
        
        xorme_escape = codecs.encode(codecs.decode(xorme, 'unicode_escape'), 'ascii')
        
        xord = ""
        for i in range(len(xorme_escape)):
            xc = xorme_escape[i] ^ xorkey[i%len(xorkey)]
            #xc = xorme_escape[i]
            xord += "\\x" + hex(xc)[2:].zfill(2)        

        intxt = intxt[:j] + xord + intxt[k:]

    return intxt

def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} outputfile.i inputfile.i")
        exit(-1)

    outfile, infile = sys.argv[1:3]
    
    # read in the input file
    intxt = ""
    with open(infile, "r") as fp:
        intxt = fp.read()

    outtxt = xorstrings(intxt)
    
    with open(outfile, "w") as fp:
        fp.write(outtxt)

    exit(0)

if __name__ == '__main__':
    main()
