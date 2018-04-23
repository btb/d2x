#
# Makefile for dummy poly_acc library
#

set(poly_acc_SRCS
    poly_acc.c
)

set(poly_acc_OTHERS
    makefile
    pa_enabl.h          poly_acc.h
)
set_source_files_properties(${poly_acc_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(poly_acc ${poly_acc_SRCS} ${poly_acc_OTHERS})
