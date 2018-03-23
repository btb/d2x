#
# Makefile for texture map library
#

#Uncomment the following 3 lines if you want to build with only C code.
set(NASM 1)
#set(GLOBAL_CFLAGS /dNASM)
#set(GLOBAL_AFLAGS /DNASM)

set(texmap_SRCS
	ntmap.c
	tmapflat.c
)

set(texmap_OTHERS
	dd.bat
	makefile
	ntmap.new
	ntmapout.h
	scanline.h
	texmap.h
	texmapl.h
	tmap_inc.asm
)
set_source_files_properties(${texmap_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

if(NOT DEFINED NASM)
    list(APPEND texmap_SRCS
        tmap_per.asm tmap_flt.asm tmapfade.asm tmap_ed.asm
        tmap_pnn.asm tmap_pln.asm tmap_plt.asm tmap_pnt.asm
        tmap_qnn.asm tmap_qln.asm tmap_qlt.asm tmap_qnt.asm
        tmap_lnn.asm tmap_lln.asm tmap_llt.asm tmap_lnt.asm
    )
	list(APPEND texmap_OTHERS
		scanline.c
	)
else()
	list(APPEND texmap_OTHERS
		tmap_per.asm tmap_flt.asm tmapfade.asm tmap_ed.asm
		tmap_pnn.asm tmap_pln.asm tmap_plt.asm tmap_pnt.asm
		tmap_qnn.asm tmap_qln.asm tmap_qlt.asm tmap_qnt.asm
		tmap_lnn.asm tmap_lln.asm tmap_llt.asm tmap_lnt.asm
	)
    list(APPEND texmap_SRCS scanline.c)
endif()

add_library(texmap ${texmap_SRCS} ${texmap_OTHERS})

include_directories(../pa_null)
include_directories(../bios)
include_directories(../fix)
include_directories(../includes)
include_directories(../3d)
include_directories(../vecmat)
include_directories(../2d)
