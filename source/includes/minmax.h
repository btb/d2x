
#ifndef _MINMAX_H
#define _MINMAX_H

#include <stdlib.h>

#if !defined(max)
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#endif /* _MINMAX_H */
