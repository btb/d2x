#
# Makefile for macro and header files
#

set(includes_OTHERS
    dtypes.h
    dtypes.inc
    makefile
    psmacros.inc
    pstypes.h
    pstypes.inc
    test.c
)
set_source_files_properties(${includes_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_custom_target(includes SOURCES ${includes_OTHERS})

include_directories(../bios)

#add_executable(includes_test test.c) # mactest.c
#target_link_libraries(includes_test io)
