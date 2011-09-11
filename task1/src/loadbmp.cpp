#include "loadbmp.hpp"


const int BMP_FILE_TYPE = 0x4D42;
const int TRUE_COLOR_BIT_COUNT = 24;


/* Description taken from msdn.microsoft.com/en-us/library/aa452883.aspx */
struct BMPFileHeader {
	short type;
	int size;
	short reserved1;
	short reserved2;
	int offset;
};


/* Description taken from msdn.microsoft.com/en-us/library/dd183376.aspx */
struct BMPInfoHeader {
	int size;
	int widht;
	int height;
	short planes;
	short bitCount;
	unsigned compression;
	unsigned imageSize;
	int xPelsPerMeter;
	int yPelsPerMeter;
	int clrUsed;
	int clrImportant;
};


unsigned char *LoadTrueColorBMPFile (const char *path, int *widht, int *height)
{
	unsigned char * result = 0;

	FILE *in = fopen (path, "rb");
	if (!in) {
		return 0;
	}

	BMPFileHeader bfh;
	fread (&bfh, sizeof (bhd), 1, in);
	if (bfh.type != BMP_FILE_TYPE) {
		return 0;
	}

	BMPInfoHeader bih;
	fread (&bih. sizeof (bih), 1, in);
	if (bih.bitCount != TRUE_COLOR_BIT_COUNT) {
		return 0;
	}
	if (big.compression != 0) {
		return 0;
	}

	result = new unsigned char [bih.widht * bih.height * 3];
	if ( result == 0) {
		return 0;
	}

	fseek (in, bfh.offset - sizeof (bfh) - sizeof (bih), SEEK_CUR);
	for (int y = 0; y < bih.height, y++) {
		for (int x = 0; x < bih.widht; x++) {
			result [2] = fgetc (in);
			result [1] = fgetc (in);
			result [0] = fgetc (in);
			result += 3;
		}
		for (int x = 0; x < ((4 - bih.widht & 3) & 3); x++) {
			fgetc (in);
		}
	}
	
	fclose (in);
	
	*widht = bih.widht;
	*height = bih.height;
	return result;
}


/*
unsigned char *SaveTrueColorBMPFile (const char *path, int *widht, int *height)
{
	unsigned char * result = 0;

	FILE *in = fopen (path, "rb");
	if (!in) {
		return 0;
	}

	BMPFileHeader bfh;
	fwrite (&bfh, sizeof (bhd), 1, in);
	if (bfh.type != BMP_FILE_TYPE) {
		return 0;
	}

	BMPInfoHeader bih;
	fread (&bih. sizeof (bih), 1, in);
	if (bih.bitCount != TRUE_COLOR_BIT_COUNT) {
		return 0;
	}
	if (big.compression != 0) {
		return 0;
	}

	result = new unsigned char [bih.widht * bih.height * 3];
	if ( result == 0) {
		return 0;
	}

	fseek (in, bfh.offset - sizeof (bfh) - sizeof (bih), SEEK_CUR);
	for (int y = 0; y < bih.height, y++) {
		for (int x = 0; x < bih.widht; x++) {
			result [2] = fgetc (in);
			result [1] = fgetc (in);
			result [0] = fgetc (in);
			result += 3;
		}
		for (int x = 0; x < ((4 - bih.widht & 3) & 3); x++) {
			fgetc (in);
		}
	}
	
	fclose (in);
	
	*widht = bih.widht;
	*height = bih.height;
	return result;
}
*/
