#!/bin/bash

if [ $# != 2 ]; then
    echo "USAGE: ./script <selProb2> <repid>"
    exit
fi

neutral=$1

if [ "$neutral" == "0" ]; then
    fitness="-w"
else     
    fitness="-W 1 0.00000001 $neutral $neutral "
fi

id=$2

Ne="100 500 1000 5000 10000"
L="10000 50000  100000   500000 1000000  "
param="2.5e-8" 


binary=$(ls ./AnA-FiTS-* | tail -n 1 ) 


for N in $(echo $Ne) 
do     
    for l in $(echo $L)
    do 
	$binary  $fitness -s $id -N $N -L $l -m $param -r $param -n run.$N.$l.$neutral.$id  & 
	sleep 1 
    done 
done

wait
 
L="5000000 10000000"

for N in $(echo $Ne) 
do     
    for l in $(echo $L)    
    do 
	if [ "$L" == "10000000" -a "$N" == 10000 ]; then
	    minusR="-R 7"
	else 
	    minusR=""
	fi
	$binary  $fitness -s $id -N $N -L $l -m $param -r $param -n run.$N.$l.$neutral.$id  $minusR
	sleep 1 


    done
done

