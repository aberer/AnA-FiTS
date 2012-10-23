#! /bin/sh


binary=$(ls ./AnA-FiTS-* | tail -n 1 ) 

Ne="500 1000 5000"
L="100000 500000"
param="2.5e-8" 

for tuneParam in $(seq 2 10  )
do 
    for N in $(echo $Ne) 
    do     
	for l in $(echo $L)
	do 
	    $binary -w -R $tuneParam  -s 1  -N $N -L $l -m $param -r $param -n run.$N.$l.$id >  run.$N.$l.$tuneParam  &  
	    sleep 1 
	done 
    done
    wait 
done 
