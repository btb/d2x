#
# Makefile for bios functions
#

set(io_SRCS
    error.c
)

set(io_OTHERS
    dpmi.h
    error.h
    findfile.h
    ipx.h
    joy.h
    key.h
    makefile
    mono.h
    mouse.h
    rbaudio.h
    swiftcfg.h
    testj.c
    testm.c
    testms.c
    timer.h
    x
    x.c
)
set_source_files_properties(${io_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(io ${io_SRCS} ${io_OTHERS})

if(TARGSYS MATCHES "^DOS4G$")
    add_subdirectory(dos)
elseif(TARGSYS MATCHES "^WIN32")
    add_subdirectory(win32)
    target_link_libraries(io misc)
    target_link_libraries(io winmm)
endif()

#add_executable(demo cdrom.c)
#target_link_libraries(demo io)

add_executable(testk testk.c)
target_link_libraries(testk io misc)

add_executable(testt testt.c)
target_link_libraries(testt io)
