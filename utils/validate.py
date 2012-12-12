#! /usr/bin/python

import os 
import sys
from progressbar import *
from multiprocessing import * 


if len(sys.argv) != 3 : 
    print """USAGE: ./script <case> <numSim>
where numSim is the number of simulations to be conducted 
and  case is 
1\tno recombination (neutrality)
2\trecombination (neutrality)
3\tinstantaneous population doubling (neutrality)
4\tbottleneck (neutrality)
5\texponential growth after 4000 generations
"""
    sys.exit()

useMS = True 
numSamp = 50

case = int(sys.argv[1])
if case == 1: 
    params = ' -r 0 -L 1000000 ' 
    msParam = ' -t 20  '  
    theId = 'noRec' 
elif case == 2: 
    params = ' -L 1000000 ' 
    msParam = ' -t 20 -r 20 1000000 '
    theId = 'rec' 
elif case == 3 : 
    # 500 => 1000 
    params = ' -T c 4600 2.0 -L 1000000 '
    msParam = ' -t 40 -r 40 1000000 -eN 0.1 0.5 '
    theId = 'doubling'
elif case == 4: 
    params = ' -T c 4750 0.5 -L 1000000 '
    msParam = ' -t 10 -r 10 1000000 -eN 0.25 2.0 '
    theId = 'bottle'
elif case == 5: 
    params = ' -T g 4000 6.9e-4 -L 1000000 '
    numSamp = 75
    msParam = ' -t 40 -r 40 1000000 -G 2.77 -eG 0.25 0 ' 
    theId = 'expon'
else:
    sys.exit()
    print "not implemented yet" 

numSim = int(sys.argv[2])
files = [ 'numHap.txt',  'numSnp.txt',  'pi.txt',  'sfs.txt' ]
afFor = ' -W 1 0 0.5 0.5 '
afMix = ' -W 1 0 0.25 0.25 '
afAnc = ' -w ' 


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
call = "$( ls ./AnA-FiTS-* | head -n 1  )  %s %s -n %s.af-for."  % (params, afFor,  theId)   
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d  anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-for') )
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem,elem, 'af-for'))
os.system('rm anafits_info.*af-for.*')
os.system('rm anafits_graph.*af-for.*')
os.system('rm anafits_poly*.af-for.*')

# mixed 
call = "$( ls ./AnA-FiTS-* | head -n 1 )  %s %s -n %s.af-mix."  % (params, afMix,  theId)   
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d anafits anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-mix') )
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem, elem, 'af-mix'))
os.system('rm anafits_info.*af-mix.*')
os.system('rm anafits_graph.*af-mix.*')
os.system('rm anafits_poly*.af-mix.*')

# ancestry-based
call = "$( ls ./AnA-FiTS-* | head -n 1 )  %s %s -n %s.af-anc."  % (params, afAnc,  theId)   
executeCallInParallel(call, numSim)
os.system('./utils/createValidationPlots.py %d anafits  anafits_polymorphisms.%s.%s.*' % (numSamp, theId, 'af-anc') )
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem,elem, 'af-anc'))
os.system('rm anafits_info.*af-anc.*')
os.system('rm anafits_poly*af-anc.*')
os.system('rm anafits_graph*.af-anc.*')

# reference  
os.system('./ms %d %d %s | ./utils/sumStatMS.py '  % ( numSamp, numSim, msParam)) 
for elem in files: 
    os.system('mv %s $(basename %s .txt).%s.txt'  % (elem,elem, 'ref'))

modeString = "isMS" if useMS else "isSFS"
os.system("R --vanilla <  ./utils/segSites.R --args %s numSnp.af-for.txt numSnp.af-mix.txt numSnp.af-anc.txt numSnp.ref.txt > /dev/null" % modeString)
os.system("R --vanilla <  ./utils/numHap.R --args %s numHap.af-for.txt numHap.af-mix.txt numHap.af-anc.txt numHap.ref.txt > /dev/null" % modeString)
os.system("R --vanilla <  ./utils/pie.R --args  %s pi.af-for.txt pi.af-mix.txt pi.af-anc.txt pi.ref.txt > /dev/null" %  modeString)
os.system("R --vanilla <  ./utils/sfs.R --args  %s sfs.af-for.txt sfs.af-mix.txt sfs.af-anc.txt sfs.ref.txt > /dev/null" %  modeString)

os.system("mv sfs.pdf sfs-%s.pdf" % theId)
os.system("mv segSites.pdf segSites-%s.pdf" % theId)
os.system("mv pies.pdf pies-%s.pdf" % theId)
os.system("mv numHap.pdf numHap-%s.pdf" % theId)
