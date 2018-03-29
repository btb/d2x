#
# Makefile for fixed-point library
#

set(fix_SRCS)

set(fix_OTHERS
    asingen.c
    fix.asm
    fix.h
    fix.inc
    fixc.c
    fixppc.a
    makefile
    makefile.mac
    singen.c
    tables.c
    test.asm
)

if(TARGSYS MATCHES "^DOS4G$" OR TARGSYS MATCHES "^WIN32")
    list(APPEND fix_SRCS fix.asm)
else()
    list(APPEND fix_SRCS fixc.c tables.c)
endif()

list(REMOVE_ITEM fix_OTHERS ${fix_SRCS})
set_source_files_properties(${fix_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(fix ${fix_SRCS} ${fix_OTHERS})

add_executable(fixtest fixtest.c)
target_link_libraries(fixtest fix io)
