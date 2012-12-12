#! /usr/bin/python 


import os 
import sys 
from progressbar import *
from multiprocessing import * 


if len(sys.argv) != 3 : 
    print """USAGE: ./script <case> <numSim>
1\trecombination (selection, 3-point-mass)
2\tbottleneck  (selection, gamma distributions)
"""
    sys.exit()

sfsCall = './sfs_code  1 1 ' 
seqLen = 100000

numSamp = 50


addItLater = " -a N -A "
# addItLater = "" 

case = int(sys.argv[1])
if case == 1: 
    params = ' -N 250 -L  %d  -r 1e-6 -m 1e-6   -W 1 0.01 0.2 0.8  '  % seqLen
    sfsParam = ' -N 250 -t 0.001 -r 0.001 -W 1 5 0.2 0.8 ' + addItLater
    theId = 'select3PM'
elif case == 2: 
    params = ' -N 250 -L %d -r 1e-6 -m 1e-6 -W 4 1.0 0.01 0.001 ' % seqLen
    sfsParam = ' -N 250 -t 0.001 -r 0.001 -W 3 -5 0.5  '  + addItLater
    theId = 'selectNeg'
elif case == 3 : 
    params = ' -N 250 -L %d -r 1e-5 -m 1e-6 -W 1 0.005 0.9 0.1 '  % seqLen
    sfsParam = ' -N 250 -t 0.001 -r 0.01  -W 1 2.5 0.9 0.1 -a N -A ' + addItLater
    numSamp = 100
    theId = 'moreRecomb' 
else:
    sys.exit()
    print "not implemented yet" 

numSim = int(sys.argv[2])

files = [ 'numHap.txt',  'numSnp.txt',  'pi.txt',  'sfs.txt' ]

widgets = ['progress: ', Percentage(), ' ', Bar(marker='=',left='[',right=']'),
           ' ', ETA()] 

def worker(x): 
    worker.q.put("bla bla bla")
    call = worker.call 
    call += "%d -s %d > /dev/null 2> /dev/null "  % (x,x)
    os.system(call)
    return 0 
        

def workerInit(q, call): 
    worker.q = q 
    worker.call = call 


def executeCallInParallel(call, num): 
    pbar = ProgressBar(widgets = widgets, maxval= num-1)
    pbar.start()
    jobs = range(0,num-1 )
    q = Queue()
    p = Pool(None, workerInit, [q, call ])
    results = p.imap(worker,jobs)
    for i in range(len(jobs)): 
        tmp = q.get()
        pbar.update(i)
    pbar.finish()
    p.close()
    p.join()

# forward simulation 
call = "$( ls ./AnA-FiTS-* | head -n 1  )   %s  -n %s.af-for."  % (params, theId)
# print call 
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d anafits anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-for'))
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt' % (elem, elem, 'af-for'))
os.system('rm anafits_poly*.%s.*' % theId)
os.system('rm anafits_info*.%s.*' % theId)
os.system('rm anafits_graph*.%s.*' % theId)

# sfs code fast 
numSamp = numSamp / 2  
call = sfsCall  +  " -L 20  5000 "  + sfsParam  + " -n " + str(numSamp)  + " -o sfs-fast." + theId   + '.'
print call 
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d SFS  sfs-fast.%s.*' % (numSamp * 2, theId) ) 
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt' % (elem, elem, 'sfs-fast'))
os.system('rm sfs-fast.%s.*' % theId) 
 
# sfs code slow  
# call = sfsCall + ( " -L 1 %d " % seqLen )  + sfsParam  + " -n " + str(numSamp)  + " -o sfs-slow." + theId   + '.'
# print call 
# executeCallInParallel(call, numSim)
# os.system('./utils/createValidationPlots.py %d SFS  sfs-slow.%s.*' % (numSamp * 2, theId) ) 
# for elem in files: 
#     os.system('mv %s $(basename %s .txt).%s.txt' % (elem, elem, 'sfs-slow'))
# os.system('rm sfs-slow.%s.*' % theId) 


os.system("./utils/segSites-sel.R  numSnp.af-for.txt  numSnp.sfs-fast.txt  > /dev/null")
print "#segsites"
os.system("./utils/numHap-sel.R  numHap.af-for.txt  numHap.sfs-fast.txt    > /dev/null")
print "numHap"
os.system("./utils/pie-sel.R  pi.af-for.txt  pi.sfs-fast.txt > /dev/null")
print "pi"
os.system("./utils/sfs-sel.R  sfs.af-for.txt  sfs.sfs-fast.txt > /dev/null")
print 'sfs'

os.system("mv sfs.pdf sfs-sel.%s.pdf" % theId)
os.system("mv segSites.pdf segSites-sel.%s.pdf" % theId)
os.system("mv pies.pdf pies-sel.%s.pdf" % theId)
os.system("mv numHap.pdf numHap-sel.%s.pdf" % theId)
