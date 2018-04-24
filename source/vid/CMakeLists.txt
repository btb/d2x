#
# Makefile for VGA library
#

set(vid_SRCS)

set(vid_OTHERS
    makefile
    sdl/vga.c
    stub/makefile
    stub/macstubs.c
    stub/novesa.c
    vga/framebuf.c
    vga/fxdpmi.h        vga/fxvesa.h
    vga/modex.asm
    vga/tweak.inc
    vga/vesa.asm
    vga/vga.c
    vga/vgaregs.inc
    vga.h
)

if(TARGSYS MATCHES "^DOS4G$")
    list(APPEND vid_SRCS
        vga/framebuf.c
        vga/modex.asm
        vga/vesa.asm
        vga/vga.c
    )

    find_path(3DFX_INCLUDE_DIR 3dfx.h ../../extlib/glide/dos/include/)
    include_directories(${3DFX_INCLUDE_DIR})

elseif(TARGSYS MATCHES "^WIN32$")
    list(APPEND vid_SRCS
        stub/novesa.c
    )

else()
    list(APPEND vid_SRCS
        sdl/vga.c
    )

endif()

list(REMOVE_ITEM vid_OTHERS ${vid_SRCS})
set_source_files_properties(${vid_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(vid ${vid_SRCS} ${vid_OTHERS})

if(SDL2_CONFIG)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(vid PUBLIC ${SDL2_LIBRARIES})
endif()
