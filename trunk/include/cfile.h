/* $Id: cfile.h,v 1.11 2004-08-28 23:17:45 schaffner Exp $ */
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

/*
 *
 * Prototypes for compressed file functions...
 *
 */

#ifndef _CFILE_H
#define _CFILE_H

#include <stdio.h>

#include "maths.h"
#include "vecmat.h"

typedef struct CFILE CFILE;

//Specify the name of the hogfile.  Returns 1 if hogfile found & had files
int cfile_init(char *hogname);

int cfile_size(char *hogname);

CFILE * cfopen(char * filename, char * mode);
int cfilelength( CFILE *fp );							// Returns actual size of file...
size_t cfread( void * buf, size_t elsize, size_t nelem, CFILE * fp );
int cfclose(CFILE *cfile);
int cfgetc( CFILE * fp );
int cfseek( CFILE *fp, long int offset, int where );
int cftell( CFILE * fp );
char * cfgets( char * buf, size_t n, CFILE * fp );

int cfeof(CFILE *cfile);
int cferror(CFILE *cfile);

int cfexist( char * filename );	// Returns true if file exists on disk (1) or in hog (2).

// Deletes a file.
int cfile_delete(char *filename);

// Rename a file.
int cfile_rename(char *oldname, char *newname);

// Make a directory
int cfile_mkdir(char *pathname);

// cfwrite() writes to the file
int cfwrite(void *buf, int elsize, int nelem, CFILE *cfile);

// cfputc() writes a character to a file
int cfputc(int c, CFILE *cfile);

// cfputs() writes a string to a file
int cfputs(char *str, CFILE *cfile);

// Allows files to be gotten from an alternate hog file.
// Passing NULL disables this.
// Returns 1 if hogfile found (& contains file), else 0.  
// If NULL passed, returns 1
int cfile_use_alternate_hogfile( char * name );

// Allows files to be gotten from the Descent 1 hog file.
// Passing NULL disables this.
// Returns 1 if hogfile found (& contains file), else 0.
// If NULL passed, returns 1
int cfile_use_descent1_hogfile(char * name);

// All cfile functions will check this directory if no file exists
// in the current directory.
void cfile_use_alternate_hogdir( char * path );

//tell cfile about your critical error counter 
void cfile_set_critical_error_counter_ptr(int *ptr);

// prototypes for reading basic types from cfile
int cfile_read_int(CFILE *file);
short cfile_read_short(CFILE *file);
sbyte cfile_read_byte(CFILE *file);
fix cfile_read_fix(CFILE *file);
fixang cfile_read_fixang(CFILE *file);
void cfile_read_vector(vms_vector *v, CFILE *file);
void cfile_read_angvec(vms_angvec *v, CFILE *file);
void cfile_read_matrix(vms_matrix *v, CFILE *file);

// Reads variable length, null-termined string.   Will only read up
// to n characters.
void cfile_read_string(char *buf, int n, CFILE *file);

// functions for writing cfiles
int cfile_write_int(int i, CFILE *file);
int cfile_write_short(short s, CFILE *file);
int cfile_write_byte(sbyte u, CFILE *file);

// writes variable length, null-termined string.
int cfile_write_string(char *buf, CFILE *file);

#endif
