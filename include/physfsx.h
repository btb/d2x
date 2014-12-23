/* $Id: physfsx.h,v 1.12 2006-07-29 04:02:40 chris Exp $ */

/*
 *
 * Some simple physfs extensions
 *
 */

#ifndef PHYSFSX_H
#define PHYSFSX_H

#if !defined(macintosh) && !defined(_MSC_VER)
#include <sys/param.h>
#endif
#if defined(__linux__)
#include <sys/vfs.h>
#elif defined(__MACH__) && defined(__APPLE__)
#include <sys/mount.h>
#endif
#include <string.h>
#include <stdarg.h>

#include <physfs.h>

#include "pstypes.h"
#include "error.h"
#include "vecmat.h"
#include "args.h"

// Initialise PhysicsFS, set up basic search paths and add arguments from .ini file(s).
// The arguments are used to determine the search paths, so the first .ini file must be
// in the same directory as D2X. A second one can be in the user directory.
static inline void PHYSFSX_init(int argc, char *argv[])
{
	int t;

	PHYSFS_init(argv[0]);
	PHYSFS_permitSymbolicLinks(1);

	PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
	InitArgs( argc,argv );

	if ((t = FindArg("-userdir"))
#ifdef __unix__
		|| 1	// or if it's a unix platform
#endif
		)
	{
		// This stuff below seems overly complicated - brad

		char *path = Args[t+1];
		char fullPath[PATH_MAX + 5];

#ifdef __unix__
		if (!t)
			path = "~/.d2x";
#endif
		PHYSFS_removeFromSearchPath(PHYSFS_getBaseDir());

		if (path[0] == '~') // yes, this tilde can be put before non-unix paths.
		{
			const char *home = PHYSFS_getUserDir();

			strcpy(fullPath, home);	// prepend home to the path
			path++;
			if (*path == *PHYSFS_getDirSeparator())
				path++;
			strncat(fullPath, path, PATH_MAX + 5 - strlen(home));
		}
		else
			strncpy(fullPath, path, PATH_MAX + 5);

		PHYSFS_setWriteDir(fullPath);
		if (!PHYSFS_getWriteDir())
		{						// need to make it
			char *p;
			char ancestor[PATH_MAX + 5];	// the directory which actually exists
			char child[PATH_MAX + 5];		// the directory relative to the above we're trying to make

			strcpy(ancestor, fullPath);
			while (!PHYSFS_getWriteDir() && ((p = strrchr(ancestor, *PHYSFS_getDirSeparator()))))
			{
				if (p[1] == 0)
				{					// separator at the end (intended here, for safety)
					*p = 0;			// kill this separator
					if (!((p = strrchr(ancestor, *PHYSFS_getDirSeparator()))))
						break;		// give up, this is (usually) the root directory
				}

				p[1] = 0;			// go to parent
				PHYSFS_setWriteDir(ancestor);
			}

			strcpy(child, fullPath + strlen(ancestor));
			for (p = child; (p = strchr(p, *PHYSFS_getDirSeparator())); p++)
				*p = '/';
			PHYSFS_mkdir(child);
			PHYSFS_setWriteDir(fullPath);
		}

		PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 1);
		AppendArgs();
	}

	if (!PHYSFS_getWriteDir())
	{
		PHYSFS_setWriteDir(PHYSFS_getBaseDir());
		if (!PHYSFS_getWriteDir())
			Error("can't set write dir\n");
		else
			PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 0);
	}

	//tell cfile where hogdir is
	if ((t=FindArg("-hogdir")))
		PHYSFS_addToSearchPath(Args[t + 1], 1);
#ifdef __unix__
	else if (!FindArg("-nohogdir"))
		PHYSFS_addToSearchPath(SHAREPATH, 1);
#endif
#ifdef MACINTOSH
	PHYSFS_addToSearchPath(":Data:", 1); // untested
#endif
}

static inline PHYSFS_sint64 PHYSFSX_readString(PHYSFS_file *file, char *s)
{
	char *ptr = s;

	if (PHYSFS_eof(file))
		*ptr = 0;
	else
		do
			PHYSFS_read(file, ptr, 1, 1);
		while (!PHYSFS_eof(file) && *ptr++ != 0);

	return strlen(s);
}

static inline PHYSFS_sint64 PHYSFSX_gets(PHYSFS_file *file, char *s)
{
	char *ptr = s;

	if (PHYSFS_eof(file))
		*ptr = 0;
	else
		do
			PHYSFS_read(file, ptr, 1, 1);
		while (!PHYSFS_eof(file) && *ptr++ != '\n');
	*ptr = 0;

	return strlen(s);
}

static inline PHYSFS_sint64 PHYSFSX_writeU8(PHYSFS_file *file, PHYSFS_uint8 val)
{
	return PHYSFS_write(file, &val, 1, 1);
}

static inline PHYSFS_sint64 PHYSFSX_writeString(PHYSFS_file *file, char *s)
{
	return PHYSFS_write(file, s, 1, (PHYSFS_uint32)strlen(s) + 1);
}

static inline PHYSFS_sint64 PHYSFSX_puts(PHYSFS_file *file, char *s)
{
	return PHYSFS_write(file, s, 1, (PHYSFS_uint32)strlen(s));
}

static inline int PHYSFSX_putc(PHYSFS_file *file, int c)
{
	unsigned char ch = (unsigned char)c;

	if (PHYSFS_write(file, &ch, 1, 1) < 1)
		return -1;
	else
		return (int)c;
}

static inline PHYSFS_sint64 PHYSFSX_printf(PHYSFS_file *file, char *format, ...)
{
	char buffer[1024];
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);

	return PHYSFSX_puts(file, buffer);
}

#define PHYSFSX_writeFix	PHYSFS_writeSLE32
#define PHYSFSX_writeFixAng	PHYSFS_writeSLE16

static inline int PHYSFSX_writeVector(PHYSFS_file *file, vms_vector *v)
{
	if (PHYSFSX_writeFix(file, v->x) < 1 ||
	 PHYSFSX_writeFix(file, v->y) < 1 ||
	 PHYSFSX_writeFix(file, v->z) < 1)
		return 0;

	return 1;
}

static inline int PHYSFSX_writeAngleVec(PHYSFS_file *file, vms_angvec *v)
{
	if (PHYSFSX_writeFixAng(file, v->p) < 1 ||
	 PHYSFSX_writeFixAng(file, v->b) < 1 ||
	 PHYSFSX_writeFixAng(file, v->h) < 1)
		return 0;

	return 1;
}

static inline int PHYSFSX_writeMatrix(PHYSFS_file *file, vms_matrix *m)
{
	if (PHYSFSX_writeVector(file, &m->rvec) < 1 ||
	 PHYSFSX_writeVector(file, &m->uvec) < 1 ||
	 PHYSFSX_writeVector(file, &m->fvec) < 1)
		return 0;

	return 1;
}

static inline int PHYSFSX_getRealPath(const char *stdPath, char *realPath)
{
	const char *realDir = PHYSFS_getRealDir(stdPath);
	const char *sep = PHYSFS_getDirSeparator();
	char *p;

	if (!realDir)
	{
		realDir = PHYSFS_getWriteDir();
		if (!realDir)
			return 0;
	}

	strncpy(realPath, realDir, PATH_MAX - 1);
	if (strlen(realPath) >= strlen(sep))
	{
		p = realPath + strlen(realPath) - strlen(sep);
		if (strcmp(p, sep)) // no sep at end of realPath
			strncat(realPath, sep, PATH_MAX - 1 - strlen(realPath));
	}

	if (strlen(stdPath) >= 1)
		if (*stdPath == '/')
			stdPath++;

	while (*stdPath)
	{
		if (*stdPath == '/')
			strncat(realPath, sep, PATH_MAX - 1 - strlen(realPath));
		else
		{
			if (strlen(realPath) < PATH_MAX - 2)
			{
				p = realPath + strlen(realPath);
				p[0] = *stdPath;
				p[1] = '\0';
			}
		}
		stdPath++;
	}

	return 1;
}

static inline int PHYSFSX_rename(char *oldpath, char *newpath)
{
	char old[PATH_MAX], new[PATH_MAX];

	PHYSFSX_getRealPath(oldpath, old);
	PHYSFSX_getRealPath(newpath, new);
	return (rename(old, new) == 0);
}


// returns -1 if error
// Gets bytes free in current write dir
static inline PHYSFS_sint64 PHYSFSX_getFreeDiskSpace()
{
#if defined(__linux__) || (defined(__MACH__) && defined(__APPLE__))
	struct statfs sfs;

	if (!statfs(PHYSFS_getWriteDir(), &sfs))
		return (PHYSFS_sint64)(sfs.f_bavail * sfs.f_bsize);

	return -1;
#else
	return 0x7FFFFFFF;
#endif
}

//Open a file for reading, set up a buffer
static inline PHYSFS_file *PHYSFSX_openReadBuffered(char *filename)
{
	PHYSFS_file *fp;
	PHYSFS_uint64 bufSize;

	if (filename[0] == '\x01')
	{
		//FIXME: don't look in dir, only in hogfile
		filename++;
	}

	fp = PHYSFS_openRead(filename);
	if (!fp)
		return NULL;

	bufSize = PHYSFS_fileLength(fp);
	while (!PHYSFS_setBuffer(fp, bufSize) && bufSize)
		bufSize /= 2;	// even if the error isn't memory full, for a 20MB file it'll only do this 8 times

	return fp;
}

//Open a file for writing, set up a buffer
static inline PHYSFS_file *PHYSFSX_openWriteBuffered(char *filename)
{
	PHYSFS_file *fp;
	PHYSFS_uint64 bufSize = 1024*1024;	// hmm, seems like an OK size.

	fp = PHYSFS_openWrite(filename);
	if (!fp)
		return NULL;

	while (!PHYSFS_setBuffer(fp, bufSize) && bufSize)
		bufSize /= 2;

	return fp;
}

#endif /* PHYSFSX_H */
