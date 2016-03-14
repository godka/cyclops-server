#!/bin/bash
rm *.lib
rm -rf SDL2
rm -rf live555

tar -zxvf SDL2-2.0.4.tar.gz
cd SDL2-2.0.4
./configure && make -j && sudo make install
cd ../

tar -zxvf SDL2_net-2.0.1.tar.gz
cd SDL2_net-2.0.1
./configure && make -j && sudo make install
cd ../

tar -zxvf live555-latest.tar.gz
cd live
./genMakefiles mingw
make -j && sudo make install
cd ../

make -j && sudo make install