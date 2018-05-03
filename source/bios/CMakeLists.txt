#
# Makefile for bios functions
#

set(io_SRCS)

set(io_OTHERS
    dos/dpmi.c
    dos/findfile.c
    dos/ipx.c
    dos/joy.asm         dos/joyc.c
    dos/key.asm         dos/key.c           dos/keys.inc        dos/oldkey.asm
    dos/mono.c
    dos/mouse.asm       dos/mouse.c
    dos/rbaudio.c       dos/rbaudio.new
    dos/timer.asm
    dpmi.h
    findfile.h
    ipx.h
    joy.h
    key.h
    makefile
    mono.h
    mouse.h
    posix/findfile.c
    posix/terminal.c
    rbaudio.h
    sdl/ipx.c
    sdl/joyc.c
    sdl/key.c
    sdl/mouse.c
    sdl/timer.c
    stub/mono.c
    stub/rbaudio.c
    swiftcfg.h
    testj.c             testm.c             testms.c
    timer.h
    win32/findfile.c
    win32/ipx.c
    win32/joyc.c
    win32/key.c
    win32/mono.c        win32/winmono.c
    win32/mouse.c
    win32/rbaudio.c
    win32/timer.c
    x                   x.c
)

if(TARGSYS MATCHES "^DOS4G$")
    list(APPEND io_SRCS
        dos/dpmi.c
        dos/findfile.c
        dos/ipx.c
        dos/joy.asm         dos/joyc.c
        dos/key.c
        dos/mono.c
        dos/mouse.c
        dos/rbaudio.c
        dos/timer.asm
    )

elseif(TARGSYS MATCHES "^WIN32")
    list(APPEND io_SRCS
        win32/findfile.c
        win32/ipx.c
        win32/joyc.c
        win32/key.c
        win32/mono.c
        win32/mouse.c
        win32/rbaudio.c
        win32/timer.c
    )

else()
    list(APPEND io_SRCS
        posix/findfile.c
        posix/terminal.c
        sdl/ipx.c
        sdl/joyc.c
        sdl/key.c
        sdl/mouse.c
        sdl/timer.c
        stub/mono.c
        stub/rbaudio.c
    )

endif()

list(REMOVE_ITEM io_OTHERS ${io_SRCS})
set_source_files_properties(${io_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${io_SRCS} ${io_OTHERS})

add_library(io ${io_SRCS} ${io_OTHERS})

if(TARGSYS MATCHES "^WIN32")
    target_link_libraries(io misc)
    target_link_libraries(io winmm)
endif()

if(SDL2_CONFIG)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(io PUBLIC ${SDL2_LIBRARIES})
endif()

#add_executable(demo cdrom.c)
#target_link_libraries(demo io)

add_executable(testk testk.c)
target_link_libraries(testk fix io misc)

add_executable(testt testt.c)
target_link_libraries(testt fix io)
