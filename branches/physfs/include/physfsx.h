/* $Id: physfsx.h,v 1.1.2.1 2003-05-17 04:42:17 btb Exp $ */

/*
 *
 * Some simple physfs extensions
 *
 */

#ifndef PHYSFSX_H
#define PHYSFSX_H

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

static inline int PHYSFSX_writeU8(PHYSFS_file *file, PHYSFS_uint8 val)
{
	return PHYSFS_write(file, &val, 1, 1);
}

static inline int PHYSFSX_writeString(PHYSFS_file *file, char *s)
{
	return PHYSFS_write(file, s, 1, strlen(s) + 1);
}

#endif /* PHYSFSX_H */
