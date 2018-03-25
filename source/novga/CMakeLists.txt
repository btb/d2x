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
