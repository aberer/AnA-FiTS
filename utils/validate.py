#! /usr/bin/python

import os 
import sys
from progressbar import *
from multiprocessing import * 


numSim =  1000
theId = 'noRec' 
numSamp =  50

files = [ 'numHap.txt',  'numSnp.txt',  'pi.txt',  'sfs.txt' ]
afFor = ' -W 1 0 0.5 0.5 '
afMix = ' -W 1 0 0.25 0.25 '
afAnc = ' -w ' 

params = ' -r 0  ' 
# params = ' ' 
msParam = ' -t 200 -r 0 10000000  ' 


widgets = ['progress: ', Percentage(), ' ', Bar(marker='=',left='[',right=']'),
           ' ', ETA()] 

def f(x): 
    f.q.put("Doing : "+ str(x))
    call = f.call 
    call += "%d -s %d > /dev/null" % (x,x) 
    os.system(call)
    return 0 

def f_init(q, call):
    f.q = q
    f.call = call 

def executeCallInParallel(call, num):     
    pbar = ProgressBar(widgets=widgets, maxval=num-1)
    pbar.start()
    jobs = range(0,num-1)
    q = Queue()
    p = Pool(None, f_init, [q, call ])
    results = p.imap(f,jobs)
    for i in range(len(jobs)): 
        tmp = q.get()
        pbar.update(i)
    pbar.finish()
    p.close()
    p.join()

# forward 
call = "./AnA-FiTS-* %s %s -n %s.af-for."  % (params, afFor,  theId)   
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-for') )
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem,elem, 'af-for'))

# mixed 
call = "./AnA-FiTS-* %s %s -n %s.af-mix."  % (params, afMix,  theId)   
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-mix') )
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem, elem, 'af-mix'))

# ancestry-based
call = "./AnA-FiTS-* %s %s -n %s.af-anc."  % (params, afAnc,  theId)   
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d  anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-anc') )
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem,elem, 'af-anc'))

# ms 
os.system('~/proj/msdir/ms %d %d %s | ~/proj/AnA-FiTS/utils/sumStatMS.py '  % ( numSamp, numSim, msParam)) 
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem,elem, 'ms'))

os.system("./utils/segSites.R numSnp.af-for.txt numSnp.af-mix.txt numSnp.af-anc.txt numSnp.ms.txt > /dev/null")
os.system("./utils/numHap.R numHap.af-for.txt numHap.af-mix.txt numHap.af-anc.txt numHap.ms.txt > /dev/null")
os.system("./utils/pie.R pi.af-for.txt pi.af-mix.txt pi.af-anc.txt pi.ms.txt > /dev/null")
os.system("./utils/sfs.R sfs.af-for.txt sfs.af-mix.txt sfs.af-anc.txt sfs.ms.txt > /dev/null")
