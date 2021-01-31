#!/bin/bash

mkdir deps
cd deps

# Build and install vitaGL, mathneon and vitaShaRK

git clone https://github.com/Rinnegatamante/vitaGL.git
cd vitaGL
git reset --hard 9c963ab
make HAVE_SBRK=1 HAVE_SHARK=1 install -j4
cd ../

git clone https://github.com/Rinnegatamante/math-neon.git
cd math-neon
make install -j4
cd ../

git clone https://github.com/Rinnegatamante/vitaShaRK.git
cd vitaShaRK
# Taken from https://github.com/Rinnegatamante/vitaShaRK/README.md
vita-libs-gen SceShaccCg.yml build
cd build
make install
cd ..
cp shacccg.h $VITASDK/arm-vita-eabi/include/psp2/shacccg.h
make install

cd ../../
rm -rf deps
