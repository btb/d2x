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



#ifndef _BYTESWAP_H
#define _BYTESWAP_H

#include "pstypes.h"

extern ushort swapshort(ushort s);
extern uint swapint(uint i);

#define SWAPSHORT(x) (((ubyte)x << 8) | (((ushort)x) >> 8))
#define SWAPINT(x)   ((x<<24) | (((ulong)x) >> 24) | ((x &0x0000ff00) << 8) | ((x & 0x00ff00000) >> 8))

#ifndef MACINTOSH
#define INTEL_INT(x)	x
#define INTEL_SHORT(x)	x
#else
#define INTEL_INT(x)	SWAPINT(x)
#define INTEL_SHORT(x)	SWAPSHORT(x)
#endif

#endif
