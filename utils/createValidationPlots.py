#! /usr/bin/python

import os 
import sys 
from progressbar import * 
from tempfile import * 
from multiprocessing import * 


numSamp = int(sys.argv[1]) 
files = sys.argv[2:]


USAGE=""" 
./script <numSamp> <file...>
"""

if len(sys.argv) < 2: 
    print USAGE
    sys.exit(0)

widgets = ['progress: ', Percentage(), ' ', Bar(marker='=',left='[',right=']'),
           ' ', ETA()] 


def workerInit(q): 
    workerFun.q = q

def workerFun(afile): 
    tmp = mkstemp()    
    global numSamp
    cmd = "./convertSeq " + afile +  " | ./utils/statistics.py " + str(numSamp)  + " > " + tmp[1]
    os.system(cmd)
    workerFun.q.put(tmp[1])
    return tmp[1] 


numSnp = []
numHaps = []
pies = []
sfsDict = {}
ctr = 1

q = Queue() 
p = Pool(None, workerInit, [q])
results = p.imap(workerFun, files)
p.close()

pbar = ProgressBar(widgets=widgets, maxval=len(files))
pbar.start()
ctr = 0
for i in range(len(files)):
    tmpFile = q.get()
    fh = open(tmpFile, "r")
    numSnp.append(fh.readline().strip().split()[1])
    numHaps.append(fh.readline().strip().split()[1]) 
    pies.append( fh.readline().strip().split()[1]) 
    fh.readline()
    
    sfsTmp = map(lambda x : x.strip() , fh.readlines())
    fh.close()
    os.unlink(tmpFile)

    for sfs in sfsTmp:         
        sfs = sfs.split()
        num = int(sfs[0]) 
        freq = float(sfs[1])

        
        if sfsDict.has_key(num): 
            sfsDict[num] += float(freq) 
        else : 
            sfsDict[num] = float(freq) 
    
    pbar.update(ctr)
    ctr += 1 
pbar.finish()
p.join()

for key in sfsDict.keys():
    sfsDict[key] /= float(len(numSnp))

fh = open("numSnp.txt", "w")
for elem in numSnp: 
    fh.write("%s\n" % elem)
fh.close()

fh = open("numHap.txt", "w")
for elem in numHaps: 
    fh.write("%s\n" % elem) 
fh.close()

fh = open("pi.txt", "w")
for elem in pies: 
    fh.write("%s\n" % elem)
fh.close()

fh = open('sfs.txt', 'w')
for key in sfsDict.keys(): 
    fh.write("%d\t%f\n" % (key ,sfsDict[key]) )
fh.close()
