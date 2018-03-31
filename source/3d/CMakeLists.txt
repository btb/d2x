#
# Makefile for 3d library
#

set(3d_SRCS)

set(3d_OTHERS
    3d.h            3d.inc
    3dtest.c        3dtest.lnk
    clipper.asm     clipper.h       clipperc.c
    d.bat           dd.bat
    draw.asm        draw_seg.c      drawc.c
    globvars.asm    globvars.h      globvarsc.c
    horizon.asm     horizonc.c
    instance.asm    instancec.c
    interp.asm      interpc.c
    m.bat           makefile        makefile.mac
    matrix.asm      matrixc.c
    objects.asm     out.c
    points.asm      pointsc.c
    rod.asm         rodc.c
    segment.c       setup.asm       setupc.c
    t.bat           trakball.c
    upvecs.c        upvecs2.c
    vecview.c       vecview2.c
)

if(TARGSYS MATCHES "^DOS4G$" OR TARGSYS MATCHES "^WIN32")
    list(APPEND 3d_SRCS
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
else()
    list(APPEND 3d_SRCS
        clipperc.c  horizonc.c  matrixc.c   rodc.c
        drawc.c     instancec.c pointsc.c   setupc.c
        globvarsc.c interpc.c
    )
endif()

list(REMOVE_ITEM 3d_OTHERS ${3d_SRCS})
set_source_files_properties(${3d_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(3d ${3d_SRCS} ${3d_OTHERS})

#add_executable(3dtest 3dtest.c segment.c trakball.c)
#target_link_libraries(3dtest 3d vecmat gr io fix div0 texmap misc bm)
