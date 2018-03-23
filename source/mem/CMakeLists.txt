#
# Makefile for memory debugging
#

set(mem_SRCS
	mem.c
)

set(mem_OTHERS
	1
	2
	makefile
	makefile.mac
	mem.h
)
set_source_files_properties(${mem_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(mem ${mem_SRCS} ${mem_OTHERS})

include_directories(../includes)
include_directories(../bios)

