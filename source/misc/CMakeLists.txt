# $Date: 1996/01/23 22:32:42 $
#
# Makefile for misc libaray
#

set(misc_SRCS
    args.c
#   byteswap.c  #Mac
#   fileutil.c  #Mac
    hash.c
)
#    error.c     #Mac, missing

set(misc_OTHERS
	args.h
	byteswap.h
	debug.h
	errtest.c
	fileutil.h
	hash.h
	makefile
	makefile.mac
	parsarg.c   #DOS/Win
	parsarg.h
	parstest.c
	strutil.c   #Mac
	strutil.h
)
set_source_files_properties(${misc_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(misc ${misc_SRCS} ${misc_OTHERS})

include_directories(../includes)
include_directories(../misc)
include_directories(../mem)
include_directories(../fix)
include_directories(../cfile)
include_directories(../pa_null)
include_directories(../bios)    #DOS
#include_directories(../win95)  #Windows

#add_executable(parstest parstest.c)
#target_link_libraries(parstest misc)

#add_executable(errtest errtest.c)
#target_link_libraries(errtest misc)

