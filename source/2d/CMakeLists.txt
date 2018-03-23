#
# Makefile for 2d graphics library
#

set(gr_SRCS
    bitblt.c
    bitmap.c
    box.c
    canvas.c
    circle.c
    disc.c
    font.c
    gpixel.c
    gr.c
    ibitblt.c
    lbitblt.asm
    line.c
    linear.asm  #DOS/Win
#    macstubs.c  #Mac
    pcx.c
    pixel.c
    poly.c
    rect.c
    rle.c
    scale.c     #DOS
    scalea.asm  #DOS
#    scalemac.c  #Mac
    scanline.c
)
#    blit.c     #Mac, missing
#    roller.c   #missing

set(gr_OTHERS
    clip.h
    cursor.c
    effect2d.h
    effects.c
    effects.h
    gr.h
    gr.inc
    grdef.h
    ibitblt.h
    makefile
    makefile.mac
    mspeed.c
    nonvesa.c
    paging.asm
    palette.h
    pcx.h
    pflow.asm
    rle.h
    selector.asm
    tmapbits.c
    tpoly.c
    vesalib.c
    vesatest.c
)
add_library(gr ${gr_SRCS} ${gr_OTHERS})

set_source_files_properties(${gr_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

include_directories(../includes)
include_directories(../misc)
include_directories(../mem)
include_directories(../fix)
include_directories(../cfile)
include_directories(../pa_null)
include_directories(../bios)

#add_executable(testg testg.c tmapbits.c)
#target_link_libraries(testg io gr misc mem iff cfile)

#add_executable(testp)
#target_link_libraries(testp io gr misc mem cflib)

#add_executable(vesatest vesatest.c)
#target_link_libraries(vesatest io gr misc mem cflib)
