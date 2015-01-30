/* Maths.h library header file */

#ifndef _MATHS_H
#define _MATHS_H

#include "fix.h"
#include "vecmat.h"


#define D_RAND_MAX 32767

void d_srand (unsigned int seed);
int d_rand ();			// Random number function which returns in the range 0-0x7FFF


#endif
