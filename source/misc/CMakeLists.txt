# $Date: 1996/01/23 22:32:42 $
#
# Makefile for misc libaray
#

set(misc_SRCS
    args.c
#   byteswap.c  #Mac
    error.c
#   fileutil.c  #Mac
    hash.c
)

set(misc_OTHERS
	args.c
	args.h
	byteswap.h
	debug.h
	error.h
	errtest.c
	fileutil.h
	hash.c
	hash.h
	makefile
	makefile.mac
	parsarg.c   #DOS/Win
	parsarg.h
	parstest.c
	strutil.c   #Mac
	strutil.h
)

if(TARGSYS MATCHES "^OSX$")
    list(APPEND misc_SRCS strutil.c)
endif()

list(REMOVE_ITEM misc_OTHERS ${misc_SRCS})
set_source_files_properties(${misc_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(misc ${misc_SRCS} ${misc_OTHERS})

#add_executable(parstest parstest.c)
#target_link_libraries(parstest misc)

#add_executable(errtest errtest.c)
#target_link_libraries(errtest misc)

