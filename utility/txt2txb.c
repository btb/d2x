#include <stdio.h>

void
main(int argc, char *argv[])
{
	FILE *file, *outfile;
	char equals, ch;
	int code;

	if (argc != 3) {
		printf("TXT2TXB V1.0 Copyright (c) Bryan Aamot, 1995\n"
		       "Text to TXB converter for Descent HOG files.\n"
		       "Converts a ascii text files to *.txb descent hog file format.\n"
		       "Usage: TXT2TXB <text file name> <txb file name>\n"
		       "Example: TXT2TXB briefing.txt briefing.txb\n");
		exit(1);
	}
	file = fopen(argv[1], "rb");
	if (!file) {
		printf("Can't open file (%s)\n", argv[1]);
		exit(2);
	}

	outfile = fopen(argv[2], "wb");
	if (!outfile) {
		printf("Can't open file (%s)\n", argv[2]);
		fclose(file);
		exit(2);
	}

	for (;;) {
		ch = getc(file);
		if (feof(file)) break;
		if (ch!=0x0d) {
			if (ch==0x0a) {
				fprintf(outfile, "\x0a");
			} else {
				code = ( ( (ch &0xfC) >> 2) + ( (ch &0x03) << 6 ) ) ^ 0xe9;
				fprintf(outfile, "%c", code);
			}
		}
	}

	fclose(outfile);
	fclose(file);
}
