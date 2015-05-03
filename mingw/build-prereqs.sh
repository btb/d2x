#!/bin/sh

set -e

prefix="`pwd`"
host=x86_64-w64-mingw32

cd "$prefix"
hg clone -r stable-2.0 http://hg.icculus.org/icculus/physfs/ || true
cd physfs
hg up
cmake -DCMAKE_SYSTEM_NAME=windows -DCMAKE_C_COMPILER=$host-gcc -DCMAKE_C_FLAGS=-Wno-pointer-to-int-cast -DCMAKE_INSTALL_PREFIX="$prefix" .
make install

cd "$prefix"
hg clone -r SDL-1.2 http://hg.libsdl.org/SDL || true
cd SDL
hg up
./autogen.sh
./configure --host=$host --prefix="$prefix"
make install

cd "$prefix"
hg clone -r SDL-1.2 http://hg.libsdl.org/SDL_mixer || true
cd SDL_mixer
hg up
mkdir -p build
./autogen.sh
./configure --host=$host --with-sdl-prefix="$prefix" --prefix="$prefix"
make install-hdrs install-lib

cd "$prefix"
sed -i "" 's/^prefix=.*/prefix=mingw/' bin/sdl-config
