#!/bin/sh

set -e

prefix="`pwd`"
host=i686-w64-mingw32

cd "$prefix"
hg clone -r stable-2.0 http://hg.icculus.org/icculus/physfs/ || true
cd physfs
hg up
mkdir -p build
cd build
cmake -DCMAKE_SYSTEM_NAME=windows -DCMAKE_C_COMPILER=$(which $host-gcc) -DCMAKE_C_FLAGS=-Wno-pointer-to-int-cast -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$prefix" -DPHYSFS_BUILD_TEST=OFF ..
make install/strip

cd "$prefix"
hg clone -r SDL-1.2 http://hg.libsdl.org/SDL || true
cd SDL
hg up
./autogen.sh
./configure --host=$host --prefix="$prefix" --disable-static
make install
$host-strip $prefix/bin/SDL.dll

cd "$prefix"
hg clone -r SDL-1.2 http://hg.libsdl.org/SDL_mixer || true
cd SDL_mixer
hg up
./autogen.sh
./configure --host=$host --with-sdl-prefix="$prefix" --prefix="$prefix" --disable-static
make install-hdrs install-lib
$host-strip $prefix/bin/SDL_mixer.dll

cd "$prefix"

for file in bin/sdl-config lib/pkgconfig/sdl.pc lib/pkgconfig/SDL_mixer.pc; do \
sed -i "" 's/^prefix=.*/prefix=arch\/win32/' $file; \
done
