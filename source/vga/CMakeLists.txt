#
# Makefile for VGA library
#

set(vga_SRCS
    framebuf.c
    modex.asm
    palette.c
    vesa.asm
    vga.c
)

set(vga_OTHERS
    dd.bat
    framebuf.jas
    fxdpmi.h
    fxvesa.h
    grx.h
    makefile
    tweak.inc
    vga.h
    vga.jas
    vgaregs.inc
    new/palette.c
    new/vga.c
)
set_source_files_properties(${vga_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(vga ${vga_SRCS} ${vga_OTHERS})

find_path(3DFX_INCLUDE_DIR 3dfx.h ../../extlib/glide/dos/include/)
include_directories(${3DFX_INCLUDE_DIR})
