#
# Makefile for Descent libraries
#

enable_language(C ASM_MASM)

if(NOT CMAKE_BUILD_TYPE MATCHES "^Debug$")
    #add_definitions(/DNDEBUG)
endif()

find_path(3DFX_INCLUDE_DIR 3dfx.h /3Dfx/Sdk/Glide3x/Src/Include/)
mark_as_advanced(3DFX_INCLUDE_DIR)


add_subdirectory(includes)
add_subdirectory(misc)
add_subdirectory(mem)
add_subdirectory(fix)       #Assembly only
add_subdirectory(cfile)
add_subdirectory(pa_null)
add_subdirectory(2d)
add_subdirectory(vga)       #VGA only
add_subdirectory(bios)      #DOS only
add_subdirectory(iff)
add_subdirectory(div)       #Assembly only
#add_subdirectory(ui)       #DOS only
add_subdirectory(vecmat)    #Assembly only
add_subdirectory(3d)        #Assembly only
add_subdirectory(texmap)
#add_subdirectory(win95)    #Windows only
add_subdirectory(novga)     #Windows only
#add_subdirectory(tactile)  #Don't have
#add_subdirectory(unarj)    #Don't have
#add_subdirectory(sosdigi)  #Don't have
#add_subdirectory(sosmidi)  #Don't have

add_subdirectory(main)
