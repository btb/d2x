#ifndef __HMP_H
#define __HMP_H
#include <physfs.h>


#define HMP_TRACKS 32

typedef struct hmp_track {
	unsigned char *data;
	unsigned int len;
	unsigned char *cur;
	unsigned int left;
	unsigned int cur_time;
} hmp_track;

typedef struct hmp_file {
	int num_trks;
	hmp_track trks[HMP_TRACKS];
	unsigned int cur_time;
	int tempo;
	unsigned char *pending;
	unsigned int pending_size;
	unsigned int pending_event;
	int stop;
	int bufs_in_mm;
	int bLoop;
	unsigned int midi_division;
} hmp_file;

hmp_file *hmp_open(const char *filename);
void hmp_close(hmp_file *hmp);
void hmp2mid(char *hmp_name, char *mid_name);

#endif
