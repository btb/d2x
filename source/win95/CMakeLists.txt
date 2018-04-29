#
# Makefile for Win32 'PseudoBIOS' Extensions

# Build for Windows
set(TARGSYS "WIN32")

set(win95_SRCS
    comm.c
    dd.c                ddgr.c              direct3d.c          ds.c
    gfx.c
    midifile.c
    palette.c
    winapp.c            winckpit.asm        winmidi.c           winregs.c
)

list(APPEND win95_OTHERS
    comm.c          comm.h
    dd.c            dd.h
    ddgfx.c         ddgr.c          ddraw.c
    direct3d.c      direct3d.h
    ds.c            ds.h
    gfx.c           global.h
    iforce.c        iforce.h
    makefile
    midifile.c      midifile.h      midiseq.h
    muldiv32.h
    palette.c
    swift.c
    tactile.c       tactile.h
    winapp.c        winapp.h
    winckpit.asm
    winmidi.c       winmidi.h
    winregs.c       winregs.h
    xtapi.c         xtapi.h
)

if(TACTILE)
    list(APPEND win95_SRCS iforce.c tactile.c)
endif()

list(REMOVE_ITEM win95_OTHERS ${win95_SRCS})
set_source_files_properties(${win95_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(win95 ${win95_SRCS} ${win95_OTHERS})

include_directories(${DIRECTX_INCLUDE_DIR})
