#
# Makefile for VGA library
#

set(vid_SRCS
    vga/framebuf.c
    vga/modex.asm
    vga/vesa.asm
    vga/vga.c
)

set(vid_OTHERS
    makefile
    vga/fxdpmi.h        vga/fxvesa.h
    vga/tweak.inc
    vga/vgaregs.inc
    vga.h
    )
set_source_files_properties(${vid_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(vid ${vid_SRCS} ${vid_OTHERS})

find_path(3DFX_INCLUDE_DIR 3dfx.h ../../extlib/glide/dos/include/)
include_directories(${3DFX_INCLUDE_DIR})
