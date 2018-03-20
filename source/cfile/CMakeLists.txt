#
# Makefile for cfile library
#

set(cfile_SRCS
    cfile.c
)

set(cfile_OTHERS
    cfile.h
    makefile
    makefile.mac
    nocfile.h
)
set_source_files_properties(${cfile_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(cfile ${cfile_SRCS}	${cfile_OTHERS})

include_directories(../mem)
include_directories(../includes)
include_directories(../misc)
