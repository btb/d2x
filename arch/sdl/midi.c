/*
 *
 * SDL midi support
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include "SDL.h"
#include "SDL_mixer.h"

#include "mono.h"
#include "inferno.h"
#include "u_mem.h"
#include "physfsrwops.h"
#include "hmp.h"


extern int Digi_initialized;

static int midi_volume = 128 / 2; // Max volume
char digi_last_midi_song[16] = "";
char digi_last_melodic_bank[16] = "";
char digi_last_drum_bank[16] = "";

// handle for the initialized MIDI song
Mix_Music *SongHandle = NULL;
ubyte *SongData = NULL;
unsigned int SongSize;


void digi_set_midi_volume( int mvolume )
{
	int old_volume = midi_volume;

	if ( mvolume > 127 )
		midi_volume = 127;
	else if ( mvolume < 0 )
		midi_volume = 0;
	else
		midi_volume = mvolume;

	if (!Redbook_playing && (old_volume < 1) && ( midi_volume > 1 ) ) {
		if (!SongHandle)
			digi_play_midi_song( digi_last_midi_song, digi_last_melodic_bank, digi_last_drum_bank, 1 );
	}
	Mix_VolumeMusic(midi_volume);
}


void digi_play_midi_song( char *filename, char *melodic_bank, char *drum_bank, int loop )
{
	SDL_RWops *rw = NULL;

	if (!Digi_initialized) return;

	digi_stop_current_song();

	if ( filename == NULL )	return;

	strcpy( digi_last_midi_song, filename );
	strcpy( digi_last_melodic_bank, melodic_bank );
	strcpy( digi_last_drum_bank, drum_bank );

	if ( midi_volume < 1 )
		return; // Don't play song if volume == 0;

	// initialize the song
	if (cfexist(filename)) {
		mprintf((0, "Loading %s\n", filename));
		hmp2mid(filename, &SongData, &SongSize);
		rw = SDL_RWFromConstMem(SongData, SongSize);
		SongHandle = Mix_LoadMUS_RW(rw);
	}

	if (!SongHandle) {
		char fname[128];
		CFILE *fp;
		int sl;

		// load .mid version, if available
		sl = (int)strlen( filename );
		strcpy( fname, filename );
		fname[sl-3] = 'm';
		fname[sl-2] = 'i';
		fname[sl-1] = 'd';

		if (cfexist(fname)) {
			mprintf((0, "Loading %s\n", fname));
			fp = cfopen( fname, "rb" );

			SongSize = cfilelength( fp );
			SongData = d_calloc(SongSize, 1);
			if (SongData == NULL) {
				cfclose(fp);
				mprintf( (1, "Error allocating %d bytes for '%s'", SongSize, filename ));
				return;
			}

			if ( cfread ( SongData, SongSize, 1, fp ) != 1 ) {
				mprintf( (1, "Error reading midi file, '%s'", filename ));
				cfclose(fp);
				d_free(SongData);
				return;
			}

			cfclose(fp);

			rw = SDL_RWFromConstMem(SongData, SongSize);
			SongHandle = Mix_LoadMUS_RW(rw);
		}
	}

	if (!SongHandle) {
		mprintf( (1, "MIDI Error : %s\n", Mix_GetError()));
		if (SongData)
			d_free(SongData);
		return;
	}

	// start the song playing
	Mix_PlayMusic(SongHandle, (loop ? -1 : 1));
	Mix_HookMusicFinished(digi_stop_current_song);

	return;
}


void digi_stop_current_song()
{
	if (!Digi_initialized) return;

	// Stop last song...
	if (SongHandle) {
		// stop the last MIDI song from playing
		Mix_HaltMusic();
		// uninitialize the last MIDI song
		Mix_FreeMusic( SongHandle );
		SongHandle = NULL;
	}
	if (SongData) {
		d_free(SongData);
		SongData = NULL;
	}
}


void digi_pause_midi()
{
	if (!Digi_initialized) return;
	
	Mix_PauseMusic();
}


void digi_resume_midi()
{
	if (!Digi_initialized) return;
	
	Mix_ResumeMusic();
}
