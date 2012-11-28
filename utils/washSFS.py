#! /usr/bin/python

import sys 


if not sys.stdin.isatty() or len(sys.argv) != 1 :
    lines = sys.stdin.readlines()
else : 
    print USAGE
    sys.exit(1)

lines = lines[6:-1]

numMut = len(lines[0].strip())


print "// chromId 0"
print "// fixed\t"
print "// mutations\t" + ("4309,4882,0,A;" * (numMut))
ctr = 0
for line in lines: 
    print "%d\t%s" % (ctr, line.strip())
    ctr += 1 
