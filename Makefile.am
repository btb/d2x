bin_PROGRAMS = @TARGETS@ hogcreate hogextract mveplayer mvlcreate mvlextract txb2tex tex2txb
EXTRA_PROGRAMS = d2x d2x-sdl d2x-gl d2x-svga miner miner-sdl miner-gl miner-svga

# Common to all d2x platforms/options

d2x_CPPFLAGS = -DFL1_WITH_FLAT -I$(top_srcdir)/include -I$(top_srcdir)/main -I$(top_srcdir)/arch/include

d2x_SOURCES = \
2d/2dsline.c  2d/box.c      2d/disc.c     2d/ibitblt.c  2d/pcx.c      2d/rect.c     2d/scalec.c   \
2d/bitblt.c   2d/canvas.c   2d/font.c     2d/gr.c       2d/line.c     2d/pixel.c    2d/rle.c      \
2d/bitmap.c   2d/circle.c   2d/gpixel.c   2d/palette.c  \
3d/clipper.c  3d/globvars.c 3d/interp.c   3d/points.c   3d/setup.c    3d/draw.c     3d/instance.c \
3d/matrix.c   3d/rod.c      \
arch/sdl/digi.c    arch/sdl/init.c    arch/sdl/midi.c    arch/sdl/rbaudio.c arch/sdl/timer.c   \
iff/iff.c     \
libmve/decoder16.c libmve/decoder8.c  libmve/mve_audio.c libmve/mvelib.c    libmve/mveplay.c   \
main/ai.c       main/ai2.c      main/aipath.c   main/automap.c  main/bm.c       main/cli.c      \
main/cmd.c      main/cntrlcen.c main/collide.c  main/config.c   main/console.c  main/controls.c \
main/credits.c  main/cvar.c     main/digiobj.c  main/effects.c  main/endlevel.c main/entity.c   \
main/escort.c   main/fireball.c main/fuelcen.c  main/fvi.c      main/game.c     main/gamecntl.c \
main/gamefont.c main/gamemine.c main/gamepal.c  main/gamerend.c main/gamesave.c main/gameseg.c  \
main/gameseq.c  main/gauges.c   main/hostage.c  main/hud.c      main/inferno.c  main/kconfig.c  \
main/kludge.c   main/laser.c    main/lighting.c main/menu.c     main/mglobal.c  main/mission.c  \
main/morph.c    main/movie.c    main/newdemo.c  main/newmenu.c  main/object.c   main/paging.c   \
main/physics.c  main/piggy.c    main/player.c   main/polyobj.c  main/powerup.c  main/render.c   \
main/robot.c    main/scores.c   main/segment.c  main/slew.c     main/songs.c    main/state.c    \
main/switch.c   main/terrain.c  main/texmerge.c main/text.c     main/titles.c   main/vclip.c    \
main/wall.c     main/weapon.c   \
maths/vecmat.c  maths/rand.c    maths/tables.c  \
mem/mem.c \
misc/args.c        misc/error.c       misc/strio.c       misc/ignorecase.c  misc/physfsrwops.c \
misc/hash.c        misc/hmp.c         misc/strutil.c     \
texmap/ntmap.c     texmap/scanline.c

d2x_LDFLAGS =
d2x_LDADD = $(PHYSFS_LIBS)

if MACDATA
TRANSPARENCY_COLOR = 0
else
TRANSPARENCY_COLOR = 255
endif

SUFFIXES = .asm
.asm.o:
	$(NASM) -dTRANSPARENCY_COLOR=${TRANSPARENCY_COLOR} $(NASMFLAGS) -Itexmap/ $< -o $@

if USE_ASM
d2x_SOURCES += texmap/tmappent.S texmap/tmapppro.S
d2x_LDADD += 2d/linear.o 2d/tmerge_a.o maths/vecmata.o maths/fix.o \
texmap/tmap_flt.o texmap/tmap_lin.o texmap/tmap_ll.o  texmap/tmap_per.o texmap/tmapfade.o
else
d2x_SOURCES += 2d/tmerge.c maths/fixc.c
endif

if USE_NETWORK
d2x_SOURCES += main/kmatrix.c main/multi.c main/multibot.c main/network.c main/netmisc.c
endif

_miner_CPPFLAGS = -DEDITOR
_miner_SOURCES = main/bmread.c main/dumpmine.c \
main/editor/autosave.c  main/editor/centers.c   main/editor/curves.c    main/editor/eglobal.c   \
main/editor/ehostage.c  main/editor/elight.c    main/editor/eobject.c   main/editor/eswitch.c   \
main/editor/fixseg.c    main/editor/func.c      main/editor/group.c     main/editor/info.c      \
main/editor/kbuild.c    main/editor/kcurve.c    main/editor/kfuncs.c    main/editor/kgame.c     \
main/editor/kgroup.c    main/editor/khelp.c     main/editor/kmine.c     main/editor/ksegmove.c  \
main/editor/ksegsel.c   main/editor/ksegsize.c  main/editor/ktmap.c     main/editor/kview.c     \
main/editor/macro.c     main/editor/med.c       main/editor/meddraw.c   main/editor/medmisc.c   \
main/editor/medrobot.c  main/editor/medsel.c    main/editor/medwall.c   main/editor/mine.c      \
main/editor/objpage.c   main/editor/segment.c   main/editor/seguvs.c    main/editor/texpage.c   \
main/editor/texture.c \
ui/barbox.c    ui/button.c    ui/checkbox.c  ui/file.c      ui/gadget.c    ui/icon.c      \
ui/inputbox.c  ui/keypad.c    ui/keypress.c  ui/keytrap.c   ui/listbox.c   ui/menu.c      \
ui/menubar.c   ui/message.c   ui/mouse.c     ui/number.c    ui/popup.c     ui/radio.c     \
ui/scroll.c    ui/ui.c        ui/uidraw.c    ui/userbox.c   ui/window.c

if USE_LIBPNG
d2x_SOURCES += misc/pngfile.c
endif


# Platform-specific things

if MSDOS
d2x_CPPFLAGS += -I$(top_srcdir)/arch/dos/include
d2x_SOURCES += arch/dos/digi.c arch/dos/dpmi.c arch/dos/findfile.c arch/dos/vid.c arch/dos/init.c arch/dos/ipx.c arch/dos/joyc.c arch/dos/joydefs.c arch/dos/key.c arch/dos/mouse.c arch/dos/rbaudio.c arch/dos/vesa.c
d2x_LDADD += arch/dos/timer.o arch/dos/modex.o
endif

miner_CPPFLAGS = $(d2x_CPPFLAGS) $(_miner_CPPFLAGS)
miner_SOURCES = $(d2x_SOURCES) $(_miner_SOURCES)
miner_LDFLAGS = $(d2x_LDFLAGS)
miner_LDADD = $(d2x_LDADD)


# Remaining targets are based on d2x_*

d2x_svga_CPPFLAGS = $(d2x_CPPFLAGS) -DSVGA -DSVGALIB_INPUT -DSVGALIB_VIDEO
d2x_svga_SOURCES = $(d2x_SOURCES) arch/svgalib/event.c arch/svgalib/vid.c arch/svgalib/key.c arch/svgalib/mouse.c arch/svgalib/init.c
d2x_svga_LDFLAGS = $(d2x_LDFLAGS)
d2x_svga_LDADD = $(d2x_LDADD) -lvga -lvgagl

miner_svga_CPPFLAGS = $(d2x_svga_CPPFLAGS) $(_miner_CPPFLAGS)
miner_svga_SOURCES = $(d2x_svga_SOURCES) $(_miner_SOURCES)
miner_svga_LDFLAGS = $(d2x_svga_LDFLAGS)
miner_svga_LDADD = $(d2x_svga_LDADD)


_d2x_sdl_CPPFLAGS = $(d2x_CPPFLAGS) $(SDL_CFLAGS) $(SDLMIXER_CFLAGS) -I$(top_srcdir)/arch/linux/include -DSDL_INPUT
_d2x_sdl_SOURCES = $(d2x_SOURCES) arch/sdl/event.c arch/sdl/key.c arch/sdl/mouse.c

d2x_sdl_CPPFLAGS = $(_d2x_sdl_CPPFLAGS) -DSDL_VIDEO
d2x_sdl_SOURCES = $(_d2x_sdl_SOURCES) arch/sdl/vid.c texmap/tmapflat.c
d2x_sdl_LDFLAGS = $(d2x_LDFLAGS)
d2x_sdl_LDADD = $(d2x_LDADD) $(SDL_LIBS) $(SDLIMAGE_LIBS) $(SDLMIXER_LIBS)

if USE_LINUX_JOY
_d2x_sdl_SOURCES += arch/linux/joystick.c arch/linux/joydefs.c
else
_d2x_sdl_SOURCES += arch/sdl/joy.c arch/sdl/joydefs.c
endif

if USE_NETWORK
_d2x_sdl_SOURCES += arch/linux/ipx_mcast4.c arch/linux/ipx_udp.c arch/linux/linuxnet.c
endif

if MINGW32
_d2x_sdl_SOURCES += arch/win32/mingw_init.c
d2x_sdl_LDADD += -lwinmm

if USE_NETWORK
_d2x_sdl_SOURCES += arch/win32/ipx_win.c
d2x_sdl_LDADD += -lwsock32
endif

else # !MINGW32

_d2x_sdl_SOURCES += arch/linux/init.c
d2x_sdl_LDADD += -lm

if USE_NETWORK

if USE_NATIVE_IPX
_d2x_sdl_SOURCES += arch/linux/ipx_bsd.c
endif

if USE_KALINIX
_d2x_sdl_SOURCES += arch/linux/ipx_kali.c arch/linux/ukali.c
endif

endif # USE_NETWORK

endif # !MINGW32

if MACOSX
#d2x_sdl_LDFLAGS += -all_load
endif

miner_sdl_CPPFLAGS = $(d2x_sdl_CPPFLAGS) $(_miner_CPPFLAGS)
miner_sdl_SOURCES = $(d2x_sdl_SOURCES) $(_miner_SOURCES)
miner_sdl_LDFLAGS = $(d2x_sdl_LDFLAGS)
miner_sdl_LDADD = $(d2x_sdl_LDADD)


# GL target is based on d2x_sdl_*, other than renderer

d2x_gl_CPPFLAGS = $(d2x_sdl_CPPFLAGS) -DOGL -DSDL_GL_VIDEO
d2x_gl_SOURCES = $(_d2x_sdl_SOURCES) arch/ogl/vid.c arch/ogl/ogl.c arch/ogl/sdlgl.c
d2x_gl_LDFLAGS = $(d2x_sdl_LDFLAGS)
d2x_gl_LDADD = $(d2x_sdl_LDADD)

if MACOSX
d2x_gl_LDFLAGS += -framework OpenGL
endif

if MINGW32
d2x_gl_LDADD += -lopengl32 -lglu32
else
if !MACOSX
d2x_gl_LDADD += -lGL -lGLU
endif
endif

miner_gl_CPPFLAGS = $(d2x_gl_CPPFLAGS) $(_miner_CPPFLAGS)
miner_gl_SOURCES = $(d2x_gl_SOURCES) $(_miner_SOURCES)
miner_gl_LDFLAGS = $(d2x_gl_LDFLAGS)
miner_gl_LDADD = $(d2x_gl_LDADD)


# Other programs

mveplayer_SOURCES = libmve/decoder16.c libmve/decoder8.c libmve/mve_audio.c libmve/mve_main.c libmve/mvelib.c libmve/mveplay.c
mveplayer_MANS = libmve/mveplayer.1
mveplayer_CFLAGS = -I$(top_srcdir)/include $(SDL_CFLAGS) $(SDLMIXER_CFLAGS)
mveplayer_LDADD = $(SDL_LIBS) $(SDLMIXER_LIBS)

hogcreate_SOURCES = utilities/hogcreate.c
hogcreate_MANS = utilities/hogcreate.1

hogextract_SOURCES = utilities/hogextract.c
hogextract_MANS = utilities/hogextract.1

mvlcreate_SOURCES = utilities/mvlcreate.c
mvlcreate_MANS = utilities/mvlcreate.1

mvlextract_SOURCES = utilities/mvlextract.c
mvlextract_MANS = utilities/mvlextract.1

txb2tex_SOURCES = utilities/txb2tex.c
txb2tex_MANS = utilities/txb2tex.1

tex2txb_SOURCES = utilities/tex2txb.c
tex2txb_MANS = utilities/tex2txb.1


pkgdatadir = @sharepath@
missiondir = ${pkgdatadir}/missions

CLEANFILES = Info.plist
