/* $Id: physfsx.h,v 1.1.2.3 2003-05-22 07:38:50 btb Exp $ */

/*
 *
 * Some simple physfs extensions
 *
 */

#ifndef PHYSFSX_H
#define PHYSFSX_H

#include <sys/param.h>
#include <physfs.h>

#define PHYSFSX_readU8(file, val) PHYSFS_read(file, val, 1, 1)

static inline int PHYSFSX_readString(PHYSFS_file *file, char *s)
{
	char *ptr = s;

	if (PHYSFS_eof(file))
		*ptr = 0;
	else
		do
			PHYSFSX_readU8(file, ptr);
		while (!PHYSFS_eof(file) && *ptr++!=0);

	return strlen(s);
}

static inline int PHYSFSX_gets(PHYSFS_file *file, char *s)
{
	char *ptr = s;

	if (PHYSFS_eof(file))
		*ptr = 0;
	else
		do
			PHYSFSX_readU8(file, ptr);
		while (!PHYSFS_eof(file) && *ptr++!='\n');

	return strlen(s);
}

static inline int PHYSFSX_writeU8(PHYSFS_file *file, PHYSFS_uint8 val)
{
	return PHYSFS_write(file, &val, 1, 1);
}

static inline int PHYSFSX_writeString(PHYSFS_file *file, char *s)
{
	return PHYSFS_write(file, s, 1, strlen(s) + 1);
}

static inline int PHYSFSX_puts(PHYSFS_file *file, char *s)
{
	return PHYSFS_write(file, s, 1, strlen(s));
}

static inline int PHYSFSX_rename(char *oldpath, char *newpath)
{
	char old[PATH_MAX], new[PATH_MAX];

	snprintf(old, PATH_MAX, "%s/%s", PHYSFS_getWriteDir(), oldpath);
	snprintf(new, PATH_MAX, "%s/%s", PHYSFS_getWriteDir(), newpath);
	return (rename(old, new) == 0);
}

#endif /* PHYSFSX_H */
