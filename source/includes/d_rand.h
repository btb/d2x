
#ifndef _D_RAND_H
#define _D_RAND_H

#define D_RAND_MAX 32767U
#define d_rand() (rand() & D_RAND_MAX)

#endif
