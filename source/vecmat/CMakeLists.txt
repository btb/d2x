#
# Makefile for vector/matrix library
#

set(vecmat_SRCS
    vecmat.asm
)

set(vecmat_OTHERS
    bug.c
    disttest.c
    makefile
    sml9x9.asm
    vecmat.h
    vecmat.inc
    vectest.c
)
set_source_files_properties(${vecmat_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(vecmat ${vecmat_SRCS} ${vecmat_OTHERS})

#add_executable(vectest vectest.c)
#target_link_libraries(vectest vecmat io fix)
