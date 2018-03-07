//
// Created by sf on 4/15/16.
//

#include <physfs.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
// endianness check
//#include <SDL_endian.h>

#include "strutil.h"

char *substr(const char *str, size_t pos, size_t len)
{
	char *sub = malloc(len + 1);
	memcpy(sub, &str[pos], len);
	sub[len] = '\0';
	return len;
}

size_t findstr(const char *haystack, const char *needle, size_t pos)
{
	char *ptr = strstr(&haystack[pos], needle);
	if (ptr == NULL)
		return (size_t)-1;
	return ptr - haystack;
}

void strlower(char *str)
{
	for (int i = 0; str[i]; i++) {
		str[i] = tolower(str[i]);
	}
}

uint64_t file_size(const char *filename)
{
	struct stat statbuf;
	stat(filename, &statbuf);
	return statbuf.st_size;
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define LogInfo(...) printf(__VA_ARGS__)
#define LogWarning(...) printf(__VA_ARGS__)
#define LogError(...) printf(__VA_ARGS__)

// We actually only use BINARY here, but just for the sake of completion
typedef enum CueFileType
{
	CueFileType_FT_UNDEFINED, // Should only be set if parsing failed
	CueFileType_FT_BINARY,
	CueFileType_FT_MOTOROLA,
	CueFileType_FT_AIFF,
	CueFileType_FT_WAVE,
	CueFileType_FT_MP3
} CueFileType;

// FIXME: Add more (all?) supported formats?
typedef enum CueTrackMode
{
	CueTrackMode_MODE_UNDEFINED, // Should only be set if parsing failed
	CueTrackMode_MODE1_2048,
	CueTrackMode_MODE1_2352,
	CueTrackMode_MODE2_2048,
	CueTrackMode_MODE2_2324,
	CueTrackMode_MODE2_2336,
	CueTrackMode_MODE2_2352
} CueTrackMode;

// FIXME: This is a very incomplete CueSheet parser!
typedef struct CueParser
{
//private:
	enum
	{
		PARSER_START,
		PARSER_FILE,
		PARSER_TRACK,
		PARSER_FINISH,
		PARSER_ERROR
	} parserState;

	/*    static std::regex commandRegex ;
	    static std::regex fileArgRegex ;
	    static std::regex trackArgRegex;
	    static std::regex indexArgRegex;*/

	char *dataFileName;
	CueFileType fileType;
	CueTrackMode trackMode;
} CueParser;

	// Parse command while not being in a specific context
	int _CueParser_parseStart(CueParser *this, char *cmd, char *arg)
	{
		// Waiting for "FILE" command
		if (stricmp(cmd, "FILE") != 0)
		{
			LogInfo("Encountered unexpected command: \"%s\", ignoring", cmd);
			return 0;
		}
		// auto matchIter = std::sregex_iterator(arg.begin(), arg.end(), fileArgRegex);
		// FILE argument might be in quotation marks - better handle that

		size_t first_char = 0, last_char = strlen(arg) - 1;
		if (arg[first_char] == '"')
		{
			while ((last_char > 0) && (arg[last_char] != '"'))
			{
				last_char--;
			}
			// Trim quotation marks
			last_char -= 1;
			first_char += 1;
		}
		else
		{
			// Just find the last non-whitespace character
			last_char = first_char + 1;
			while ((last_char < strlen(arg)) && !isspace(arg[last_char]))
			{
				last_char++;
			}
			if (last_char == strlen(arg))
			{
				LogError("Malformed argument for FILE command (arguments are: \"%s\")",
				         arg);
				return 0;
			}
			last_char -= 1;
		}
		if (last_char >= first_char)
		{
			this->dataFileName = substr(arg, first_char, last_char - first_char + 1);
			LogInfo("Reading from \"%s\"", this->dataFileName);
		}
		else
		{
			LogError("Bad file name for FILE command (arguments are: \"%s\")", arg);
			return 0;
		}

		// Find the file type string
		first_char = last_char + 1;
		while ((first_char < strlen(arg)) && !isalnum(arg[first_char]))
		{
			first_char++;
		}
		if (first_char == strlen(arg))
		{
			LogError("File type not specified for \"%s\" (arguments are: \"%s\")", this->dataFileName,
			         arg);
			return 0;
		}
		last_char = strlen(arg) - 1;
		while ((last_char > first_char) && isspace(arg[last_char]))
		{
			last_char--;
		}

		char *fileTypeStr = substr(arg, first_char, last_char - first_char + 1);

		if (stricmp(fileTypeStr, "BINARY") != 0)
		{
			LogError("Unsupported file type: \"%s\"", fileTypeStr);
			this->parserState = PARSER_ERROR;
			this->fileType = CueFileType_FT_UNDEFINED;
			free(fileTypeStr);
			return 0;
		}
		free(fileTypeStr);
		this->fileType = CueFileType_FT_BINARY;
		return 1;
	}

	// Parse command while being in a FILE context
	int _CueParser_parseFile(CueParser *this, char *cmd, char *arg)
	{
		// Waiting for the "TRACK" command
		if (stricmp(cmd, "TRACK") != 0)
		{
			// According to
			// https://www.gnu.org/software/ccd2cue/manual/html_node/FILE-_0028CUE-Command_0029.html#FILE-_0028CUE-Command_0029
			// only TRACK is allowed after FILE
			LogError("Encountered unexpected command: \"%s\" (only TRACK is allowed)", cmd);
			this->parserState = PARSER_ERROR;
			this->fileType = CueFileType_FT_UNDEFINED;
			return 0;
		}

		// Read track number
		size_t first_char = 0;
		while ((first_char < strlen(arg)) && isspace(arg[first_char]))
		{
			first_char++;
		}
		size_t last_char = first_char;
		while ((last_char < strlen(arg)) && isdigit(arg[last_char]))
		{
			last_char++;
		}
		char *s = substr(arg, first_char, last_char - first_char + 1);
		int trackNumber = atoi(s);
		free(s);

		if (trackNumber > 1)
		{
			LogWarning("First track is not numbered 1 (actual number is %d)", trackNumber);
		}

		// Read track mode
		first_char = last_char + 1;
		last_char = strlen(arg) - 1;
		while ((first_char <= last_char) && isspace(arg[first_char]))
		{
			first_char++;
		}
		while ((last_char >= first_char) && isspace(arg[last_char]))
		{
			last_char--;
		}
		char *modeStr = substr(arg, first_char, last_char - first_char + 1);
		this->trackMode = CueTrackMode_MODE_UNDEFINED;
		if (stricmp(modeStr, "MODE1/2048") == 0)
			this->trackMode = CueTrackMode_MODE1_2048;
		else if (stricmp(modeStr, "MODE1/2352") == 0)
			this->trackMode = CueTrackMode_MODE1_2352;
		else if (stricmp(modeStr, "MODE2/2048") == 0)
			this->trackMode = CueTrackMode_MODE2_2048;
		else if (stricmp(modeStr, "MODE2/2324") == 0)
			this->trackMode = CueTrackMode_MODE2_2324;
		else if (stricmp(modeStr, "MODE2/2336") == 0)
			this->trackMode = CueTrackMode_MODE2_2336;
		else if (stricmp(modeStr, "MODE2/2352") == 0)
			this->trackMode = CueTrackMode_MODE2_2352;
		if (this->trackMode == CueTrackMode_MODE_UNDEFINED)
		{
			LogError("Unknown/unimplemented mode \"%s\"", modeStr);
			this->parserState = PARSER_ERROR;
			free(modeStr);
			return 0;
		}
		free(modeStr);
		return 1;
	}

	// Parse command while being in a TRACK context
	int _CueParser_parseTrack(CueParser *this, char *cmd, char *arg)
	{
		// TODO: check for possible commands, put parser into an "error" state if command is not
		// valid
		if (stricmp(cmd, "INDEX") != 0)
		{
			LogInfo("Encountered unexpected/unknown command: \"%s\", ignoring", cmd);
			return 0;
		}
		// FIXME: I seriously could not make heads or tails of these indices.
		return 1;
	}

	int _CueParser_parse(CueParser *this, const char *cueFilename)
	{
		FILE *cueFile = fopen(cueFilename, "rb");
		if (!cueFile)
		{
			// Stream is unusable, bail out
			return 0;
		}
		while (cueFile)
		{
			char *cueLine = NULL;
			fgets(cueLine, 1024, cueFile);

			// Cut the leading whitespaces
			size_t lead_whitespace = 0;
			while ((lead_whitespace < strlen(cueLine)) && isspace(cueLine[lead_whitespace]))
			{
				lead_whitespace++;
			}
			if (lead_whitespace == strlen(cueLine))
			{
				continue;
			}
			size_t first_whitespace = findstr(cueLine, " ", lead_whitespace);
			if (first_whitespace == (size_t)-1)
			{
				continue;
			}
			char *command = substr(cueLine, lead_whitespace, first_whitespace - lead_whitespace);
			size_t last_whitespace = first_whitespace;
			while ((last_whitespace < strlen(cueLine)) && isspace(cueLine[last_whitespace]))
			{
				last_whitespace++;
			}
			char *arg = substr(cueLine, 0, last_whitespace);
			switch (this->parserState)
			{
				case PARSER_START:
					if (_CueParser_parseStart(this, command, arg))
					{
						this->parserState = PARSER_FILE;
					}
					break;
				case PARSER_FILE:
					if (_CueParser_parseFile(this, command, arg))
					{
						this->parserState = PARSER_TRACK;
					}
					break;
				case PARSER_TRACK:
					if (_CueParser_parseTrack(this, command, arg))
					{
						this->parserState = PARSER_FINISH;
					}
					break;
				default:
					LogError("Invalid CueParser state!");
			}
			free(command);
			free(arg);
			if ((this->parserState == PARSER_FINISH) || (this->parserState == PARSER_ERROR))
				return this->parserState == PARSER_FINISH;
		}
		return this->parserState == PARSER_FINISH;
	}

//public:
	CueParser *CueParser_new(const char *cueFile)
	{
		CueParser *this = malloc(sizeof(CueParser));

		this->parserState = PARSER_START;
		this->fileType = CueFileType_FT_UNDEFINED;
		this->trackMode = CueTrackMode_MODE_UNDEFINED;

		_CueParser_parse(this, cueFile);
		return this;
	}

	int CueParser_isValid(CueParser *this) { return this->parserState == PARSER_FINISH; }
	char *CueParser_getDataFileName(CueParser *this) { return this->dataFileName; }
	CueFileType CueParser_getDataFileType(CueParser *this) { return this->fileType; }
	CueTrackMode CueParser_getTrackMode(CueParser *this) { return this->trackMode; }
	void CueParser_delete(CueParser *this) { free(this->dataFileName); free(this); }
// End of CueParser

// --- iso9660 reader follows

// lsb-msb type as defined by iso9660

typedef struct Int16LsbMsb
{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	uint16_t val;       // lsb
	uint16_t __padding; // msb
#elif SDL_BYTEORDER == SDL_BIG_ENDIAN
	uint16_t __padding;
	uint16_t val;
#else
#error Unknown endianness!
#endif
} Int16LsbMsb;

typedef struct Sint16LsbMsb
{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	int16_t val;
	int16_t __padding;
#elif SDL_BYTEORDER == SDL_BIG_ENDIAN
	int16_t __padding;
	int16_t val;
#else
#error Unknown endianness!
#endif
} Sint16LsbMsb;

typedef struct Int32LsbMsb
{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	uint32_t val;
	uint32_t __padding;
#elif SDL_BYTEORDER == SDL_BIG_ENDIAN
	uint32_t __padding;
	uint32_t val;
#else
#error Unknown endianness!
#endif
} Int32LsbMsb;

typedef struct Sint32LsbMsb
{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	int32_t val;
	int32_t __padding;
#elif SDL_BYTEORDER == SDL_BIG_ENDIAN
	int32_t __padding;
	int32_t val;
#else
#error Unknown endianness!
#endif
} Sint32LsbMsb;

typedef struct DecDatetime
{
	char year[4];
	char month[2];
	char day[2];
	char hour[2];
	char minute[2];
	char second[2];
	char hndSecond[2]; // hundredth of a second
	uint8_t gmtOff;
} DecDatetime;

	// return value will be good for is checking whether two files on the same disk
	// were created at the same moment!
	PHYSFS_sint64 DecDatetime_toUnixTime(DecDatetime d) // Convert to a saner (?) time representation
	{
		// The following is clearly an example of now NOT to do time stuff
		// The spec states that all fields are ASCII... we're gonna abuse that
		int year_int =
			(d.year[0] - '0') * 1000 + (d.year[1] - '0') * 100 + (d.year[2] - '0') * 10 + (d.year[3] - '0');
		int month_int = (d.month[0] - '0') * 10 + (d.month[1] - '0');
		int day_int = (d.day[0] - '0') * 10 + (d.day[1] - '0');
		int hour_int = (d.hour[0] - '0') * 10 + (d.hour[1] - '0');
		int minute_int = (d.minute[0] - '0') * 10 + (d.minute[1] - '0');
		int second_int = (d.second[0] - '0') * 10 + (d.second[1] - '0');
		// int hndsec_int = (d.hndSecond[0] - '0') * 10 +
		//                 (d.hndSecond[1] - '0');
		// The resulting number is very obviously erroneous, because I don't
		// account for leap years/seconds correctly
		struct tm time_struct;
		time_struct.tm_sec = second_int;
		time_struct.tm_min = minute_int;
		time_struct.tm_hour = hour_int;
		time_struct.tm_mday = day_int;
		time_struct.tm_mon = month_int;
		time_struct.tm_year = year_int - 1900;
		time_struct.tm_isdst = 0;

		PHYSFS_sint64 unixSeconds = mktime(&time_struct);
		return unixSeconds;
	}
// End of DecDatetime

// Okay, TWO different datetime formats?
typedef struct DirDatetime
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t gmtOffset;
} DirDatetime;

	PHYSFS_sint64 DirDatetime_toUnixTime(DirDatetime d)
	{
		struct tm time_struct;
		time_struct.tm_sec = d.second;
		time_struct.tm_min = d.minute;
		time_struct.tm_hour = d.hour;
		time_struct.tm_mday = d.day;
		time_struct.tm_mon = d.month;
		time_struct.tm_year = d.year;
		time_struct.tm_isdst = 0;

		PHYSFS_sint64 unixSeconds = mktime(&time_struct);

		return unixSeconds;
	}
// End of DirDatetime

//static_assert(sizeof(Int16LsbMsb) == 4, "Unexpected int16_lsb_msb_size!");
//static_assert(sizeof(Sint16LsbMsb) == 4, "Unexpected int16_lsb_msb_size!");
//static_assert(sizeof(Int32LsbMsb) == 8, "Unexpected int16_lsb_msb_size!");
//static_assert(sizeof(Sint32LsbMsb) == 8, "Unexpected int16_lsb_msb_size!");
//static_assert(sizeof(DecDatetime) == 17, "Unexpected dec_datetime size!");
//static_assert(sizeof(DirDatetime) == 7, "Unexpected dir_datetime size!");

typedef struct CueIO
{
//private:
	//friend class CueArchiver;

	char *imageFile;  // For stream duplication
	int32_t lbaStart;   // Starting LBA for this stream
	int32_t lbaCurrent; // Current block for this stream
	int32_t posInLba;   // Current position in lba
	int64_t length;     // Allowed length of the stream
	CueFileType fileType;
	CueTrackMode trackMode;
	FILE *fileStream;
} CueIO;

	int64_t _CueIO_lbaToByteOffset(CueIO *this, uint32_t lba);

	CueIO *_CueIO_new(const char *fileName, uint32_t lbaStart, int64_t length,
		CueFileType fileType /* = CueFileType_FT_BINARY */,
		CueTrackMode trackMode /* = CueTrackMode_MODE1_2048 */)
	{
		CueIO *this = malloc(sizeof(CueIO));

		this->imageFile = strdup(fileName);
		this->lbaStart = lbaStart;
		this->lbaCurrent = lbaStart;
		this->posInLba = 0;
		this->length = length;
		this->fileType = fileType;
		this->trackMode = trackMode;

		this->fileStream = fopen(fileName, "rb");
		fseek(this->fileStream, _CueIO_lbaToByteOffset(this, this->lbaStart), SEEK_SET);
		return this;
	}

	// Convert LBA to actual position in the stream
	int64_t _CueIO_lbaToByteOffset(CueIO *this, uint32_t lba)
	{
		switch (this->trackMode)
		{
			// Each block is 2048 bytes, translation is trivial
			case CueTrackMode_MODE1_2048:
			case CueTrackMode_MODE2_2048:
				return lba * 2048;
			// Each block is 2048 bytes, but there's a prefix and
			// a postfix area for each sector
			case CueTrackMode_MODE1_2352:
			return lba * 2352 + 12 + 4; // 12 sync bytes, 4 header bytes
			// FIXME: Reality check?
			// For mode2, each block might be slightly larger than for mode1
			// These cases are dealing with "cooked" data
			case CueTrackMode_MODE2_2324:
				return lba * 2324; // Each sector is 2324 bytes
									// Strangely enough, mode2/2336 is the same as mode2/2352 without header?
			case CueTrackMode_MODE2_2336:
				return lba * 2336 + 8;
			case CueTrackMode_MODE2_2352:
				return lba * 2048 + 12 + 4 + 8;
			default:
				LogError("Unknown track mode set!");
				// Return negative offset to indicate error
				return -1;
		}
	}

	// Get the "user data" block size
	int32_t _CueIO_blockSize(CueIO *this)
	{
		// FIXME: Reality check?
		switch (this->trackMode)
		{
			case CueTrackMode_MODE1_2048:
			case CueTrackMode_MODE2_2048:
			case CueTrackMode_MODE1_2352:
			case CueTrackMode_MODE2_2352:
			// Some docs say mode2 contains 2336 bytes of user data per block,
			// others insist on 2048 bytes...
			case CueTrackMode_MODE2_2336:
				return 2048;
			case CueTrackMode_MODE2_2324:
				return 2324;
			default:
				LogError("Bad track mode!");
		}
		// Unsupported track mode
		return -1;
	}

	// Get the "binary" block size
	int32_t _CueIO_binBlockSize(CueIO *this)
	{
		switch (this->trackMode)
		{
			case CueTrackMode_MODE1_2048:
			case CueTrackMode_MODE2_2048:
				return 2048;
			case CueTrackMode_MODE1_2352:
			case CueTrackMode_MODE2_2352:
				return 2352;
			case CueTrackMode_MODE2_2336:
				return 2336;
			case CueTrackMode_MODE2_2324:
				return 2324;
			default:
				LogError("Bad track mode!");
		}
		// Unsupported track mode
		return -1;
	}

	// Offset of the user data portion of the block
	int32_t _CueIO_binDataOffset(CueIO *this)
	{
		switch (this->trackMode)
		{
			// FIXME: Check mode2 correctness??
			case CueTrackMode_MODE1_2048:
			case CueTrackMode_MODE2_2048:
				return 0; // Only user data is present here
			case CueTrackMode_MODE2_2324:
				return 0;
			case CueTrackMode_MODE1_2352:
				return 12 + 4; // 12 bytes sync, 4 bytes header
			case CueTrackMode_MODE2_2352:
				return 12 + 4 + 8; // 12 bytes sync, 4 bytes header, 8 bytes subheader
			case CueTrackMode_MODE2_2336:
				return 8; // 8 bytes subheader (?)
			default:
				LogError("Bad track mode!");
		}
		// Unsupported track mode
		return -1;
	}

	int _CueIO_seek(CueIO *this, int64_t offset);

	int64_t _CueIO_read(CueIO *this, void *buf, int64_t len)
	{
		// Ignore size 0 reads
		if (!len)
			return 0;
		// Since we probably will have to read in parts,
		// we have to make the buffer seekable
		char *bufWrite = (char *)buf;
#if 0 // FIXME: This code won't work, actually.
        // If the data is "cooked", just read it.
        if (trackMode == CUE_TrackMode_MODE1_2048 ||
            trackMode == CUE_TrackMode_MODE2_2048)
        {
            // FIXME: This won't correctly handle multi-extent files
            lbaCurrent += len / 2048;
            int64_t start = ftell(fileStream);
            fread(bufWrite, 1, len, fileStream);
            return ftell(fileStream) - start;
        }
#endif
		int64_t remainLength = this->length - (this->lbaCurrent - this->lbaStart) * _CueIO_blockSize(this) - this->posInLba;
		if (remainLength < 0)
		{
			LogError("Trying to read past end of stream!");
			return -1;
		}
		if (len > remainLength)
		{
			// FIXME: This produces way too much output as well, though we could use it somehow?
			// LogWarning("Requested read of size %" PRIu64 " is bigger than remaining %" PRIu64
			//           " bytes",
			//           len, remainLength);
			len = remainLength;
		}
		int64_t totalRead = 0;
		do
		{
			int64_t readSize = MIN(len - totalRead, (int64_t)(_CueIO_blockSize(this) - this->posInLba));
			size_t count = fread(bufWrite + totalRead, 1, readSize, this->fileStream);
			totalRead += count;
			if (count != readSize)
			{
				LogWarning("Read buffer underrun! Wanted %" PRId64 " bytes, got %" PRId64, readSize,
				           count);
				return totalRead;
			}
			this->posInLba += readSize;
			if (this->posInLba >= _CueIO_blockSize(this))
			{
				this->posInLba = 0;
				this->lbaCurrent += 1;
				_CueIO_seek(this, (this->lbaCurrent - this->lbaStart) * _CueIO_blockSize(this));
			}
		} while (len > totalRead);
		return totalRead;
	}

	int _CueIO_seek(CueIO *this, int64_t offset)
	{
		if (offset > this->length)
		{
			return 0;
		}
		// FIXME: This assumes the offset is more or less *sane*
		uint32_t blockOffset = offset / _CueIO_blockSize(this);
		uint32_t posInBlock = offset % _CueIO_blockSize(this);

		this->lbaCurrent = this->lbaStart + blockOffset;
		this->posInLba = posInBlock;
		uint64_t binOffset = _CueIO_lbaToByteOffset(this, this->lbaCurrent) + this->posInLba;
		return (fseek(this->fileStream, binOffset, SEEK_SET) != -1);
	}

	PHYSFS_sint64 _CueIO_tell(CueIO *this)
	{
		// return _CueIO_lbaToByteOffset(lbaCurrent - lbaStart) + posInLba;
		return _CueIO_blockSize(this) * (this->lbaCurrent - this->lbaStart) + this->posInLba;
	}

	CueIO *_CueIO_newFromCueIO(const CueIO *other)
	{
		CueIO *this = malloc(sizeof(CueIO));
		
		this->imageFile = strdup(other->imageFile);
		this->lbaStart = other->lbaStart;
		this->lbaCurrent = other->lbaCurrent;
		this->posInLba = other->posInLba;
		this->length = other->length;
		this->fileType = other->fileType;
		this->trackMode = other->trackMode;

		this->fileStream = fopen(this->imageFile, "rb");
		fseek(this->fileStream, _CueIO_lbaToByteOffset(this, this->lbaStart) + this->posInLba, SEEK_SET);
		return this;
	}

	void _CueIO_delete(CueIO *this) { fclose(this->fileStream); free(this->imageFile); free(this); }

	static PHYSFS_sint64 cueIoRead(PHYSFS_Io *io, void *buffer, PHYSFS_uint64 len);
	static PHYSFS_sint64 cueIoWrite(PHYSFS_Io *io, const void *buffer, PHYSFS_uint64 len);
	static int cueIoSeek(PHYSFS_Io *io, PHYSFS_uint64 offset);
	static PHYSFS_sint64 cueIoTell(PHYSFS_Io *io);
	static PHYSFS_sint64 cueIoLength(PHYSFS_Io *io);
	static PHYSFS_Io *cueIoDuplicate(PHYSFS_Io *io);
	static int cueIoFlush(PHYSFS_Io *io);
	static void cueIoDestroy(PHYSFS_Io *io);

	static const PHYSFS_Io __PHYSFS_cueIoInterface =
	{
		0, NULL,
		cueIoRead,
		cueIoWrite,
		cueIoSeek,
		cueIoTell,
		cueIoLength,
		cueIoDuplicate,
		cueIoFlush,
		cueIoDestroy
	};

	static PHYSFS_Io *_CueIO_createIo()
	{
		PHYSFS_Io *io = malloc(sizeof (PHYSFS_Io));
		memcpy(io, &__PHYSFS_cueIoInterface, sizeof (*io));
		return io;
	}

//public:
	static PHYSFS_sint64 cueIoRead(PHYSFS_Io *io, void *buffer, PHYSFS_uint64 len)
	{
		CueIO *cio = (CueIO *)io->opaque;
		return _CueIO_read(cio, buffer, len);
	}

	// We always ignore write requests
	static PHYSFS_sint64 cueIoWrite(PHYSFS_Io *io, const void *buffer, PHYSFS_uint64 len)
	{
		return -1;
	}

	static int cueIoSeek(PHYSFS_Io *io, PHYSFS_uint64 offset)
	{
		CueIO *cio = (CueIO *)io->opaque;
		return _CueIO_seek(cio, offset);
	}

	static PHYSFS_sint64 cueIoTell(PHYSFS_Io *io)
	{
		CueIO *cio = (CueIO *)io->opaque;
		return _CueIO_tell(cio);
	}

	static PHYSFS_sint64 cueIoLength(PHYSFS_Io *io)
	{
		CueIO *cio = (CueIO *)io->opaque;
		return cio->length;
	}

	// A note on io->duplicate:
	// The physfs.h doc-comment states that duplicate should return a
	// "new value for a stream's (opaque) field", but that's actually
	// not true (according to implementations in the code).
	// In fact you have to construct a new PHYSFS_Io object, with no
	// dependencies on the old one.
	static PHYSFS_Io *cueIoDuplicate(PHYSFS_Io *io)
	{
		CueIO *cio = (CueIO *)io->opaque;
		// Just go ahead and construct a new file stream
		PHYSFS_Io *retval = _CueIO_createIo();
		// Set the appropriate fields
		io->opaque = _CueIO_newFromCueIO(cio);
		return retval;
	}

	static int cueIoFlush(PHYSFS_Io *io)
	{
		return 1;
	}

	static void cueIoDestroy(PHYSFS_Io *io)
	{
		CueIO *cio = (CueIO *)io->opaque;
		_CueIO_delete(cio);
		free(io);
	}

	static PHYSFS_Io *getIo(char *fileName, uint32_t lba, int64_t length, CueFileType ftype,
	                        CueTrackMode tmode)
	{
		CueIO *cio = _CueIO_new(fileName, lba, length, ftype, tmode);
		if (!cio->fileStream)
		{
			_CueIO_delete(cio);
			return NULL;
		}
		PHYSFS_Io *io = _CueIO_createIo();
		io->opaque = cio;
		return io;
	}
// End of CueIO

typedef struct FSEntry FSEntry;

#define PHYSFS_API_VERSION 0
typedef struct CueArchiver
{
//private:
	char *imageFile;
	CueFileType fileType;
	CueTrackMode trackMode;

	CueIO *cio;
	FSEntry *root;
} CueArchiver;

	typedef struct IsoVolumeDescriptor
	{
		uint8_t type;
		uint8_t identifier[5];
		uint8_t version;
		uint8_t _padding; // This field is here to avoid alignment issues.
		                  // It's only used in the boot volume descriptor, and
		                  // therefore not interesting to us.
		union {
			// Better not even try this one
			/*struct
			{
			    uint8_t bootSystemIdentifier__unused[31];
			    uint8_t bootIdentifier__unused[32];
			    uint8_t bootSystemUse__unused[1977];
			} boot;*/
			struct
			{
				// uint8_t __unused; // Disabled due to ___padding being there
				char sysIdentifier[32];
				char volIdentifier[32];
				uint8_t _unused_8[8];
				Int32LsbMsb volSpaceSize;
				uint8_t _unused_32[32];
				Int16LsbMsb volSetSz;
				Int16LsbMsb volSeqNr;
				Int16LsbMsb lbs;
				Int32LsbMsb pathTblSz;
				uint32_t pathTblLLoc;
				uint32_t optPathTblLLoc;
				uint32_t pathTblMLoc;
				uint32_t optPathTblMLoc;
				uint8_t rootDirEnt[34];
				char volSetIdentifier[128];
				char publisherIdentifier[128];
				char dataPrepIdentifier[128];
				char appIdentifier[128];
				char copyrightIdentifier[38];
				char abstractFileId[36];
				char biblioFileId[37];
				DecDatetime volCreationTime;
				DecDatetime volModificationTime;
				DecDatetime volExpirationTime;
				DecDatetime volEffectiveTime;
				uint8_t fileStructureVersion;
				uint8_t _unused1;
				uint8_t _app_defined__unused[512];
				uint8_t _reserved[653];
			} primary;
			// Supplementary volume descriptor is ignored completely
			struct
			{
				uint8_t _padding[2040];
			} terminator;
		};
	} IsoVolumeDescriptor;

	// NOTE: this would have all sorts of alignment issues if used with proper types!
	typedef struct IsoDirRecord_hdr
	{
		uint8_t length;
		uint8_t xarLength;
		uint8_t extentLoc[8];    // cast to int32_lsb_msb
		uint8_t extentLength[8]; // cast to int32_lsb_msb
		DirDatetime recTime;
		uint8_t flags;
		uint8_t fuSize;
		uint8_t gapSize;
		uint8_t volSeqNumber[4]; // cast to int16_lsb_msb
		uint8_t fnLength;
		// That's a bit of a hack, since actual filename might be sized differently
		char fileName[222];
	} IsoDirRecord_hdr;

	typedef enum FSEntryFlags
	{
		FSFLAG_HIDDEN = 0x01,
		FSFLAG_DIRENT = 0x02,
		FSFLAG_ASFILE = 0x04,
		FSFLAG_XATTRINFO = 0x08,
		FSFLAG_XATTRPERM = 0x10,
		FSFLAG_RESERVED1 = 0x20,
		FSFLAG_RESERVED2 = 0x40,
		FSFLAG_NOTFINAL = 0x80
	} FSEntryFlags;

	typedef struct FSEntry
	{
		char *name;
		enum
		{
			FSEntry_FS_FILE,
			FSEntry_FS_DIRECTORY
		} type;
		uint32_t offset;
		uint64_t length;
		int64_t timestamp;
		//		std::map<UString, FSEntry> children;
	} FSEntry;

	void _CueArchiver_readDir(CueArchiver *this, const IsoDirRecord_hdr *dirRecord, FSEntry *parent)
	{
		Int32LsbMsb lm_location;
		Int32LsbMsb lm_length;
		DirDatetime d_datetime;
		memcpy(&lm_location, dirRecord->extentLoc, sizeof(Int32LsbMsb));
		memcpy(&lm_length, dirRecord->extentLength, sizeof(Int32LsbMsb));
		memcpy(&d_datetime, &dirRecord->recTime, sizeof(DirDatetime));
		uint32_t location = lm_location.val;
		int32_t length = lm_length.val;
		int32_t readpos = 0;
		char *semicolonPos = strrchr((char *)dirRecord->fileName, ';');
		if (semicolonPos)
		{
			*semicolonPos = '\0';
		} // Ignore the semicolon and everything after it
		parent->name = strdup(dirRecord->fileName);
		// As of commit 07a2fe9, we only use lower-case names
		strlower(parent->name);
		parent->length = length;
		parent->offset = location;
		parent->timestamp = DirDatetime_toUnixTime(d_datetime);
#if 0 // Stop archiver from being extremely chatty
        LogInfo("Adding entry: %s", parent->name);
        LogInfo("  Location %" PRIu64, parent->offset);
        LogInfo("  Length: %" PRIu64, parent->length);
#endif
		if (!(dirRecord->flags & FSFLAG_DIRENT))
		{
			parent->type = FSEntry_FS_FILE;
			return;
		} // Next portion is directory-specific
		parent->type = FSEntry_FS_DIRECTORY;
		IsoDirRecord_hdr childDirRecord;
#if 0
        LogInfo("Recursing into: %s (location: %d)", parent.name, location);
#endif
		_CueIO_seek(this->cio, _CueIO_blockSize(this->cio) * location + readpos);
		do
		{
			// Find a non-empty record
			do
			{
				// Each record starts at an even offset
				if (readpos % 2)
				{
					readpos += 1;
					_CueIO_seek(this->cio, _CueIO_tell(this->cio) + 1);
				}
				// Read first 33 bytes containing everything but the name
				readpos += _CueIO_read(this->cio, &childDirRecord, 33);
				// We can safely bail out if we get over the record length
				if (readpos >= length)
					return;
				// We've read an empty record, that's fine
				if (childDirRecord.length == 0)
				{
					// Check if we crossed the boundary
					if (readpos % _CueIO_blockSize(this->cio) < 33)
					{
						// Yup, sure did.
						readpos -= readpos % _CueIO_blockSize(this->cio);
					}
					_CueIO_seek(this->cio, _CueIO_blockSize(this->cio) * location + readpos);
				}
			} while (childDirRecord.length == 0);

			if (childDirRecord.fnLength > 0)
			{
				readpos += _CueIO_read(this->cio, childDirRecord.fileName, childDirRecord.fnLength);
				childDirRecord.fileName[childDirRecord.fnLength] = '\0';
			}
			else
			{
				continue;
			}
			// Each (?) directory on a CD has a "this directory" and "parent directory" entries, we
			// just ignore them
			if (!isalnum(childDirRecord.fileName[0]))
			{
				continue;
			}

			// Now decode what we've read
			FSEntry childEntry;
			int64_t pos = _CueIO_tell(this->cio);
			_CueArchiver_readDir(this, &childDirRecord, &childEntry);
			// Reset reading position
			_CueIO_seek(this->cio, pos);
			// _CueIO_seek(this->cio, _CueIO_blockSize(this->cio) * location + readpos);
			//parent->children[childEntry.name] = childEntry;
		} while ((childDirRecord.length > 0));
	}

	//static_assert(sizeof(IsoVolumeDescriptor) == 2048, "Unexpected volume size!");
	//static_assert(sizeof(IsoDirRecord_hdr) == 255, "Unexpected direntry size!");
	//static_assert(offsetof(IsoDirRecord_hdr, fnLength) == 32, "Unexpected filename offset!");

	CueArchiver *_CueArchiver_new(char *fileName, CueFileType ftype, CueTrackMode tmode)
	{
		CueArchiver *this = malloc(sizeof(CueArchiver));

		this->imageFile = strdup(fileName);
		this->fileType = ftype;
		this->trackMode = tmode;

		// "Hey, a .cue-.bin file pair should be really easy to read!" - sfalexrog, 15.04.2016
		// FIXME: This fsize is completely and utterly wrong - unless you're reading an actual iso
		// (mode1_2048)
		uint64_t fsize = file_size(fileName);
		LogInfo("Opening file %s of size %" PRIu64, fileName, fsize);
		this->cio = _CueIO_new(fileName, 0, fsize, ftype, tmode);
		if (!this->cio->fileStream)
		{
			LogError("Could not open file: bad stream!");
		}
		_CueIO_seek(this->cio, _CueIO_blockSize(this->cio) * 16);
		LogInfo("Reading ISO volume descriptor");
		IsoVolumeDescriptor descriptor;
		_CueIO_read(this->cio, &descriptor, sizeof(descriptor));
		LogInfo("CD magic: %c, %c, %c, %c, %c", descriptor.identifier[0], descriptor.identifier[1],
		        descriptor.identifier[2], descriptor.identifier[3], descriptor.identifier[4]);
		const char magic[] = {'C', 'D', '0', '0', '1'};
		if (memcmp((void *)magic, (void *)descriptor.identifier, 5))
		{
			LogError("Bad CD magic!");
		}
		LogInfo("Descriptor type: %d", (int)descriptor.type);
		IsoDirRecord_hdr rootRecord;
		memcpy(&rootRecord, descriptor.primary.rootDirEnt, 34);
		LogInfo("Volume ID: %s", descriptor.primary.volIdentifier);
		LogInfo("Root dirent length: %d", (int)rootRecord.length);
		_CueArchiver_readDir(this, &rootRecord, this->root);
		
		return this;
	}

	void _CueArchiver_delete(CueArchiver *this) { _CueIO_delete(this->cio); free(this->imageFile); free(this); }

	const FSEntry *_CueArchiver_getFsEntry(const CueArchiver *this, const char *name)
	{
		const FSEntry *current = this->root;
		char *dname = strdup(name);
		if (strlen(dname) > 0)
		{
			//auto pathParts = dname.split("/");
			//for (auto ppart = pathParts.begin(); ppart != pathParts.end(); ppart++)
			//{
			//	auto subdir = current->children.find(*ppart);
			//	if (subdir == current->children.end())
			//	{
			//		// Not a valid directory, fail fast.
			//		PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
			//		free(dname);
			//		return NULL;
			//	}
			//	// Go into specified subdirectory
			//	current = &(subdir->second);
			//}
		}
		free(dname);
		return current;
	}

	PHYSFS_EnumerateCallbackResult _CueArchiver_enumerateFiles(CueArchiver *this, const char *dirname, PHYSFS_EnumerateCallback cb,
	                                              const char *origdir, void *callbackdata)
	{
		const FSEntry *current = _CueArchiver_getFsEntry(this, dirname);
		if (!current)
			return PHYSFS_ENUM_ERROR;
		if (current->type == FSEntry_FS_DIRECTORY)
		{
			//for (auto entry = current->children.begin(); entry != current->children.end(); entry++)
			//{
			//	auto ret = cb(callbackdata, origdir, entry->first.cStr());
			//	switch (ret)
			//	{
			//		case PHYSFS_ENUM_ERROR:
			//			PHYSFS_setErrorCode(PHYSFS_ERR_APP_CALLBACK);
			//			return PHYSFS_ENUM_ERROR;
			//		case PHYSFS_ENUM_STOP:
			//			return PHYSFS_ENUM_STOP;
			//		default:
			//			// Continue enumeration
			//			break;
			//	}
			//}
		}
		return PHYSFS_ENUM_OK;
	}

	PHYSFS_Io *_CueArchiver_openRead(CueArchiver *this, const char *fnm)
	{
		const FSEntry *entry = _CueArchiver_getFsEntry(this, fnm);
		if (!entry || (entry->type == FSEntry_FS_DIRECTORY))
		{
			return NULL;
		}
		return getIo(this->imageFile, entry->offset, entry->length, this->fileType, this->trackMode);
	}

	int _CueArchiver_stat(CueArchiver *this, const char *name, PHYSFS_Stat *stat)
	{
		const FSEntry *current = _CueArchiver_getFsEntry(this, name);
		if (!current)
		{
			return 0;
		}

		stat->readonly = 1;
		stat->accesstime = current->timestamp;
		stat->createtime = current->timestamp;
		stat->modtime = current->timestamp;
		switch (current->type)
		{
			case FSEntry_FS_FILE:
				stat->filetype = PHYSFS_FILETYPE_REGULAR;
				stat->filesize = current->length;
				break;
			case FSEntry_FS_DIRECTORY:
				stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
				stat->filesize = 0;
				break;
			default:
				// Well, this should never happen?
				LogError("Unexpected FSEntry::type value!");
		}
		return 1;
	}

//public:
	static void *cueOpenArchive(PHYSFS_Io *io, const char *filename, int forWriting, int *claimed)
	{
		LogWarning("Opening \"%s\"", filename);
		// FIXME: Here we assume the filename actually points to the actual .cue file,
		// ignoring the PHYSFS_Io (though how would we even read the accompanying file?)
		// TODO: Actually read from PHYSFS_Io to allow mounting non-CUE images?
		if (!filename)
		{
			LogError("FIXME: Cannot operate on purely-PhysFS_Io archives (need a filename)");
			return NULL;
		}

		if (forWriting)
		{
			LogError("Cue files cannot be written to");
			return NULL;
		}

		CueParser *parser = CueParser_new(filename);
		if (!CueParser_isValid(parser))
		{
			LogError("Could not parse file \"%s\"", filename);
			return NULL;
		}

		// We know it's a valid CUE file, so claim it
		*claimed = 1;

#if 0
		fs::path cueFilePath(filename);

		fs::path dataFilePath(cueFilePath.parent_path()); // parser.getDataFileName());
		dataFilePath /= parser.getDataFileName().cStr();
		if (exists(dataFilePath))
		{
			LogWarning("Could not find binary file \"%s\" referenced in the cuesheet",
			           CueParser_getDataFileName(parser));
			LogWarning("Trying case-insensitive search...");
			char *ucBin = CueParser_getDataFileName(parser);
			for (fs::directory_entry &dirent :
			fs::directory_iterator(cuefilepath.parent_path()))
			for (auto dirent_it = fs::directory_iterator(cuefilepath.parent_path());
			     dirent_it != fs::directory_iterator(); dirent_it++)
			{
				auto dirent = *dirent_it;
				loginfo("trying %s", dirent.path());
				char *ucdirent = dirent.path().filename();
				if (stricmp(ucdirent, ucbin) == 0)
				{
					datafilepath = cuefilepath.parent_path();
					datafilepath /= dirent.path().filename();
				}
			}
			if (exists(dataFilePath))
			{
				LogError("Binary file does not exist: \"%s\"", dataFilePath);
				return NULL;
			}
			LogWarning("Using \"%s\" as a binary file source", dataFilePath);
		}

		CueArchiver *archiver = CueArchiver_new(dataFilePath, CueParser_getDataFileType(parser),
		                       CueParser_getTrackMode(parser));
#endif
		CueParser_delete(parser);
		return NULL; //archiver;
	}

	static PHYSFS_EnumerateCallbackResult cueEnumerateFiles(void *opaque, const char *dirname,
	                                                        PHYSFS_EnumerateCallback cb,
	                                                        const char *origdir, void *callbackdata)
	{
		CueArchiver *archiver = (CueArchiver *)opaque;
		return _CueArchiver_enumerateFiles(archiver, dirname, cb, origdir, callbackdata);
	}

	static PHYSFS_Io *cueOpenRead(void *opaque, const char *fnm)
	{
		CueArchiver *archiver = (CueArchiver *)opaque;
		return _CueArchiver_openRead(archiver, fnm);
	}

	static PHYSFS_Io *cueOpenWrite(void *opaque, const char *filename)
	{
		PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
		return NULL;
	}

	static PHYSFS_Io *cueOpenAppend(void *opaque, const char *filename)
	{
		PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
		return NULL;
	}

	static int cueRemove(void *opaque, const char *filename)
	{
		PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
		return 0;
	}

	static int cueMkdir(void *opaque, const char *filename)
	{
		PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
		return 0;
	}

	static int cueStat(void *opaque, const char *fn, PHYSFS_Stat *stat)
	{
		CueArchiver *archiver = (CueArchiver *)opaque;
		return _CueArchiver_stat(archiver, fn, stat);
	}

	static void cueCloseArchive(void *opaque)
	{
		CueArchiver *archiver = (CueArchiver *)opaque;
		_CueArchiver_delete(archiver);
	}

	static PHYSFS_Archiver *createArchiver()
	{
		static PHYSFS_Archiver cueArchiver = {PHYSFS_API_VERSION,
		                                      {
		                                          "CUE", "Cuesheet-Backed Image File",
		                                          "Alexey Rogachevsky <sfalexrog@gmail.com>",
		                                          "https://github.com/sfalexeog",
		                                          0 // supportsSymlinks
		                                      },
		                                      cueOpenArchive,
		                                      cueEnumerateFiles,
		                                      cueOpenRead,
		                                      cueOpenWrite,
		                                      cueOpenAppend,
		                                      cueRemove,
		                                      cueMkdir,
		                                      cueStat,
		                                      cueCloseArchive};
		return &cueArchiver;
	}
// End of CueArchiver

void parseCueFile(char *fileName)
{
	CueParser *parser = CueParser_new(fileName);
	LogInfo("Parser status: %d", CueParser_isValid(parser));
	LogInfo("Data file: %s", CueParser_getDataFileName(parser));
	LogInfo("Track mode: %d", (int)CueParser_getTrackMode(parser));
	LogInfo("File mode: %d", (int)CueParser_getDataFileType(parser));
	CueParser_delete(parser);
}

PHYSFS_Archiver *getCueArchiver() { return createArchiver(); }
