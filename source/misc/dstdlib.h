
#ifndef _DSTDLIB_H
#define _DSTDLIB_H

#include <stdlib.h>

/* min and max macros */
#if !defined(max)
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#endif /* _DSTDLIB_H */
