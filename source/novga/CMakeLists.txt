#
# Makefile for nonvga functions
#

set(novga_SRCS
	novesa.c
)

set(novga_OTHERS
	makefile
)
set_source_files_properties(${novga_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(novga ${novga_SRCS} ${novga_OTHERS})

include_directories(../includes)
include_directories(../2d)
include_directories(../fix)
