#
# Makefile for Win32 'PseudoBIOS' Extensions

# Build for Windows
set(TARGSYS "WIN32")

set(win95_SRCS
    palette.c key.c findfile.c timer.c rbaudio.c error.c
    ipx.c joyc.c swift.c mouse.c winapp.c mono.c
    comm.c winregs.c ds.c midifile.c winmidi.c winckpit.asm
    gfx.c dd.c ddgr.c direct3d.c xtapi.c iforce.c tactile.c
)

list(APPEND win95_OTHERS
    comm.h
    dd.h
    ddgfx.c
    ddraw.c
    direct3d.h
    ds.h
    error.h
    global.h
    iforce.h
    makefile
    midifile.h
    midiseq.h
    tactile.h
    winapp.c
    winmidi.h
    winmono.c
    winregs.h
    xtapi.h
)
set_source_files_properties(${win95_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(win95 ${win95_SRCS} ${win95_OTHERS})

# Where to find the DirectX stuff
#EXTLIBS = $(MINERLOCAL)\source\extlib\dd
