/*
 *
 * DPH: This is the file where all the stub functions go.
 * The aim is to have nothing in here, eventually
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "pstypes.h"
#include "error.h"
#include "args.h"
#include "rbaudio.h"

extern int Redbook_playing;
static int initialised = 0;

void RBAExit()
{
    if (initialised) {
	/* FIXME: Close cdrom device */
    }
}

void RBAInit()
{
    if (initialised) return;
    if (FindArg("-nocdrom")) return; 

    /* FIXME: Initialize cdrom device */

}

int RBAEnabled()
{
 return 1;
}

void RBARegisterCD()
{

}

int RBAPlayTrack(int a)
{
return 0; 
}

void RBAStop()
{
}

void RBASetVolume(int a)
{

}

void RBAPause()
{
}

void RBAResume()
{
}

int RBAGetNumberOfTracks()
{
return 0;
}

int RBAPlayTracks(int tracknum,int something)
{
return -1;
}

int RBAGetTrackNum()
{
return -1;
}

int RBAPeekPlayStatus()
{
 return -1;
}

int CD_blast_mixer()
{
 return 0;
}
