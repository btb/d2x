#
# Makefile for Win32 'PseudoBIOS' Extensions

# Build for Windows
set(TARGSYS "WIN32")

set(win95_SRCS
    palette.c key.c findfile.c timer.c rbaudio.c error.c
    ipx.c joyc.c mouse.c winapp.c mono.c
    comm.c winregs.c ds.c midifile.c winmidi.c winckpit.asm
    gfx.c dd.c ddgr.c direct3d.c
)

list(APPEND win95_OTHERS
    comm.c          comm.h
    dd.bat          dd.c            dd.h
    ddgfx.c         ddgr.c          ddraw.c
    direct3d.c      direct3d.h
    ds.c            ds.h
    error.c         findfile.c
    gfx.c           global.h
    iforce.c        iforce.h
    ipx.c           joyc.c          key.c
    makefile
    midifile.c      midifile.h      midiseq.h
    mono.c          mouse.c
    muldiv32.h
    palette.c       rbaudio.c
    swift.c
    tactile.c       tactile.h
    timer.c
    winapp.c        winapp.h
    winckpit.asm
    winmidi.c       winmidi.h
    winmono.c
    winregs.c       winregs.h
    xtapi.c         xtapi.h
)

if(TACTILE)
    list(APPEND win95_SRCS iforce.c tactile.c)
endif()

list(REMOVE_ITEM win95_OTHERS ${win95_SRCS})
set_source_files_properties(${win95_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(win95 ${win95_SRCS} ${win95_OTHERS})

# Where to find the DirectX stuff
#EXTLIBS = $(MINERLOCAL)\source\extlib\dd
