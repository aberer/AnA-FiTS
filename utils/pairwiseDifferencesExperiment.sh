#! /bin/sh

if [ $# != 2 ]; then
    echo "usage: ./script <inputFile> <id>"
    exit
fi

id=$2

./utils/convertData.py $1 > data/tmp 
./utils/drawPairs.py data/tmp | cut -f 4  > data/dist.$id
./utils/plotDist.R data/dist.$id $id

echo -n  "avg is "
cat data/dist.$id | awk '{sum+=$1}END{print $1/NR}'
rm data/tmp


