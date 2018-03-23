#
# Makefile for DESCENT execuable
#

set(TARGSYS "WIN32" CACHE STRING "one of WIN32 DOS4G CAUSEWAY")
set(COMPILER "WATCOM" CACHE STRING "one of WATCOM SYMANTEC TURBOC")

find_path(SOS_DIGI_INCLUDE_DIR sos.h ../../extlib/soslib)
find_library(SOS_DIGI_LIBRARIES sosdigi ../../extlib/soslib)

find_path(SOS_MIDI_INCLUDE_DIR sosm.h ../../extlib/soslib)
find_library(SOS_MIDI_LIBRARIES sosmidi ../../extlib/soslib)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SOSDigi DEFAULT_MSG SOS_DIGI_LIBRARIES SOS_DIGI_INCLUDE_DIR)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SOSMidi DEFAULT_MSG SOS_MIDI_LIBRARIES SOS_MIDI_INCLUDE_DIR)

mark_as_advanced(
  SOS_DIGI_INCLUDE_DIR SOS_DIGI_LIBRARIES
  SOS_MIDI_INCLUDE_DIR SOS_MIDI_LIBRARIES
)

# These define changes in the code

option(RELEASE_VERSION  "turn off test, cheat, & debugging functions" ON)
option(NMONO            "is monochrome debugging turned off?" ON)
#option(NDEBUG           "is debugging turned off? (kills Asserts, Int3s, etc.)" ON)
if(NOT CMAKE_BUILD_TYPE MATCHES "^Debug$")
    set(NDEBUG ON)
endif()

option(D2_OEM           "8-level OEM version?" OFF)

option(_3DFX            "is the 3Dfx code compiled in?" OFF)
option(TACTILE          "are tactile-feedback joysticks supported?" OFF)

option(SHAREWARE        "is this the smaller shareware" OFF)
option(BUILD_EDITOR     "is the editor included?" OFF)
option(NO_MULTIPLAYER   "turn off networking" OFF)

# These never really get changed

option(PIGGY_NO_PAGING  "don't use John's new paging code" OFF)
option(STORE_DEMO       "is the one-level store demo in?" OFF)
option(ARCADE           "is the Arcade/AWD code in?" OFF)
option(MARK_ON          "are marks on?" OFF)
#set(PASSWORD "RUST" CACHE STRING "is a password required?")
option(DEMO_ONLY        "is this a non-playable demo?" OFF)


# These define changes in compiling & linking
# Undefining DEBUG_ON will kill symbols and source

option(OPTIMIZE_ON      "build optimized" ON)
option(DEBUG_ON         "debug" OFF)

option(STACK_CHECK_ON   "re-enable stack check" OFF)

#set(VERSION_NAME "CD Burn Test" CACHE STRING "This defines the name of this version")


# What programs to make
if(TARGSYS MATCHES "^WIN32$")
    set(PROGS descentw)
elseif(SHAREWARE)
    set(PROGS d2share)
else()
    set(PROGS descent2)
endif()

# Files to link for Descent
set(descent2_SRCS
    game.c gamerend.c gamecntl.c gauges.c mglobal.c
    scores.c object.c laser.c physics.c bm.c
    menu.c gamesave.c switch.c effects.c texmerge.c polyobj.c
    gamemine.c fireball.c ai.c ai2.c aipath.c hostage.c
    powerup.c fuelcen.c render.c piggy.c digiobj.c
    gameseg.c wall.c collide.c lighting.c
    robot.c morph.c vclip.c weapon.c fvi.c newdemo.c titles.c
    gameseq.c controls.c automap.c text.c
    network.c newmenu.c netmisc.c
    gamefont.c hud.c playsave.c
    endlevel.c kconfig.c crypt.c
    multi.c cntrlcen.c credits.c config.c kmatrix.c
    paging.c mission.c songs.c bmread.c
    escort.c gamepal.c multibot.c state.c
    cdmix32.c
)
#    vfx.c vfxread.c #missing
#    3dmax.c #missing
#    demo.c garage.c radar.c coindev.c serial.c hash.c args.c

set(descent2_OTHERS
    3dfx.c      3dfx_des.h
    ai.c        ai.h        ai2.c       aipath.c    aistruct.h
    arcade.c    arcade.h    automap.c   automap.h
    bm.c        bm.h        bmread.c    bmread.h
    cdmix32.c   cdrom.c     cdrom.h
    cntrlcen.c  cntrlcen.h  coindev.c   coindev.h   collide.c   collide.h
    compbit.h   config.c    config.h    controls.c  controls.h
    credits.c   credits.h   crypt.c
    desc_id.h   descentw.c  desw.h      digi.c      digi.h      digiobj.c
    dumpmine.c
    effects.c   effects.h   endlevel.c  endlevel.h  escort.c
    files.h     fireball.c  fireball.h  fuelcen.c   fuelcen.h
    fvi.c       fvi.h
    game.c      game.h      gamecntl.c  gamefont.c  gamefont.h
    gamemine.c  gamemine.h  gamepal.c   gamepal.h   gamerend.c
    gamesave.c  gamesave.h  gameseg.c   gameseg.h   gameseq.c   gameseq.h
    gamestat.h  gauges.c    gauges.h
    hostage.c   hostage.h   hud.c
    iglasses.c  iglasses.h
    inferno.c   inferno.h   inferno.ini
    joydefs.c   joydefs.h   joydefsw.c
    kconfig.c   kconfig.h   kmatrix.c   kmatrix.h
    laser.c     laser.h     lighting.c  lighting.h
    makefile
    menu.c      menu.h      mglobal.c
    mission.c   mission.h   modem.c     modem.h     morph.c     morph.h
    movie.c     movie.h     multi.c     multi.h     multibot.c  multibot.h
    netmisc.c   netmisc.h   network.c   network.h   newdemo.c   newdemo.h
    newmenu.c   newmenu.h   newmodel.c
    object.c    object.h
    paging.c    paging.h    physics.c   physics.h   piggy.c     piggy.h
    player.h    playsave.c  playsave.h  polyobj.c   polyobj.h
    powerup.c   powerup.h
    rbtest.c    render.c    render.h    robot.c     robot.h
    scores.c    scores.h    screens.h   segment.h   segpoint.h
    slew.c      slew.h      songs.c     songs.h     sounds.h
    state.c     state.h     switch.c    switch.h
    terrain.c   terrain.h   texmerge.c  texmerge.h  text.c      text.h
    textures.h  titles.c    titles.h
    vclip.c     vclip.h
    wall.c      wall.h      weapon.c    weapon.h
    windigi.c   winferno.c  winmodem.c  winvideo.c
    settings.h.in           settings.h.orig
    vers_id.h.in            vers_id.h.orig
    editor/editor.h         editor/kdefs.h          editor/texpage.h
)

if(TARGSYS MATCHES "^WIN32$")
    list(APPEND descent2_SRCS winferno.c descentw.c winmodem.c joydefsw.c windigi.c) #winmovie.c
else()
    list(APPEND descent2_SRCS digi.c inferno.c joydefs.c movie.c) #victor.c iglasses.c modem.c
endif()

if(ARCADE)
    list(APPEND descent2_SRCS arcade.c)
endif()

if(NOT NDEBUG)
    list(APPEND descent2_SRCS dumpmine.c)
endif()

if(NOT RELEASE_VERSION)
    list(APPEND descent2_SRCS slew.c)
endif()

if(SHAREWARE)
    list(APPEND descent terrain.c)
endif()

# These sources are always built optimized
#descent2_SRCS_OPT = ## gameseg.c wall.c

set(descent2_LIBS
    3d gr fix iff vecmat
    div0 misc
    cfile
    texmap
)
#   iglasses loadpats vidlib unarj
#   lzw sosdwxcr sosmwxcr 

if(TACTILE AND NOT TARGSYS MATCHES "^WIN32$")
    list(APPEND descent2_LIBS tactile)
endif()

if(NOT NDEBUG)
    list(APPEND descent2_LIBS mem)
endif()

if(TARGSYS MATCHES "^WIN32$")
    list(APPEND descent2_LIBS
        win95
        ddraw dsound mvelibw
        novga tapi32
    )

    if(NOT TACTILE)
        list(APPEND descent2_LIBS iforce)
    endif()

    if(NOT RELEASE_VERSION)
        list(APPEND descent2_LIBS eregreg2)
    endif()

else()
    list(APPEND descent2_LIBS
        io vga ${SOS_DIGI_LIBRARIES} ${SOS_MIDI_LIBRARIES}
        poly_acc
    )
    # gclfr32 mvelb32s s3dtkwdr vlib spd3d
endif()

set(descent2_DIRS editor)

# Link flags for Descent
#descent2_LFLAGS = option map,verbose,stack=50k

# Files in the editor subdirectory
if(BUILD_EDITOR)

    list(APPEND EDITOR_SRCS
        med.c einit.c eglobal.c mine.c curves.c kcurve.c
        khelp.c ksegsel.c kbuild.c kview.c kmine.c seguvs.c ksegsize.c
        ksegmove.c ktmap.c kfuncs.c autosave.c group.c texture.c
        fixseg.c eobject.c elight.c texpage.c objpage.c medmisc.c
        medsel.c kgame.c info.c medwall.c eswitch.c medrobot.c meddraw.c
        segment.c centers.c
    )
    #   ehostage.c 

    # These sources are always built optimized
    #EDITOR_SRCS_OPT =  

    list(APPEND descent2_LIBS ui) #medlisp

    # Editor data files to check out with source
    list(APPEND EDITOR_DATA
        segmove.pad segsize.pad texture.pad object.pad curve.pad
        group.pad objmov.pad lighting.pad test.pad med.mnu global.key
    )
else()
    set(EDITOR_SRCS)
endif()


if(WATCOM)
    if(TARGSYS MATCHES "^DOS4G$")
        add_definitions(/bt=dos)
    endif()
endif()

# Force include of settings for all files
if(MSVC)
    add_definitions(/FI"settings.h")
elseif(WATCOM)
    add_definitions(/fi=settings.h)
elseif(BORLAND)
    add_definitions(/HIsettings.h)
else()
    # GCC or Clang
    add_definitions(-include settings.h)
endif()

# DescentW is really Descent 2
set(descentw_SRCS   ${descent2_SRCS})
set(descentw_LIBS   ${descent2_LIBS})
set(descentw_DIRS   ${descent2_DIRS})
set(descentw_LFLAGS ${descent2_LFLAGS})
set(descentw_DATA   ${descent2_DATA})

# D2Share is really Descent 2
set(d2share_SRCS   ${descent2_SRCS})
set(d2share_LIBS   ${descent2_LIBS})
set(d2share_DIRS   ${descent2_DIRS})
set(d2share_LFLAGS ${descent2_LFLAGS})
set(d2share_DATA   ${descent2_DATA})

list(REMOVE_ITEM descent2_OTHERS ${descent2_SRCS})

set_source_files_properties(${descent2_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(${PROGS} ${descent2_SRCS} ${descent2_OTHERS})

target_link_libraries(${PROGS} ${descent2_LIBS})

include_directories(../pa_null)
include_directories(../includes)
include_directories(../vecmat)
include_directories(../fix)
include_directories(../2d)
include_directories(../bios)
include_directories(../iff)
include_directories(../3d)
include_directories(../texmap)
include_directories(../mem)
include_directories(../misc)
include_directories(../vga)
include_directories(../cfile)
include_directories(../div)
if(TARGSYS MATCHES "^WIN32$")
    include_directories(../win95)
endif()

if(SOS_DIGI_INCLUDE_DIR)
    include_directories(${SOS_DIGI_INCLUDE_DIR})
endif()
if(SOS_MIDI_INCLUDE_DIR)
    include_directories(${SOS_MIDI_INCLUDE_DIR})
endif()


# Check various settings for conflicts
if(BUILD_EDITOR AND NDEBUG)
    message(FATAL_ERROR "Error in ${INI_FILE}: If BUILD_EDITOR defined, NDEBUG must not be.")
endif()
if(ARCADE AND NOT NEWDEMO)
    message(FATAL_ERROR "Error in ${INI_FILE}: ARCADE option requires that NEWDEMO also be set.")
endif()
if(DEMO_ONLY AND NOT NEWDEMO)
    message(FATAL_ERROR "Error in ${INI_FILE}: DEMO_ONLY defined, but NEWDEMO not.")
endif()

if(BUILD_EDITOR)
    set(EDITOR ON)
endif()
if(RELEASE_VERSION)
    set(RELEASE ON)
endif()
if(TARGSYS MATCHES "^WIN32$")
    set(WINDOWS 1)
endif()
if(MARK_ON)
    set(_MARK_ON ON)
endif()
if(NOT NO_MULTIPLAYER)
    set(NETWORK ON)
endif()
if(NOT PIGGY_NO_PAGING)
    set(PIGGY_USE_PAGING ON)
endif()

configure_file(settings.h.in settings.h)

#set up serial number and name header file
if(NOT DEFINED VERSION_NAME AND NOT RELEASE_VERSION)
    set(VERSION_NAME "Internal Development (${USER})")
endif()
if(SHAREWARE)
    set(VERSION_TYPE "Interactive Demo")
elseif(D2_OEM)
    set(VERSION_TYPE "Destination:Quartzon")
elseif(COMPILATION)
    set(VERSION_TYPE "Full Version (Comp)")
else()
    set(VERSION_TYPE "Full Version")
endif()

configure_file(vers_id.h.in vers_id.h)
include_directories("${PROJECT_BINARY_DIR}/source/main")
