rem d1vars sets up compiler variables
call d1vars.bat
set LIBDIR = y:\dev\d1\source\main
set CCFLAGS = /dNETWORK /dRELEASE /dNDEBUG
cd misc
wmake
cd..
cd includes
wmake
cd ..
cd fix
wmake
cd ..
cd cfile
wmake
cd ..
cd 2d
wmake
cd ..
cd bios
wmake
cd ..
cd iff
wmake
cd ..
cd div
wmake
cd ..
cd mem
wmake
cd ..
cd vecmat
wmake
cd ..
cd 3d
wmake
cd ..
cd texmap
wmake
cd ..
cd ui
wmake
cd ..
cd main
cd editor
rem wmake
cd ..
wmake
cd ..
Echo Make complete.

