/* fileio.c in /misc for d1x file reading */
#ifndef _STRIO_H
#define _STRIO_H

#include <physfs.h>

char *fsplitword(PHYSFS_file *f, char splitchar);
char *splitword(char *s, char splitchar);

#endif