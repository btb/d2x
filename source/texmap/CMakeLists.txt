#
# Makefile for texture map library
#

option(NASM          	"Build with only C code?" OFF)

if(NASM)
    add_definitions(-DNASM)
endif()

set(texmap_SRCS
    ntmap.c
    tmapflat.c
)

set(texmap_OTHERS
    makefile
    ntmap.new       ntmapout.h
    scanline.c      scanline.h
    texmap.h        texmapl.h
    tmap_inc.asm
    tmap_per.asm    tmap_flt.asm    tmapfade.asm    tmap_ed.asm
    tmap_pnn.asm    tmap_pln.asm    tmap_plt.asm    tmap_pnt.asm
    tmap_qnn.asm    tmap_qln.asm    tmap_qlt.asm    tmap_qnt.asm
    tmap_lnn.asm    tmap_lln.asm    tmap_llt.asm    tmap_lnt.asm
)

if(NOT NASM)
    list(APPEND texmap_SRCS
        tmap_per.asm    tmap_flt.asm    tmapfade.asm    tmap_ed.asm
        tmap_pnn.asm    tmap_pln.asm    tmap_plt.asm    tmap_pnt.asm
        tmap_qnn.asm    tmap_qln.asm    tmap_qlt.asm    tmap_qnt.asm
        tmap_lnn.asm    tmap_lln.asm    tmap_llt.asm    tmap_lnt.asm
    )
else()
    list(APPEND texmap_SRCS scanline.c)
endif()

list(REMOVE_ITEM texmap_OTHERS ${texmap_SRCS})
set_source_files_properties(${texmap_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(texmap ${texmap_SRCS} ${texmap_OTHERS})
