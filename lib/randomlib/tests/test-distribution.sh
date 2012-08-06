#! /bin/sh
# tar.gz and zip distrib files copied to $DEVELSOURCE
# html documentation rsync'ed to  $DEVELSOURCE/doc/html/
#
# Windows version ready to build in
# $WINDOWSBUILD/RandomLib-$VERSION/BUILD-vc10
# after ./build installer is RandomLib-$VERSION-win{32,64}.exe
# (not supplying binary installers)
#
# Built version ready to install in /usr/local in
# relc/RandomLib-$VERSION/BUILD-system
VERSION=1.6
BRANCH=master
TEMP=/scratch/random-dist
DEVELSOURCE=/u/randomlib
GITSOURCE=file://$DEVELSOURCE
WINDOWSBUILD=/u/temp

test -d $TEMP || mkdir $TEMP
rm -rf $TEMP/*
mkdir $TEMP/gita
cd $TEMP/gita
git clone -b $BRANCH $GITSOURCE
cd randomlib
mkdir BUILD
cd BUILD
cmake ..
make dist
cp RandomLib-$VERSION.{zip,tar.gz} $DEVELSOURCE
rsync -a --delete doc/html/ $DEVELSOURCE/doc/html/
mkdir $TEMP/rel{a,c,x}
tar xfpzC RandomLib-$VERSION.tar.gz $TEMP/rela
tar xfpzC RandomLib-$VERSION.tar.gz $TEMP/relc
tar xfpzC RandomLib-$VERSION.tar.gz $TEMP/relx
rm -rf $WINDOWSBUILD/RandomLib-$VERSION
tar xfpzC RandomLib-$VERSION.tar.gz $WINDOWSBUILD
mkdir $WINDOWSBUILD/RandomLib-$VERSION/BUILD-vc10
(
    echo vcmake ..
    echo vmake all test install package
    echo \# cp RandomLib-$VERSION-win32.exe $DEVELSOURCE/
) > $WINDOWSBUILD/RandomLib-$VERSION/BUILD-vc10/build
chmod +x $WINDOWSBUILD/RandomLib-$VERSION/BUILD-vc10/build
mkdir $WINDOWSBUILD/RandomLib-$VERSION/BUILD-vc10-x64
(
    echo vcmake ..
    echo vmake all test install package
    echo \# cp RandomLib-$VERSION-win64.exe $DEVELSOURCE/
) > $WINDOWSBUILD/RandomLib-$VERSION/BUILD-vc10-x64/build
chmod +x $WINDOWSBUILD/RandomLib-$VERSION/BUILD-vc10=x64/build

cd $TEMP/rela/RandomLib-$VERSION
make -j10
make PREFIX=$TEMP/insta install
cd $TEMP/insta
find . -type f | sort -u > ../files.a

cd $TEMP/relc/RandomLib-$VERSION
mkdir BUILD
cd BUILD
cmake -D MAINTAINER=OFF -D CMAKE_INSTALL_PREFIX=$TEMP/instc ..
make -j10
make install
mkdir ../BUILD-maint
cd ../BUILD-maint
cmake -D MAINTAINER=ON -D CMAKE_INSTALL_PREFIX=$TEMP/instd ..
make -j10
make install
mkdir ../BUILD-matlab
cd ../BUILD-matlab
cmake -D MAINTAINER=OFF -D CMAKE_INSTALL_PREFIX=$TEMP/inste ..
make -j10
make install
mkdir ../BUILD-system
cd ../BUILD-system
cmake -D MAINTAINER=OFF ..
make -j10

cd $TEMP/instc
find . -type f | sort -u > ../files.c
cd $TEMP/instd
find . -type f | sort -u > ../files.d
cd $TEMP/inste
find . -type f | sort -u > ../files.e

cd $TEMP/relx
find . -type f | sort -u > ../files.x

cd $TEMP
cat > testprogram.cpp <<EOF
#include <RandomLib/Random.hpp>
#include <iostream>
int main() {
  RandomLib::Random r;
  std::cout << r() << "\n";
  return 0;
}
EOF
for i in a c d e; do
    cp testprogram.cpp testprogram$i.cpp
    g++ -c -g -O3 -I$TEMP/inst$i/include testprogram$i.cpp
    g++ -g -o testprogram$i testprogram$i.o -Wl,-rpath=$TEMP/inst$i/lib -L$TEMP/inst$i/lib -lRandom
    ./testprogram$i
done
