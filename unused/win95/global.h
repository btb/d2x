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

#ifndef _GLOBAL_
#define _GLOBAL_

#ifndef _WIN32
#define  BCODE                  __based(__segname("_CODE"))
#define  BSTACK                 __based(__segname("_STACK"))
#define  BSEG(x)                __based(__segname(x))
#define  HUGE                   __huge
#else
#define  BCODE
#define  BSTACK
#define  BSEG(x)
#define  HUGE
#endif

/* Allow visibility of static functions for debug
*/ 
#define  PUBLIC
#define  PRIVATE

#define  FNLOCAL          
#define  FNGLOBAL     
#define  FNEXPORT     

#endif
