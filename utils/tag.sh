#! /bin/bash

if [ "$#" != "1"  ]; then
    echo "./script <versionName>"
    exit 
fi

git tag -a "$1" -m "stable release verison $1"
make clean
make vupdate 

git tag -d "$1 "
git commit -a -m "automated pre-tag commit"
git tag -a "$1" -m "release verison $1" 
git push
git push --tags



