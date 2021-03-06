/* conf.h.  Generated from conf.h.in by configure.  */
/* conf.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* d2x major version */
#define D2XMAJOR 0

/* d2x micro version */
#define D2XMICRO 6

/* d2x minor version */
#define D2XMINOR 2

/* Define for faster i/o on little-endian cpus */
/* #undef FAST_FILE_IO */

/* Define to 1 if you have the declaration of `nanosleep', and to 0 if you
   don't. */
#define HAVE_DECL_NANOSLEEP 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <netipx/ipx.h> header file. */
/* #undef HAVE_NETIPX_IPX_H */

/* Define to 1 if you have the <physfs.h> header file. */
#define HAVE_PHYSFS_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if the system has the type `struct timespec'. */
#define HAVE_STRUCT_TIMESPEC 1

/* Define to 1 if the system has the type `struct timeval'. */
#define HAVE_STRUCT_TIMEVAL 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to enable use of the KaliNix driver */
#define KALINIX /**/

/* Define if you want to build for mac datafiles */
/* #undef MACDATA */

/* Define to use the IPX support of the OS */
/* #undef NATIVE_IPX */

/* Define to disable asserts, int3, etc. */
/* #undef NDEBUG */

/* Define if you want a network build */
#define NETWORK /**/

/* Define if you want an assembler free build */
#define NO_ASM /**/

/* Name of package */
#define PACKAGE "d2x"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "descent-source@warpcore.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "d2x"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "d2x 0.2.6"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "d2x"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.2.6"

/* Define for a "release" build */
/* #undef RELEASE */

/* Define if you have the SDL_image library */
/* #undef SDL_IMAGE */

/* Define this to be the shared game directory root */
#define SHAREPATH "/usr/local/share/games/d2x"

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* define to not use the SDL_Joystick routines. */
/* #undef USE_LINUX_JOY */

/* Version number of package */
#define VERSION "0.2.6"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define if your processor needs data to be word-aligned */
/* #undef WORDS_NEED_ALIGNMENT */


        /* General defines */
#ifndef PACKAGE_STRING
#define PACKAGE_STRING "d2x 0.2.6"
#endif
#define VERSION_NAME PACKAGE_STRING
#define NMONO 1
#define PIGGY_USE_PAGING 1
#define NEWDEMO 1

#if defined(__APPLE__) && defined(__MACH__)
#define __unix__ /* since we're doing a unix-style compilation... */
#endif

