#
# Makefile for fixed-point library
#

set(fix_SRCS
    fix.asm
)

set(fix_OTHERS
    asingen.c
    fix.h
    fix.inc
    fixtest.c
    makefile
    singen.c
    test.asm
)
set_source_files_properties(${fix_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(fix ${fix_SRCS} ${fix_OTHERS})

#add_executable(fixtest fixtest.c)
#target_link_libraries(fixtest fix io)
