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



#ifndef _FINDFILE_H
#define _FINDFILE_H

typedef struct FILEFINDSTRUCT {
   unsigned long size;
   char name[256];
} FILEFINDSTRUCT;


int FileFindFirst(char *search_str, FILEFINDSTRUCT *ffstruct);
int FileFindNext(FILEFINDSTRUCT *ffstruct);
int FileFindClose(void);

typedef struct FILETIMESTRUCT {
   unsigned short date,time;
} FILETIMESTRUCT;

//the both return 0 if no error
int GetFileDateTime(int filehandle, FILETIMESTRUCT *ftstruct);
int SetFileDateTime(int filehandle, FILETIMESTRUCT *ftstruct);

#ifdef WINDOWS
unsigned int GetFreeDiskSpace(void);
#else
int GetDiskFree(void);
#endif

#endif
