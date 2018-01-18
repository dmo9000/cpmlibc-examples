#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_4MB_HD		4194304

int main(int argc, char *argv[])
{
	char buffer[128];
  FILE *diskimg = NULL;
  off_t addr = 0;

	if (argc < 2) {
		printf("usage: mkdsk <image-file>\n");
		exit(1);
	  }

  diskimg = fopen(argv[1], "wb");
 
  if (diskimg) {
			while (addr < 0x400000) {
			memset(&buffer, 0, 32);
			if (addr < 0x8000) {
				buffer[0] = 0xe5;
			  }
			fseek(diskimg, addr, SEEK_SET);
			if (fwrite(&buffer, 32, 1, diskimg) != 1) {
				perror("fwrite");
				exit(1);
				}

			addr += 32;
			}

			fclose(diskimg);
			}
  

	exit(0);
}
