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
    line.c
    palette.c
    pcx.c
    pixel.c
    poly.c
    rect.c
    rle.c
    scanline.c
)

set(gr_OTHERS
    bitblt.c        bitmap.c        box.c
    canvas.c        circle.c        clip.h          cursor.c
    dd.bat
    disc.c
    effect2d.h      effects.c       effects.h
    font.c
    gpixel.c
    gr.c            gr.h            gr.inc          gr.out          grdef.h
    ibitblt.c       ibitblt.h       lbitblt.asm
    line.c          linear.asm
    macstubs.c
    makefile        makefile.mac
    mndebug.bat
    mspeed.c        nonvesa.c
    paging.asm
    palette.h       pcx.c           pcx.h
    pflow.asm
    pixel.c         poly.c
    rect.c          rle.c           rle.h
    scale.c         scalea.asm      scalemac.c
    scanline.c
    selector.asm
    tmapbits.c      tpoly.c
    vesalib.c       vesatest.c
)
add_library(gr ${gr_SRCS} ${gr_OTHERS})

if(NOT GR_NO_ASM)
    list(APPEND gr_SRCS lbitblt.asm linear.asm scale.c scalea.asm)
else()
    list(APPEND gr_SRCS scalemac.c) #macstubs.c
endif()

list(REMOVE_ITEM gr_OTHERS ${gr_SRCS})
set_source_files_properties(${gr_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

#add_executable(testg testg.c tmapbits.c)
#target_link_libraries(testg io gr misc mem iff cfile)

#add_executable(testp)
#target_link_libraries(testp io gr misc mem cflib)

#add_executable(vesatest vesatest.c)
#target_link_libraries(vesatest io gr misc mem cflib)
