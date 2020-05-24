[![Build Status](https://travis-ci.org/utz82/bintracker.svg?branch=master)](https://travis-ci.org/utz82/bintracker)

This was the first prototype of [Bintracker](https://bintracker.org), written in C++. It is now defunct. The information below is retained for historical purposes.

# about bintracker

Bintracker is a free, open source, modular, cross-platform music editor for low-level sound drivers, and a visual front-end for the [Music Data Abstraction Language (MDAL)](https://utz82.github.io/MDAL). For more information, check out the project page at https://utz82.github.io/bintracker.


## building from source

Building bintracker from source requires an existing installation of __pugixml__ and __Allegro5__, including the respective header files. On Debian-based systems, running `apt-get install liballegro5-dev libpugixml-dev` should install all the necessary dependencies. On OS X, `brew install --HEAD allegro && brew install pugixml` should do the trick. After those dependencies have been installed, you can build bintracker by running `cmake . && make` in the main directory.

For cross-build bintracker for Windows with MinGW, there are two possible routes. You can either utilize a regular MinGW toolchain, or the [MXE](http://mxe.cc) toolchain. In either case, the Allegro5 sources are expected to be in a folder named `liballeg_w32` residing inside the bintracker main directory. Create this directory, `cd` into it and run `git clone https://github.com/liballeg/allegro5.git`. Next, you must install the necessary dependencies, including pugixml, libjpeg, libpng, zlib, libopus, and libwebp. After this step is completed, you can build the Allegro5 library. `cd` into the Allegro5 topdir. When using regular MinGW, run `cmake -DSHARED=0 -DWANT_DEMO=0 -DWANT_EXAMPLES=0 -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-mingw.cmake`. When using MXE, run `cmake -DSHARED=0 -DWANT_DEMO=0 -DWANT_EXAMPLES=0 -DCMAKE_TOOLCHAIN_FILE=/usr/lib/mxe/usr/i686-w64-mingw32.static/share/cmake/mxe-conf.cmake`. Then, run `make`. Now, you can build bintracker itself by running `make -f makefile.mingw` (regular MinGW), or make -f makefile.travis_mingw` (MXE) in the main bintracker directory.
