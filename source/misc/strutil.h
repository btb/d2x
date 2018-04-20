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

#ifndef _STRUTILS_
#define _STRUTILS_

#if defined(__DOS__) || defined(WINDOWS)

#define stricmp     _stricmp
#define strnicmp    _strnicmp
#define strdup      _strdup
#define strlwr      _strlwr

#else

#ifdef MACINTOSH
extern char *strdup(char *s);
#endif
extern int stricmp(char *str1, char *str2);
extern int strnicmp(char *str1, char *str2, int n);

void strupr( char *s1 );
void strlwr( char *s1 );
void strrev( char *s1 );

#define _MAX_PATH   260 /* maximum length of full pathname */
#define _MAX_DRIVE   3  /* maximum length of drive component */
#define _MAX_DIR    256 /* maximum length of path component */
#define _MAX_FNAME  256 /* maximum length of file name component */
#define _MAX_EXT    256 /* maximum length of extension component */

void _splitpath(char *name, char *drive, char *path, char *base, char *ext);

#endif

#endif
