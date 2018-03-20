#
# Makefile for div0 library
#

set(div0_SRCS
    div0.asm
)

set(div0_OTHERS
    div0.h
    div0.inc
    makefile
)
set_source_files_properties(${DIV0_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(div0 ${div0_SRCS} ${div0_OTHERS})

add_executable(testd testd.c testda.asm)
target_link_libraries(testd div0)
