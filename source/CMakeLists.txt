#
# Makefile for Descent libraries
#

enable_language(C ASM_MASM)

set(CMAKE_ASM_MASM_FLAGS "-omf")

if(CMAKE_SYSTEM_NAME MATCHES "^Darwin$")
    set(TARGSTRING "OSX")
elseif(CMAKE_SYSTEM_NAME MATCHES "^Windows$")
    set(TARGSTRING "WIN32")
else()
    set(TARGSTRING "DOS4G")
endif()

set(TARGSYS ${TARGSTRING} CACHE STRING "one of WIN32 DOS4G CAUSEWAY")

if(WATCOM)
    set(CMAKE_ASM_MASM_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY})
    if(TARGSYS MATCHES "^DOS4G$")
        set(CMAKE_C_FLAGS "-bt=dos -zp1" CACHE STRING "" FORCE)
        set(CMAKE_CREATE_CONSOLE_EXE "system dos4g")
    elseif(TARGSYS MATCHES "^WIN32$")
        set(CMAKE_C_FLAGS "-bt=nt /w2 -zp1" CACHE STRING "" FORCE)
        set(CMAKE_CREATE_CONSOLE_EXE "system win95")
    endif()
elseif(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_definitions(-fpack-struct=1)
endif()

option(RELEASE_VERSION  "turn off test, cheat, & debugging functions" ON)
option(NMONO            "is monochrome debugging turned off?" ON)
option(NDEBUG           "is debugging turned off? (kills Asserts, Int3s, etc.)" ON)
option(ARCADE           "is the Arcade/AWD code in?" OFF)
option(TACTILE          "are tactile-feedback joysticks supported?" OFF)

if(RELEASE_VERSION)
    add_definitions(-DRELEASE)
endif()
if(NDEBUG)
    add_definitions(-DNDEBUG)
endif()
if(NMONO)
    add_definitions(-DNMONO)
endif()
if(ARCADE)
    add_definitions(-DARCADE)
endif()

option(GR_NO_ASM        "Build with only C code?" OFF)
if(GR_NO_ASM)
	add_definitions(-DGR_NO_ASM)
endif()

option(3D_NO_ASM        "Build with only C code?" OFF)

option(FIX_NO_ASM       "Build with only C code?" OFF)
if(NOT FIX_NO_ASM)
	add_definitions(-DUSE_INLINE)
endif()

set(SUBSYSTEMS includes misc mem fix cfile 2d bios iff vecmat 3d texmap vid)

set(SUBSYSTEMS_OTHERS div ui win95) # tactile unarj sosdigi sosmidi

option(PA_NULL          "Build Polygon Accelleration library" OFF)
if(PA_NULL)
    add_definitions(-DPOLY_ACC)
    list(APPEND SUBSYSTEMS pa_null)
endif()
include_directories(pa_null)

if(TARGSYS MATCHES "^DOS4G$")
    list(APPEND SUBSYSTEMS div ui) # tactile unarj sosdigi sosmidi

elseif(TARGSYS MATCHES "^WIN32$")
    add_definitions(-DWINDOWS)
    list(APPEND SUBSYSTEMS win95) # tactile unarj

    find_path(DIRECTX_INCLUDE_DIR d3d.h ../../extlib/dxsdk/include/)

else()
    find_package(SDL2 REQUIRED)
    include(FindPkgConfig)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    pkg_check_modules(SDL2_NET REQUIRED SDL2_net)

endif()

include_directories(${SUBSYSTEMS})

foreach(SUBSYSTEM IN LISTS SUBSYSTEMS)
    add_subdirectory(${SUBSYSTEM})
endforeach()

list(REMOVE_ITEM SUBSYSTEMS_OTHERS ${SUBSYSTEMS})
foreach(SUBSYSTEM IN LISTS SUBSYSTEMS_OTHERS)
    add_subdirectory(${SUBSYSTEM} EXCLUDE_FROM_ALL)
endforeach()

add_subdirectory(main)
