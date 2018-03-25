#
# Makefile for bios functions
#

set(io_SRCS
    dpmi.c
    error.c
    findfile.c  #DOS/Win
    ipx.c
    joy.asm
    joyc.c
    key.c
    mono.c
    mouse.c
    rbaudio.c
    timer.asm
)

set(io_OTHERS
    dpmi.h
    error.h
    findfile.h
    ipx.h
    joy.h
    key.asm
    key.h
    keys.inc
    makefile
    mono.h
    mouse.asm
    mouse.h
    oldkey.asm
    rbaudio.h
    rbaudio.new
    speaker.c
    swiftcfg.h
    testj.c
    testm.c
    testms.c
    testt.c
    timer.h
    timerp.c
    x
    x.c
)
set_source_files_properties(${io_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(io ${io_SRCS} ${io_OTHERS})

#add_executable(demo cdrom.c)
#target_link_libraries(demo io)

add_executable(testk testk.c)
target_link_libraries(testk io misc)
