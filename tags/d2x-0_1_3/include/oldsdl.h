/*
 * $Source: /cvs/cvsroot/d2x/include/oldsdl.h,v $
 * $Revision: 1.2 $
 * $Author: bradleyb $
 * $Date: 2002-02-16 02:24:53 $
 *
 * Version-checking macros for SDL
 *
 */

#ifndef _OLDSDL_H
#define _OLDSDL_H

/* This macro turns the version numbers into a numeric value:
   (1,2,3) -> (1203)
   This assumes that there will never be more than 100 patchlevels
*/
#ifndef SDL_VERSIONNUM
#define SDL_VERSIONNUM(X, Y, Z)                                         \
        (X)*1000 + (Y)*100 + (Z)
#endif

/* This is the version number macro for the current SDL version */
#ifndef SDL_COMPILEDVERSION
#define SDL_COMPILEDVERSION \
        SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL)
#endif

/* This macro will evaluate to true if compiled with SDL at least X.Y.Z */
#define SDL_VERSION_ATLEAST(X, Y, Z) \
        (SDL_COMPILEDVERSION >= SDL_VERSIONNUM(X, Y, Z))

#endif