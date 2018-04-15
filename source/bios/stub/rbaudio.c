/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/


#include "pstypes.h"
#include "rbaudio.h"


// --------------------------------------------------------------------

void RBAInit(ubyte cd_drive_num) //drive a == 0, drive b == 1
{
}

//find out how many tracks on the CD, and their starting locations
void RBARegisterCD(void)
{
}

int RBAGetNumberOfTracks(void)
{
   return 0;
}

int RBAPlayTrack(int track)
{
   return 0;
}

//plays tracks first through last, inclusive
int RBAPlayTracks(int first, int last)
{
   return 0;
}

void RBAPause()
{
}

int RBAResume()
{
   return 0;
}

void RBAStop()
{
}

void RBASetStereoAudio(RBACHANNELCTL *channels)
{
}

void RBASetQuadAudio(RBACHANNELCTL *channels)
{
}

void RBAGetAudioInfo(RBACHANNELCTL *channels)
{
}

void RBASetChannelVolume(int channel, int volume)
{
}

void RBASetVolume(int volume)
{
}

long RBAGetHeadLoc(int *min, int *sec, int *frame)
{
   return 0;
}

//return the track number currently playing.  Useful if RBAPlayTracks()
//is called.  Returns 0 if no track playing, else track number
int RBAGetTrackNum()
{
   return 0;
}

int RBAPeekPlayStatus()
{
   return 0;
}

int RBAEnabled()
{
   return 0;
}

void RBAEnable()
{
}

void RBADisable()
{
}
