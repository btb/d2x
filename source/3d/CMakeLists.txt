#
# Makefile for 3d library
#

set(3d_SRCS
    clipper.asm
    draw.asm
    globvars.asm
    horizon.asm
    instance.asm
    interp.asm
    matrix.asm
    points.asm
    rod.asm
    setup.asm
)

set(3d_OTHERS
    3d.h
    3d.inc
    3dtest.c
    draw_seg.c
    makefile
    objects.asm
    out.c
    segment.c
    trakball.c
    upvecs.c
    upvecs2.c
    vecview.c
    vecview2.c
)
set_source_files_properties(${3d_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(3d ${3d_SRCS} ${3d_OTHERS})

#add_executable(3dtest 3dtest.c segment.c trakball.c)
#target_link_libraries(3dtest 3d vecmat gr io fix div0 texmap misc bm)
