#!/bin/bash

if [ $# != 2 ]; then
    echo "USAGE: ./script <selProb2> <repid>"
    exit
fi

neutral=$1

if [ "$neutral" == "0" ]; then
    fitness="-w"
else     
    fitness="-W 2 $neutral 10 2000000 $neutral 1 5000000"
fi

id=$2

Ne="100 1000 10000"
L="10000 100000 1000000 10000000"
param="2.5e-8" 


binary=$(ls ./AnA-FiTS-* | tail -n 1 ) 


for N in $(echo $Ne) 
do     
    for l in $(echo $L)
    do 
	$binary  $fitness -s $id -N $N -L $l -m $param -r $param -n run.$N.$l.$neutral.$id &
    done 
done 
