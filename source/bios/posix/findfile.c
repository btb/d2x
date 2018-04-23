/*
 *
 * findfile functions
 *
 */

#include <glob.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include "findfile.h"
#include "error.h"

static glob_t glob_a;
static int glob_whichfile;

int FileFindFirst(char *search_str, FILEFINDSTRUCT *ffstruct)
{
   int r;
   char *t;

   Assert(search_str != NULL);
   Assert(ffstruct != NULL);

   r = glob(search_str, 0, NULL, &glob_a);
   if (r)
      return 1;

   if (! glob_a.gl_pathc)
      return 1;

   glob_whichfile = 0;

   t = strrchr(glob_a.gl_pathv[glob_whichfile], '/');
   if (t == NULL)
      t = glob_a.gl_pathv[glob_whichfile];
   else
      t++;

   strncpy(ffstruct->name, t, 255);
   ffstruct->size = strlen(ffstruct->name);

   return 0;
}

int FileFindNext(FILEFINDSTRUCT *ffstruct)
{
   char *t;

   glob_whichfile++;

   if (glob_whichfile >= glob_a.gl_pathc) return -1;

   t = strrchr(glob_a.gl_pathv[glob_whichfile], '/');
   if (t == NULL)
      t = glob_a.gl_pathv[glob_whichfile];
   else
      t++;

   strncpy(ffstruct->name, t, 255);
   ffstruct->size = strlen(ffstruct->name);

   return 0;
}

int FileFindClose(void)
{
   globfree(&glob_a);

   return 0;
}

int GetDiskFree(void)
{
   struct statvfs statbuf;
   int status;
   char path[PATH_MAX];

   getcwd(path, PATH_MAX);
   status = statvfs(path, &statbuf);

   return statbuf.f_bsize * statbuf.f_bavail;
}
