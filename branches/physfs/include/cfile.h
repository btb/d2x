/* $Id: cfile.h,v 1.8.2.1 2003-05-17 04:35:51 btb Exp $ */

/*
 *
 * Wrappers for physfs abstraction layer
 *
 */

#ifndef _CFILE_H
#define _CFILE_H

#include <stdio.h>

#include <physfs.h>

#define CFILE            PHYSFS_file
#define cfopen(n,m)      PHYSFS_openRead(n)
#define cfread(p,s,n,fp) PHYSFS_read(fp,p,s,n)
#define cfclose          PHYSFS_close
#define cftell           PHYSFS_tell
#define cfexist          PHYSFS_exists
#define cfile_use_alternate_hogfile(n) PHYSFS_addToSearchPath(n,1)
#define cfile_use_descent1_hogfile(n)  PHYSFS_addToSearchPath(n,1)
#define cfilelength      PHYSFS_fileLength

//Specify the name of the hogfile.  Returns 1 if hogfile found & had files
static inline int cfile_init(char *hogname)
{
	const char *path = PHYSFS_getRealDir(hogname);
	char pathname[1024];

	if (!path)
		return 0;

	snprintf(pathname, 1024, "%s/%s", path, hogname);

	return PHYSFS_addToSearchPath(pathname, 1);
}

static inline int cfile_size(char *hogname)
{
	PHYSFS_file *fp;
	int size;

	fp = PHYSFS_openRead(hogname);
	if (fp == NULL)
		return -1;
	size = PHYSFS_fileLength(fp);
	cfclose(fp);

	return size;
}

static inline int cfgetc(PHYSFS_file *const fp)
{
	unsigned char c;

	if (PHYSFS_read(fp, &c, 1, 1) != 1)
		return EOF;

	return c;
}

static inline char * cfgets(char * const str, const int size, PHYSFS_file *const fp)
{
	int i = 0;
	int c;

	do
	{
		if (i == size-1)
			break;
		c = cfgetc(fp);
		if (c == EOF)
			break;
		str[i++] = c;
	} while (c != '\0' &&
			 c != -1 &&
			 c != '\n');
	str[i] = '\0';
	if (i == 0)
		return NULL;

	return str;
}

static inline int cfseek(PHYSFS_file *fp, long int offset, int where)
{
	int c, goal_position;

	switch(where)
	{
	case SEEK_SET:
		goal_position = offset;
		break;
	case SEEK_CUR:
		goal_position = PHYSFS_tell(fp) + offset;
		break;
	case SEEK_END:
		goal_position = PHYSFS_fileLength(fp) + offset;
		break;
	default:
		return 1;
	}
	c = PHYSFS_seek(fp, goal_position);
	return c;
}


/*
 * read some data types...
 */

#include "pstypes.h"
#include "maths.h"
#include "vecmat.h"

static inline int cfile_read_int(PHYSFS_file *file)
{
	int32_t i;

	if (!PHYSFS_readULE32(file, &i))
	{
		fprintf(stderr, "Error reading int in cfile_read_int()");
		exit(1);
	}

	return i;
}

static inline short cfile_read_short(PHYSFS_file *file)
{
	int16_t s;

	if (!PHYSFS_readULE16(file, &s))
	{
		fprintf(stderr, "Error reading short in cfile_read_short()");
		exit(1);
	}

	return s;
}

static inline byte cfile_read_byte(PHYSFS_file *file)
{
	byte b;

	if (PHYSFS_read(file, &b, sizeof(b), 1) != 1)
	{
		fprintf(stderr, "Error reading byte in cfile_read_byte()");
		exit(1);
	}

	return b;
}

static inline fix cfile_read_fix(PHYSFS_file *file)
{
	fix f;

	if (!PHYSFS_readULE32(file, &f))
	{
		fprintf(stderr, "Error reading fix in cfile_read_fix()");
		exit(1);
	}

	return f;
}

static inline fixang cfile_read_fixang(PHYSFS_file *file)
{
	fixang f;

	if (!PHYSFS_readULE16(file, &f))
	{
		fprintf(stderr, "Error reading fixang in cfile_read_fixang()");
		exit(1);
	}

	return f;
}

static inline void cfile_read_vector(vms_vector *v, PHYSFS_file *file)
{
	v->x = cfile_read_fix(file);
	v->y = cfile_read_fix(file);
	v->z = cfile_read_fix(file);
}

static inline void cfile_read_angvec(vms_angvec *v, PHYSFS_file *file)
{
	v->p = cfile_read_fixang(file);
	v->b = cfile_read_fixang(file);
	v->h = cfile_read_fixang(file);
}

static inline void cfile_read_matrix(vms_matrix *m,PHYSFS_file *file)
{
	cfile_read_vector(&m->rvec,file);
	cfile_read_vector(&m->uvec,file);
	cfile_read_vector(&m->fvec,file);
}

#endif
