#! /usr/bin/python

import sys 
import struct 

INTEGER_LENGTH = 4 
FLOAT_LENGTH=4                  # currently using floats 

printFitness = True  

def printSequence(withFitness, content, index): 
    # get number of sequences 
    numSeq = struct.unpack_from("I", content[index:(index+INTEGER_LENGTH)])[0]
    index += INTEGER_LENGTH
    # print( str(numSeq) + " sequences")

    # get number of mutations 
    for x in range(0,numSeq): 
        numMut = struct.unpack_from("I", content[index:(index+4)])[0]
        index += INTEGER_LENGTH
        # print( "number of mutations " + str(numMut))
        
        for y in range(0, numMut):
            if(withFitness):
                result = struct.unpack_from("I f", content[index:index+8])
                index += INTEGER_LENGTH + FLOAT_LENGTH
                if(printFitness): 
                    sys.stdout.write( "%d,%f;" % result)
                else : 
                    sys.stdout.write( "%d;" % result[0])
            else:
                result = struct.unpack_from("I", content[index:(index + INTEGER_LENGTH)])[0]
                index += INTEGER_LENGTH
                sys.stdout.write( "%d;" % result)
        print("")

    return index 


if len(sys.argv) != 2 : 
    print "USAGE: ./script <binary-file>"
    sys.exit()


fh = open(sys.argv[1], "rb")
content = fh.read()
index = 0

index = printSequence(True, content, index)
index = printSequence(False, content, index)

