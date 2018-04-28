#
# Makefile for IFF library
#

set(iff_SRCS
    iff.c
)

set(iff_OTHERS
    iff.h
    ifftest.c           ifftestv.c
    makefile
    vesa.h
    archive/iff8bpp.c   archive/iff15bpp.c  archive/iffmike.c
)
set_source_files_properties(${iff_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${iff_SRCS} ${iff_OTHERS})

add_library(iff ${iff_SRCS} ${iff_OTHERS})

#add_executable(ifftest ifftest.c)
#target_link_libraries(ifftest iff vga gr mem misc cfile io)

#add_executable(ifftestv ifftestv.c)
#target_link_libraries(ifftestv iff cfile) #vesa cflib 
