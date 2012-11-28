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


sfsCall = './sfs_code  1 1  -a N -A  -L 10 10000 -t 2e-5 -r 2e-5 '

case = int(sys.argv[1])
if case == 1: 
    params = ' -W 1 0.005 0.25 0.25 '
    sfsParam = ' -W  1 5 0.25 0.25 '
    theId = 'select3PM'
elif case == 2: 
    # TODO 
    params = ''
    sfsParam = ''
    theId = ''
else : 
    sys.exit()
    print "not implemented yet" 

numSamp = 50
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
call = "$( ls ./AnA-FiTS-* | head -n 1  ) -L 100000  %s  -n %s.af-for."  % (params, theId)
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d anafits anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-for'))
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt' % (elem, elem, 'af-for'))
os.system('rm anafits_*.%s.*' % theId)

# sfs code 
numSamp = 25 
call = sfsCall + sfsParam  + " -n " + str(numSamp)  + " -o sfs." + theId   + '.'
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d SFS  sfs.%s.*' % (numSamp * 2, theId) ) 
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt' % (elem, elem, 'sfs'))
# os.system('rm sfs.%s.*' % theId)

os.system("./utils/segSites-sel.R  numSnp.af-for.txt numSnp.sfs.txt > /dev/null")
os.system("./utils/numHap-sel.R  numHap.af-for.txt numHap.sfs.txt > /dev/null")
os.system("./utils/pie-sel.R  pi.af-for.txt pi.sfs.txt > /dev/null")
os.system("./utils/sfs-sel.R  sfs.af-for.txt sfs.sfs.txt > /dev/null")

os.system("mv sfs.pdf sfs-sel.%s.pdf" % theId)
os.system("mv segSites.pdf segSites-sel.%s.pdf" % theId)
os.system("mv pies.pdf pies-sel.%s.pdf" % theId)
os.system("mv numHap.pdf numHap-sel.%s.pdf" % theId)
