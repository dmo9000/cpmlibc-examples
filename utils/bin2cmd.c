
/* Adds a CP/M-86 .CMD header to a headerless .COM-format binary file. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 128 bytes for header, plus 256 for the Zero Page */
unsigned char header[384];

#ifdef __PACIFIC__
#define AV0 "BIN2CMD"
#else
#define AV0 argv[0]
#endif

int main(int argc, char **argv)
{
	FILE *fpb, *fpc;
	long pos, paras, maxsize;
	int c;

	if (argc < 3)
	{
		fprintf(stderr, "Syntax: %s binfile cmdfile\n", AV0);
		return 1;
	}
	/* Open the input file, and seek to the end to get its size */
	fpb = fopen(argv[1], "rb");
	if (!fpb || (fseek(fpb, 0, SEEK_END) < 0))
	{
		perror(argv[1]);
		return 1;
	}
	/* Get size */
	pos = ftell(fpb);
	/* Seek back to the beginning */
	if (pos < 0 || fseek(fpb, 0, SEEK_SET) < 0)
	{
		perror(argv[1]);
		return 1;
	}

	/* Calculate size in paragraphs. Add 16 paragraphs for the 
	 * Zero Page. */
	paras = (pos + 15) / 16 + 0x10;

	/* The CP/M-86 CMD format does not allow groups larger than 1M */
	if (paras > 0xFFFF)
	{
		fclose(fpb);
		fprintf(stderr, "%s: Code group size would exceed 1MiB\n",
			argv[1]);
		exit(1);
	}
	/* If the file is smaller than 64k, request a 64k segment. If 
	 * it's bigger, request all the memory CP/M-86 is prepared to
	 * give. */
	if (paras < 0x1000) maxsize = 0x1000;
	else		    maxsize = 0xFFFF;
	memset(header, 0, sizeof(header));
	header[0] = 1;				/* Code group */
	header[1] = (paras & 0xFF);	
	header[2] = (paras >> 8) & 0xFF;	/* Group length */
	header[5] = (paras & 0xFF);	
	header[6] = (paras >> 8) & 0xFF;	/* Minimum size */
	header[7] = (maxsize & 0xFF);
	header[8] = (maxsize >> 8) & 0xFF;	/* Maximum size */

	/* Open output file */
	fpc = fopen(argv[2], "wb");
	if (!fpc)
	{
		perror(argv[2]);
		fclose(fpb);
		return 1;
	}
	/* Write header and Zero Page */
	if (fwrite(header, 1, sizeof(header), fpc) < sizeof(header))
	{
		perror(argv[2]);
		fclose(fpc);
		remove(argv[2]);
		fclose(fpb);
		return 1;
	}
	/* Copy data */
	while ( (c = fgetc(fpb)) != EOF)
	{
		if (fputc(c, fpc) == EOF)
		{
			perror(argv[2]);
			fclose(fpc);
			remove(argv[2]);
			fclose(fpb);
			return 1;
		}
	}
	if (fclose(fpc))
	{
		perror(argv[2]);
		remove(argv[2]);
		fclose(fpb);
		return 1;
	}
	fclose(fpb);
	return 0;
}
