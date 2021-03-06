/*
 *
 * ALSA digital audio support
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <pthread.h>

#include "dxxerror.h"
#include "mono.h"
#include "maths.h"
#include "gr.h" // needed for piggy.h
#include "inferno.h"


//edited 05/17/99 Matt Mueller - added ifndef NO_ASM
//added on 980905 by adb to add inline fixmul for mixer on i386
#ifndef NO_ASM
#ifdef __i386__
#define do_fixmul(x,y)				\
({						\
	int _ax, _dx;				\
	asm("imull %2\n\tshrdl %3,%1,%0"	\
	    : "=a"(_ax), "=d"(_dx)		\
	    : "rm"(y), "i"(16), "0"(x));	\
	_ax;					\
})
extern inline fix fixmul(fix x, fix y) { return do_fixmul(x,y); }
#endif
#endif
//end edit by adb
//end edit -MM

//changed on 980905 by adb to increase number of concurrent sounds
#define MAX_SOUND_SLOTS 32
//end changes by adb
#define SOUND_BUFFER_SIZE 512

#define MIN_VOLUME 10

/* This table is used to add two sound values together and pin
 * the value to avoid overflow.  (used with permission from ARDI)
 * DPH: Taken from SDL/src/SDL_mixer.c.
 */
static const ubyte mix8[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
  0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
  0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
  0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A,
  0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
  0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
  0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B,
  0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
  0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
  0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C,
  0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
  0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D,
  0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
  0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
  0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE,
  0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
  0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4,
  0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
  0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5,
  0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

//added/changed on 980905 by adb to make sfx volume work, on 990221 by adb changed F1_0 to F1_0 / 2
#define SOUND_MAX_VOLUME (F1_0 / 2)

int digi_volume = SOUND_MAX_VOLUME;
//end edit by adb

static int Digi_initialized = 0;

struct sound_slot {
 int soundno;
 int playing;   // Is there a sample playing on this channel?
 int looped;    // Play this sample looped?
 fix pan;       // 0 = far left, 1 = far right
 fix volume;    // 0 = nothing, 1 = fully on
 //changed on 980905 by adb from char * to unsigned char * 
 unsigned char *samples;
 //end changes by adb
 unsigned int length; // Length of the sample
 unsigned int position; // Position we are at at the moment.
	int soundobj;   // Which soundobject is on this channel
	int persistent; // This can't be pre-empted
} SoundSlots[MAX_SOUND_SLOTS];

static int digi_max_channels = 16;

static int next_channel = 0;

/* Threading/ALSA stuff */
#define LOCK() pthread_mutex_lock(&mutex)
#define UNLOCK() pthread_mutex_unlock(&mutex)
snd_pcm_t *snd_devhandle;
pthread_t thread_id;
pthread_mutex_t mutex;


/* Audio mixing callback */
//changed on 980905 by adb to cleanup, add pan support and optimize mixer
static void audio_mixcallback(void *userdata, ubyte *stream, int len)
{
 ubyte *streamend = stream + len;
 struct sound_slot *sl;

 for (sl = SoundSlots; sl < SoundSlots + MAX_SOUND_SLOTS; sl++)
 {
  if (sl->playing)
  {
   ubyte *sldata = sl->samples + sl->position, *slend = sl->samples + sl->length;
   ubyte *sp = stream;
   signed char v;
   fix vl, vr;
   int x;

   if ((x = sl->pan) & 0x8000)
   {
    vl = 0x20000 - x * 2;
    vr = 0x10000;
   }
   else
   {
    vl = 0x10000;
    vr = x * 2;
   }
   vl = fixmul(vl, (x = sl->volume));
   vr = fixmul(vr, x);
   while (sp < streamend) 
   {
    if (sldata == slend)
    {
     if (!sl->looped)
     {
      sl->playing = 0;
      break;
     }
     sldata = sl->samples;
    }
    v = *(sldata++) - 0x80;
				*sp = mix8[*sp + fixmul(v, vl) + 0x80];
				sp++;
				*sp = mix8[*sp + fixmul(v, vr) + 0x80];
				sp++;
   }
   sl->position = sldata - sl->samples;
  }
 }
}
//end changes by adb

void *mixer_thread(void *data)
{
	int err;
	ubyte buffer[SOUND_BUFFER_SIZE];

	/* Allow ourselves to be asynchronously cancelled */
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	while (1)
	{
		memset(buffer, 0x80, SOUND_BUFFER_SIZE);
		LOCK();
		audio_mixcallback(NULL,buffer,512);
		UNLOCK();
	again:
		err = snd_pcm_writei(snd_devhandle, buffer, SOUND_BUFFER_SIZE / 2);

		if (err == -EPIPE)
		{
			// Sound buffer underrun
			err = snd_pcm_prepare(snd_devhandle);
			if (err < 0)
			{
				fprintf(stderr, "Can't recover from underrun: %s\n", snd_strerror(err));
			}
		}
		else if (err == -EAGAIN)
		{
			goto again;
		}
		else if (err != SOUND_BUFFER_SIZE / 2)
		{
			// Each frame has size 2 bytes - hence we expect SOUND_BUFFER_SIZE/2
			// frames to be written.
			fprintf(stderr, "Unknown err %d: %s\n", err, snd_strerror(err));
		}
	}
	return 0;
}


/* Initialise audio devices. */
int digi_init()
{
	int err, tmp;
	char *device = "plughw:0,0";
	snd_pcm_hw_params_t *params;
 pthread_attr_t attr;
 pthread_mutexattr_t mutexattr;

 //added on 980905 by adb to init sound kill system
 memset(SampleHandles, 255, sizeof(SampleHandles));
 //end edit by adb

 /* Open the ALSA sound device */
	if ((err = snd_pcm_open(&snd_devhandle, device, SND_PCM_STREAM_PLAYBACK)) < 0)
	{
     fprintf(stderr, "open failed: %s\n", snd_strerror( err ));  
     return -1; 
	}

	snd_pcm_hw_params_alloca(&params);
	err = snd_pcm_hw_params_any(snd_devhandle, params);
	if (err < 0)
	{
		printf("ALSA: Error %s\n", snd_strerror(err));
		return -1;
	}
	err = snd_pcm_hw_params_set_access(snd_devhandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0)
	{
		printf("ALSA: Error %s\n", snd_strerror(err));
		return -1;
	}
	err = snd_pcm_hw_params_set_format(snd_devhandle, params, SND_PCM_FORMAT_U8);
	if (err < 0)
	{
		printf("ALSA: Error %s\n", snd_strerror(err));
		return -1;
	}
	err = snd_pcm_hw_params_set_channels(snd_devhandle, params, 2);
	if (err < 0)
	{
		printf("ALSA: Error %s\n", snd_strerror(err));
		return -1;
	}
	tmp = 11025;
	err = snd_pcm_hw_params_set_rate_near(snd_devhandle, params, &tmp, NULL);
	if (err < 0)
	{
		printf("ALSA: Error %s\n", snd_strerror(err));
		return -1;
	}
	snd_pcm_hw_params_set_periods(snd_devhandle, params, 3, 0);
	snd_pcm_hw_params_set_buffer_size(snd_devhandle,params, (SOUND_BUFFER_SIZE*3)/2);

	err = snd_pcm_hw_params(snd_devhandle, params);
	if (err < 0)
	{
		printf("ALSA: Error %s\n", snd_strerror(err));
		return -1;
	}

 /* Start the mixer thread */

 /* We really should check the results of these */
 pthread_mutexattr_init(&mutexattr);
 pthread_mutex_init(&mutex,&mutexattr);
 pthread_mutexattr_destroy(&mutexattr);
 
 if (pthread_attr_init(&attr) != 0) {
  fprintf(stderr, "failed to init attr\n");
  snd_pcm_close( snd_devhandle ); 
  return -1;
 }

 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

 pthread_create(&thread_id,&attr,mixer_thread,NULL);
 pthread_attr_destroy(&attr);

 atexit(digi_close);
 Digi_initialized = 1;
 return 0;
}

/* Toggle audio */
void digi_reset() { }

/* Shut down audio */
void digi_close()
{
 if (!Digi_initialized) return;
 pthread_cancel(thread_id);
 Digi_initialized = 0;
 pthread_mutex_destroy(&mutex);
 snd_pcm_close(snd_devhandle);
}

void digi_stop_all_channels()
{
	int i;

	for (i = 0; i < MAX_SOUND_SLOTS; i++)
		digi_stop_sound(i);
}


extern void digi_end_soundobj(int channel);	
extern int SoundQ_channel;
extern void SoundQ_end();
int verify_sound_channel_free(int channel);

// Volume 0-F1_0
int digi_start_sound(short soundnum, fix volume, int pan, int looping, int loop_start, int loop_end, int soundobj)
{
	int i, starting_channel;

	if (!Digi_initialized) return -1;

	if (soundnum < 0) return -1;

	LOCK();
	Assert(GameSounds[soundnum].data != (void *)-1);

	starting_channel = next_channel;

	while(1)
	{
		if (!SoundSlots[next_channel].playing)
			break;

		if (!SoundSlots[next_channel].persistent)
			break;	// use this channel!	

		next_channel++;
		if (next_channel >= digi_max_channels)
			next_channel = 0;
		if (next_channel == starting_channel)
		{
			mprintf((1, "OUT OF SOUND CHANNELS!!!\n"));
			UNLOCK();
			return -1;
		}
	}
	if (SoundSlots[next_channel].playing)
	{
		SoundSlots[next_channel].playing = 0;
		if (SoundSlots[next_channel].soundobj > -1)
		{
			digi_end_soundobj(SoundSlots[next_channel].soundobj);
		}
		if (SoundQ_channel == next_channel)
			SoundQ_end();
	}

#ifndef NDEBUG
	verify_sound_channel_free(next_channel);
#endif

	SoundSlots[next_channel].soundno = soundnum;
	SoundSlots[next_channel].samples = GameSounds[soundnum].data;
	SoundSlots[next_channel].length = GameSounds[soundnum].length;
	SoundSlots[next_channel].volume = fixmul(digi_volume, volume);
	SoundSlots[next_channel].pan = pan;
	SoundSlots[next_channel].position = 0;
	SoundSlots[next_channel].looped = looping;
	SoundSlots[next_channel].playing = 1;
	SoundSlots[next_channel].soundobj = soundobj;
	SoundSlots[next_channel].persistent = 0;
	if ((soundobj > -1) || (looping) || (volume > F1_0))
		SoundSlots[next_channel].persistent = 1;

	i = next_channel;
	next_channel++;
	if (next_channel >= digi_max_channels)
		next_channel = 0;
	UNLOCK();

	return i;
}

// Returns the channel a sound number is playing on, or
// -1 if none.
int digi_find_channel(int soundno)
{
	if (!Digi_initialized)
		return -1;

	if (soundno < 0 )
		return -1;

	if (GameSounds[soundno].data == NULL)
	{
		Int3();
		return -1;
	}

	//FIXME: not implemented
	return -1;
}


//added on 980905 by adb from original source to make sfx volume work
void digi_set_digi_volume( int dvolume )
{
	dvolume = fixmuldiv( dvolume, SOUND_MAX_VOLUME, 0x7fff);
	if ( dvolume > SOUND_MAX_VOLUME )
		digi_volume = SOUND_MAX_VOLUME;
	else if ( dvolume < 0 )
		digi_volume = 0;
	else
		digi_volume = dvolume;

	if ( !Digi_initialized ) return;

	digi_sync_sounds();
}
//end edit by adb

void digi_set_volume(int dvolume, int mvolume)
{
	digi_set_digi_volume(dvolume);
	digi_set_midi_volume(mvolume);
//	mprintf(( 1, "Volume: 0x%x and 0x%x\n", digi_volume, midi_volume ));
}

int digi_is_sound_playing(int soundno)
{
	int i;

	soundno = digi_xlat_sound(soundno);

	LOCK();
	for (i = 0; i < MAX_SOUND_SLOTS; i++)
		  //changed on 980905 by adb: added SoundSlots[i].playing &&
		  if (SoundSlots[i].playing && SoundSlots[i].soundno == soundno)
		  //end changes by adb
		  { UNLOCK();	return 1; }
	UNLOCK();
	return 0;
}


 //added on 980905 by adb to make sound channel setting work
void digi_set_max_channels(int n) { 
	digi_max_channels	= n;

	if ( digi_max_channels < 1 ) 
		digi_max_channels = 1;
	if (digi_max_channels > MAX_SOUND_SLOTS)
		digi_max_channels = MAX_SOUND_SLOTS;

	if ( !Digi_initialized ) return;

	digi_stop_all_channels();
}

int digi_get_max_channels() { 
	return digi_max_channels; 
}
// end edit by adb

int digi_is_channel_playing(int channel)
{
	if (!Digi_initialized)
		return 0;

	LOCK();
	if (SoundSlots[channel].playing)
	{
		UNLOCK();
		return 1;
	}
	UNLOCK();
	return 0;
}

void digi_set_channel_volume(int channel, int volume)
{
	if (!Digi_initialized)
		return;

	LOCK();
	if (SoundSlots[channel].playing)
		SoundSlots[channel].volume = fixmuldiv(volume, digi_volume, F1_0);
	UNLOCK();
}

void digi_set_channel_pan(int channel, int pan)
{
	if (!Digi_initialized)
		return;

	LOCK();
	if (SoundSlots[channel].playing)
		SoundSlots[channel].pan = pan;
	UNLOCK();
}

void digi_stop_sound(int channel)
{
	LOCK();
	SoundSlots[channel].playing = 0;
	SoundSlots[channel].soundobj = -1;
	SoundSlots[channel].persistent = 0;
	UNLOCK();
}

void digi_end_sound(int channel)
{
	if (!Digi_initialized)
		return;

	LOCK();
	if (SoundSlots[channel].playing)
	{
		SoundSlots[channel].soundobj = -1;
		SoundSlots[channel].persistent = 0;
	}
	UNLOCK();
}


// MIDI stuff follows.
//added/killed on 11/25/98 by Matthew Mueller
//void digi_set_midi_volume( int mvolume ) { }
//void digi_play_midi_song( char * filename, char * melodic_bank, char * drum_bank, int loop ) {}
//void digi_stop_current_song()
//{
//#ifdef HMIPLAY
//        char buf[10];
//    
//        sprintf(buf,"s");
//        send_ipc(buf);
//#endif
//}
//end this section kill - MM
