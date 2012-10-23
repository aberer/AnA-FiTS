#! /bin/bash

# this call of anafits simulates an entire human female genome within its simulation assumptions 

rateNonNeutral=0.05

executable=$(ls ./AnA-FiTS-* | tail -n 1  )


if [ "$#" != "1" ]; then
    echo "USAGE: ./script <numIndi>"
    exit 
fi

numIndi=$1

# the lengths of the 23 human chromosomes (female)
lengths="250000000 243000000 198000000 192000000 181000000 171000000 158000000 146000000 142000000 136000000 135000000 134000000 115000000 107000000 103000000 90000000 82000000 78000000 59000000 63000000 48000000 51000000 155000000"

# effectively neutral 
fitnessModel="2 $rateNonNeutral 10 2000 $rateNonNeutral 1 500"
rate="2.5e-8"

$executable -m $rate -r $rate -W $fitnessModel -N $numIndi -L $lengths -n humanGenome.indi$numIndi
