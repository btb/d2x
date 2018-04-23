# $Date: 1996/01/23 22:32:42 $
#
# Makefile for misc libaray
#

set(misc_SRCS
    args.c
    error.c
    hash.c
)

set(misc_OTHERS
    args.h
    byteswap.c			byteswap.h
    debug.h
    error.h             errtest.c
    fileutil.c          fileutil.h
    hash.h
    makefile            makefile.mac
    parsarg.c           parsarg.h           parstest.c
    strutil.c           strutil.h
)

if(TARGSYS MATCHES "^OSX$")
    list(APPEND misc_SRCS byteswap.c fileutil.c strutil.c)
endif()

list(REMOVE_ITEM misc_OTHERS ${misc_SRCS})
set_source_files_properties(${misc_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(misc ${misc_SRCS} ${misc_OTHERS})

#add_executable(parstest parstest.c)
#target_link_libraries(parstest misc)

#add_executable(errtest errtest.c)
#target_link_libraries(errtest misc)

