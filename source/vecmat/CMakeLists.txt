#
# Makefile for vector/matrix library
#

set(vecmat_SRCS)

set(vecmat_OTHERS
    bug.c
    disttest.c
    makefile
    sml9x9.asm
    vecmat.asm          vecmat.h            vecmat.inc
    vecmatc.c           vecmatppc.a         vecmat68k.inc
)

if(NOT VECMAT_NO_ASM)
    list(APPEND vecmat_SRCS vecmat.asm)
else()
    list(APPEND vecmat_SRCS vecmatc.c)
endif()

list(REMOVE_ITEM vecmat_OTHERS ${vecmat_SRCS})
set_source_files_properties(${vecmat_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(vecmat ${vecmat_SRCS} ${vecmat_OTHERS})

add_executable(vectest vectest.c)
target_link_libraries(vectest vecmat fix misc io)
