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
    add_definitions(-DUSE_INLINE)
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

set(SUBSYSTEMS includes misc mem fix cfile 2d iff vecmat 3d texmap)

set(SUBSYSTEMS_OTHERS vga div ui win95 novga) # tactile unarj sosdigi sosmidi

option(PA_NULL          "Build Polygon Accelleration library" OFF)
if(PA_NULL)
    add_definitions(-DPOLY_ACC)
    list(APPEND SUBSYSTEMS pa_null)
endif()
include_directories(pa_null)

if(TARGSYS MATCHES "^DOS4G$")
    list(APPEND SUBSYSTEMS bios vga div ui) # tactile unarj sosdigi sosmidi

elseif(TARGSYS MATCHES "^WIN32$")
    add_definitions(-DWINDOWS)
    include_directories(vga)
    list(APPEND SUBSYSTEMS bios novga win95) # tactile unarj

elseif(TARGSYS MATCHES "^OSX$")
    include_directories(bios vga)
    list(APPEND SUBSYSTEMS novga)

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
