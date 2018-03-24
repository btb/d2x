#
# Makefile for Descent libraries
#

enable_language(C ASM_MASM)

set(TARGSYS "WIN32" CACHE STRING "one of WIN32 DOS4G CAUSEWAY")
set(COMPILER "WATCOM" CACHE STRING "one of WATCOM SYMANTEC TURBOC")

if(WATCOM)
    set(CMAKE_ASM_MASM_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY})
    if(${TARGSYS} MATCHES "^DOS4G$")
        set(CMAKE_C_FLAGS "-bt=dos -zp1" CACHE STRING "" FORCE)
        set(CMAKE_CREATE_CONSOLE_EXE "system dos4g" )
    endif()
endif()

option(RELEASE_VERSION  "turn off test, cheat, & debugging functions" ON)
option(NMONO            "is monochrome debugging turned off?" ON)
option(NDEBUG           "is debugging turned off? (kills Asserts, Int3s, etc.)" ON)
option(ARCADE           "is the Arcade/AWD code in?" OFF)

if(TARGSYS MATCHES "^WIN32$")
    add_definitions(-dWINDOWS)
endif()
if(RELEASE_VERSION)
    add_definitions(-dRELEASE)
endif()
if(NDEBUG)
    add_definitions(-dNDEBUG)
endif()
if(NMONO)
    add_definitions(-dNMONO)
endif()
if(ARCADE)
    add_definitions(-dARCADE)
endif()

find_path(SOS_DIGI_INCLUDE_DIR sos.h ../extlib/soslib)
find_library(SOS_DIGI_LIBRARIES sosdigi ../extlib/soslib)

find_path(SOS_MIDI_INCLUDE_DIR sosm.h ../extlib/soslib)
find_library(SOS_MIDI_LIBRARIES sosmidi ../extlib/soslib)

find_path(3DFX_INCLUDE_DIR 3dfx.h ../extlib/glide/dos/include/)

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
