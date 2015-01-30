/*
 * This code handles HMP files. It can:
 * - Open/read/close HMP files
 * - Play HMP via Windows MIDI
 * - Convert HMP to MIDI for further use
 * Based on work of Arne de Bruijn and the JFFEE project
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <physfs.h>

#include "hmp.h"
#include "u_mem.h"
#include "cfile.h"
#include "byteswap.h"


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
	int i, data, num_tracks, tempo;
	char buf[256];
	CFILE *fp;
	hmp_file *hmp;
	unsigned char *p;

	if (!(fp = cfopen((char *)filename, "rb")))
		return NULL;

	MALLOC(hmp, hmp_file, 1);
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

	if (cfseek(fp, 0x38, SEEK_SET))
	{
		cfclose(fp);
		hmp_close(hmp);
		return NULL;
	}
	if (cfread(&tempo, 4, 1, fp) != 1)
	{
		cfclose(fp);
		hmp_close(hmp);
		return NULL;
	}
	hmp->tempo = INTEL_INT(tempo);

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

		MALLOC(p, unsigned char, data);
		if (!(hmp->trks[i].data = p))
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

static unsigned int hmptrk2mid(ubyte* data, int size, unsigned char **midbuf, unsigned int *midlen)
{
	ubyte *dptr = data;
	ubyte lc1 = 0,last_com = 0;
	uint d;
	int n1, n2;
	unsigned int offset = *midlen;

	while (data < dptr + size)
	{
		if (data[0] & 0x80) {
			ubyte b = (data[0] & 0x7F);
			*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 1);
			memcpy(&(*midbuf)[*midlen], &b, 1);
			*midlen += 1;
		}
		else {
			d = (data[0] & 0x7F);
			n1 = 0;
			while ((data[n1] & 0x80) == 0) {
				n1++;
				d += (data[n1] & 0x7F) << (n1 * 7);
				}
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
				*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 1);
				memcpy(&(*midbuf)[*midlen], &b, 1);
				*midlen += 1;
				}
			data += n1;
		}
		data++;
		if (*data == 0xFF) { //meta?
			*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 3 + data[2]);
			memcpy(&(*midbuf)[*midlen], data, 3 + data[2]);
			*midlen += 3 + data[2];
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
					{
						*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 1);
						memcpy(&(*midbuf)[*midlen], &lc1, 1);
						*midlen += 1;
					}
					*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 2);
					memcpy(&(*midbuf)[*midlen], data + 1, 2);
					*midlen += 2;
					data += 3;
					break;
				case 0xC0:
				case 0xD0:
					if (lc1 != last_com)
					{
						*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 1);
						memcpy(&(*midbuf)[*midlen], &lc1, 1);
						*midlen += 1;
					}
					*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 1);
					memcpy(&(*midbuf)[*midlen], data + 1, 1);
					*midlen += 1;
					data += 2;
					break;
				default:
					return 0;
				}
			last_com = lc1;
		}
	}
	return (*midlen - offset);
}

ubyte tempo [19] = {'M','T','r','k',0,0,0,11,0,0xFF,0x51,0x03,0x18,0x80,0x00,0,0xFF,0x2F,0};

void hmp2mid(char *hmp_name, unsigned char **midbuf, unsigned int *midlen)
{
	int mi, i;
	short ms, time_div = 0xC0;
	hmp_file *hmp=NULL;

	hmp = hmp_open(hmp_name);
	if (hmp == NULL)
		return;

	*midlen = 0;
	time_div = hmp->tempo*1.6;

	// write MIDI-header
	*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 4);
	memcpy(&(*midbuf)[*midlen], "MThd", 4);
	*midlen += 4;
	mi = MIDIINT(6);
	*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + sizeof(mi));
	memcpy(&(*midbuf)[*midlen], &mi, sizeof(mi));
	*midlen += sizeof(mi);
	ms = MIDISHORT(1);
	*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + sizeof(ms));
	memcpy(&(*midbuf)[*midlen], &ms, sizeof(ms));
	*midlen += sizeof(ms);
	ms = MIDISHORT(hmp->num_trks);
	*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + sizeof(ms));
	memcpy(&(*midbuf)[*midlen], &ms, sizeof(ms));
	*midlen += sizeof(ms);
	ms = MIDISHORT(time_div);
	*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + sizeof(ms));
	memcpy(&(*midbuf)[*midlen], &ms, sizeof(ms));
	*midlen += sizeof(ms);
	*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + sizeof(tempo));
	memcpy(&(*midbuf)[*midlen], &tempo, sizeof(tempo));
	*midlen += sizeof(tempo);

	// tracks
	for (i = 1; i < hmp->num_trks; i++)
	{
		int midtrklenpos = 0;

		*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + 4);
		memcpy(&(*midbuf)[*midlen], "MTrk", 4);
		*midlen += 4;
		midtrklenpos = *midlen;
		mi = 0;
		*midbuf = (unsigned char *) d_realloc(*midbuf, *midlen + sizeof(mi));
		*midlen += sizeof(mi);
		mi = hmptrk2mid(hmp->trks[i].data, hmp->trks[i].len, midbuf, midlen);
		mi = MIDIINT(mi);
		memcpy(&(*midbuf)[midtrklenpos], &mi, 4);
	}

	hmp_close(hmp);
}
