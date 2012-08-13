#! /bin/sh

if [ $# != 2 ]; then
    echo "usage: ./script <inputFile> <id>"
    exit
fi

id=$2

~/AnA-FiTS/utils/convertSeq.py $1 > data/tmp 
~/AnA-FiTS/utils/drawPairs.py data/tmp  > data/dist.$id
/bin/echo -n -e "$id\t"
~/AnA-FiTS/utils/plotDist.R data/dist.$id $id

rm data/dist.$id
rm data/tmp


