#!/bin/bash

neutral=0.5

Ne="100 1000 10000"
L="10000 100000 1000000 10000000"
param="2.5e-8" 
fitness="2 $neutral 10 2000000 $neutral 1 5000000"

binary=$(ls ./AnA-FiTS-* | tail -n 1 ) 

for rep in  1 2 3 4 
do 
    for N in $(echo $Ne) 
    do 
	for L in $(echo $L)
	do 
	    $binary -W $fitness -N $N -L $L -m $param -r $param -n run.$N.$L.$rep
	        # exit 
	done 
    done 
done 
