/* $Id: physfsx.h,v 1.1.2.6 2003-06-03 11:00:40 btb Exp $ */

/*
 *
 * Some simple physfs extensions
 *
 */

#ifndef PHYSFSX_H
#define PHYSFSX_H

#include <sys/param.h>
#if defined(__linux__)
#include <sys/vfs.h>
#else if defined(__MACH__) && defined(__APPLE__)
#include <sys/param.h>
#include <sys/mount.h>
#endif

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

static inline int PHYSFSX_putc(PHYSFS_file *file, int c)
{
	unsigned char ch = (unsigned char)c;

	if (PHYSFS_write(file, &ch, 1, 1) < 1)
		return -1;
	else
		return (int)c;
}

static inline int PHYSFSX_rename(char *oldpath, char *newpath)
{
	char old[PATH_MAX], new[PATH_MAX];

	snprintf(old, PATH_MAX, "%s/%s", PHYSFS_getWriteDir(), oldpath);
	snprintf(new, PATH_MAX, "%s/%s", PHYSFS_getWriteDir(), newpath);
	return (rename(old, new) == 0);
}


// returns -1 if error
// Gets bytes free in current write dir
static inline PHYSFS_sint64 PHYSFSX_getFreeDiskSpace()
{
	struct statfs sfs;

	if (!statfs(PHYSFS_getWriteDir(), &sfs))
		return (PHYSFS_sint64)(sfs.f_bavail * sfs.f_bsize);

	return -1;
}

#endif /* PHYSFSX_H */
