#include <stdio.h>

/* HiTech-C seems to be missing these */

typedef unsigned long uint32_t;
#define SEEK_SET  0 /* seek relative to beginning of file */
#define SEEK_CUR  1 /* seek relative to current file position */
#define SEEK_END  2 /* seek relative to end of file */


int main(int argc, char *argv[])
{
  static char mybuf[128];
	uint32_t offset = 0;
	int rd = 0;
  FILE *myfile = NULL;

	if (!argv[1]) {
		printf("no filename provided\n");
		exit(1);
		} 

	myfile = fopen(argv[1], "rb");

	if (!myfile) {
		printf("error opening %s\n", argv[1]);
		exit(1);
		}

	while (!feof(myfile) && !ferror(myfile)) {
		fseek(myfile, offset, SEEK_SET); 
		rd = fread(&mybuf, 1, 128, myfile);
		/* printf("offset = %lu, rd = %d\n", offset, rd); */
		write(0, &mybuf, rd);
		offset += rd;	
		}

	/* printf("final offset = %lu\n", offset); */
	fclose(myfile);

	exit(0);

}



