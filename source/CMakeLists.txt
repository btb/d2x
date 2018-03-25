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
        set(CMAKE_CREATE_CONSOLE_EXE "system dos4g")
    endif()
    if(${TARGSYS} MATCHES "^WIN32$")
        set(CMAKE_C_FLAGS "-bt=nt /w2 -zp1" CACHE STRING "" FORCE)
        set(CMAKE_CREATE_CONSOLE_EXE "system win95")
    endif()
endif()

option(RELEASE_VERSION  "turn off test, cheat, & debugging functions" ON)
option(NMONO            "is monochrome debugging turned off?" ON)
option(NDEBUG           "is debugging turned off? (kills Asserts, Int3s, etc.)" ON)
option(ARCADE           "is the Arcade/AWD code in?" OFF)

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

# Subdirs to make for the DOS libraries
set(SUBSYSTEMS_DOS includes misc mem fix cfile pa_null 2d vga bios iff div
    ui vecmat 3d texmap) # tactile unarj sosdigi sosmidi

# Subdirs to make for the Win95 libraries
set(SUBSYSTEMS_WIN includes misc mem fix cfile pa_null 2d vga novga iff div
    vecmat win95 3d texmap) # tactile unarj

# All subdirs.  Needed for clean to work
set(SUBSYSTEMS_OTHERS includes misc mem fix cfile pa_null 2d vga bios iff div
    ui vecmat 3d texmap win95 novga) # tactile unarj sosdigi sosmidi

if(TARGSYS MATCHES "^DOS4G$")
    foreach(SUBSYSTEM IN LISTS SUBSYSTEMS_DOS)
        include_directories(${SUBSYSTEM})
    endforeach()
    foreach(SUBSYSTEM IN LISTS SUBSYSTEMS_DOS)
        add_subdirectory(${SUBSYSTEM})
    endforeach()
    list(REMOVE_ITEM SUBSYSTEMS_OTHERS ${SUBSYSTEMS_DOS})

elseif(TARGSYS MATCHES "^WIN32$")
    add_definitions(-DWINDOWS)
    foreach(SUBSYSTEM IN LISTS SUBSYSTEMS_WIN)
        include_directories(${SUBSYSTEM})
    endforeach()
    include_directories(bios)
    foreach(SUBSYSTEM IN LISTS SUBSYSTEMS_WIN)
        add_subdirectory(${SUBSYSTEM})
    endforeach()
    list(REMOVE_ITEM SUBSYSTEMS_OTHERS ${SUBSYSTEMS_WIN})

endif()

foreach(SUBSYSTEM IN LISTS SUBSYSTEMS_OTHERS)
    add_subdirectory(${SUBSYSTEM} EXCLUDE_FROM_ALL)
endforeach()

add_subdirectory(main)
