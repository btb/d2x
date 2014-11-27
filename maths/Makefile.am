noinst_LIBRARIES = libmaths.a
AM_CPPFLAGS = -I $(top_srcdir)/include

if USE_ASM
libmaths_a_SOURCES = vecmat.c rand.c tables.c
else
libmaths_a_SOURCES = vecmat.c rand.c tables.c fixc.c
endif

SUFFIXES = .asm
.asm.o:
	$(NASM) $(NASMFLAGS) $< -o $@

if USE_ASM
libmaths_a_LIBADD = vecmata.o fix.o
endif

EXTRA_DIST = \
fix.asm vecmata.asm \
fixc.c rand.c tables.c vecmat.c
