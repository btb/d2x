/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.  
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

#ifndef _TYPES_H
#define _TYPES_H

// define a dboolean
typedef int dboolean;

//define a byte 
typedef signed char byte;

//define unsigned types;
typedef unsigned char ubyte;
#ifdef __ENV_LINUX__
# include <sys/types.h>
# define _MAX_PATH 1024
# define _MAX_DIR 256
#define min(a,b) (((a)>(b))?(b):(a))
#define max(a,b) (((a)<(b))?(b):(a))
#else
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#endif

//define a boolean
typedef ubyte bool;

#ifndef NULL
#define NULL 0
#endif

#ifdef __GNUC__
#define __pack__ __attribute__((packed))
#else
#define __pack__
#endif

#endif
