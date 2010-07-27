/*
 * This code handles HMP files. It can:
 * - Open/read/close HMP files
 * - Play HMP via Windows MIDI
 * - Convert HMP to MIDI for further use
 * Based on work of Arne de Bruijn and the JFFEE project
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <physfs.h>
#include "hmp.h"
#include "u_mem.h"
#include "cfile.h"

#ifdef WORDS_BIGENDIAN
#define MIDIINT(x) (x)
#define MIDISHORT(x) (x)
#else
#define MIDIINT(x) SWAPINT(x)
#define MIDISHORT(x) SWAPSHORT(x)
#endif


// READ/OPEN/CLOSE HMP

void hmp_close(hmp_file *hmp)
{
	int i;

	for (i = 0; i < hmp->num_trks; i++)
		if (hmp->trks[i].data)
			d_free(hmp->trks[i].data);
	d_free(hmp);
}

hmp_file *hmp_open(const char *filename) {
	int i;
	char buf[256];
	long data;
	CFILE *fp;
	hmp_file *hmp;
	int num_tracks;
	unsigned char *p;

	if (!(fp = cfopen((char *)filename, "rb")))
		return NULL;

	hmp = d_malloc(sizeof(hmp_file));
	if (!hmp) {
		cfclose(fp);
		return NULL;
	}

	memset(hmp, 0, sizeof(*hmp));

	if ((cfread(buf, 1, 8, fp) != 8) || (memcmp(buf, "HMIMIDIP", 8)))
	{
		cfclose(fp);
		hmp_close(hmp);
		return NULL;
	}

	if (cfseek(fp, 0x30, SEEK_SET))
	{
		cfclose(fp);
		hmp_close(hmp);
		return NULL;
	}

	if (cfread(&num_tracks, 4, 1, fp) != 1)
	{
		cfclose(fp);
		hmp_close(hmp);
		return NULL;
	}

	if ((num_tracks < 1) || (num_tracks > HMP_TRACKS))
	{
		cfclose(fp);
		hmp_close(hmp);
		return NULL;
	}

	hmp->num_trks = num_tracks;
	hmp->tempo = 120;

	if (cfseek(fp, 0x308, SEEK_SET))
	{
		cfclose(fp);
		hmp_close(hmp);
		return NULL;
	}

	for (i = 0; i < num_tracks; i++) {
		if ((cfseek(fp, 4, SEEK_CUR)) || (cfread(&data, 4, 1, fp) != 1))
		{
			cfclose(fp);
			hmp_close(hmp);
			return NULL;
		}

		data -= 12;
		hmp->trks[i].len = data;

		if (!(p = hmp->trks[i].data = d_malloc(data)))
		{
			cfclose(fp);
			hmp_close(hmp);
			return NULL;
		}

		/* finally, read track data */
		if ((cfseek(fp, 4, SEEK_CUR)) || (cfread(p, data, 1, fp) != 1))
		{
			cfclose(fp);
			hmp_close(hmp);
			return NULL;
		}
	}
	cfclose(fp);
	return hmp;
}


// CONVERSION FROM HMP TO MIDI

static int hmptrk2mid(ubyte* data, int size, PHYSFS_file *mid)
{
	ubyte *dptr = data;
	ubyte lc1 = 0,last_com = 0;
	uint t = 0, d;
	int n1, n2;
	int offset = cftell(mid);

	while (data < dptr + size)
	{
		if (data[0] & 0x80) {
			ubyte b = (data[0] & 0x7F);
			PHYSFS_write(mid, &b, sizeof (b), 1);
			t+=b;
		}
		else {
			d = (data[0] & 0x7F);
			n1 = 0;
			while ((data[n1] & 0x80) == 0) {
				n1++;
				d += (data[n1] & 0x7F) << (n1 * 7);
				}
			t += d;
			n1 = 1;
			while ((data[n1] & 0x80) == 0) {
				n1++;
				if (n1 == 4)
					return 0;
				}
			for(n2 = 0; n2 <= n1; n2++) {
				ubyte b = (data[n1 - n2] & 0x7F);

				if (n2 != n1)
					b |= 0x80;
				PHYSFS_write(mid, &b, sizeof(b), 1);
				}
			data += n1;
		}
		data++;
		if (*data == 0xFF) { //meta?
			PHYSFS_write(mid, data, 3 + data [2], 1);
			if (data[1] == 0x2F)
				break;
		}
		else {
			lc1=data[0] ;
			if ((lc1&0x80) == 0)
				return 0;
			switch (lc1 & 0xF0) {
				case 0x80:
				case 0x90:
				case 0xA0:
				case 0xB0:
				case 0xE0:
					if (lc1 != last_com)
						PHYSFS_write(mid, &lc1, sizeof (lc1), 1);
					PHYSFS_write(mid, data + 1, 2, 1);
					data += 3;
					break;
				case 0xC0:
				case 0xD0:
					if (lc1 != last_com)
						PHYSFS_write(mid, &lc1, sizeof (lc1), 1);
					PHYSFS_write(mid, data + 1, 1, 1);
					data += 2;
					break;
				default:
					return 0;
				}
			last_com = lc1;
		}
	}
	return (cftell(mid) - offset);
}

ubyte tempo [19] = {'M','T','r','k',0,0,0,11,0,0xFF,0x51,0x03,0x18,0x80,0x00,0,0xFF,0x2F,0};

void hmp2mid(char *hmp_name, char *mid_name)
{
	PHYSFS_file *mid=NULL;
	int mi, i, loc;
	short ms;
	hmp_file *hmp=NULL;

	hmp = hmp_open(hmp_name);
	if (hmp == NULL)
		return;
	mid = PHYSFSX_openWriteBuffered(mid_name);
	if (mid == NULL)
	{
		hmp_close(hmp);
		return;
	}
	// write MIDI-header
	PHYSFS_write(mid, "MThd", 4, 1);
	mi = MIDIINT(6);
	PHYSFS_write(mid, &mi, sizeof(mi), 1);
	ms = MIDISHORT(1);
	PHYSFS_write(mid, &ms, sizeof(ms), 1);
	ms = MIDISHORT(hmp->num_trks);
	PHYSFS_write(mid, &ms, sizeof(ms), 1);
	ms = MIDISHORT((short) 0xC0);
	PHYSFS_write(mid, &ms, sizeof(ms), 1);
	PHYSFS_write(mid, tempo, sizeof(tempo), 1);

	// tracks
	for (i = 1; i < hmp->num_trks; i++)
	{
		PHYSFS_write(mid, "MTrk", 4, 1);
		loc = cftell(mid);
		mi = 0;
		PHYSFS_write(mid, &mi, sizeof(mi), 1);
		mi = hmptrk2mid(hmp->trks[i].data, hmp->trks[i].len, mid);
		mi = MIDIINT(mi);
		cfseek(mid, loc, SEEK_SET);
		PHYSFS_write(mid, &mi, sizeof(mi), 1);
		cfseek(mid, 0, SEEK_END);
	}

	hmp_close(hmp);
	cfclose(mid);
}
