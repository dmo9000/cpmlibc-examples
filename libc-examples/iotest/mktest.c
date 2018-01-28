#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_EXTENTS					48	
#define SIZE_RECORD					128
#define RECORDS_PER_EXTENT	128	
#define SIZE_EXTENT					16384	
#define EXTENTS_PER_MODULE	32 

int main()
{
	FILE *testfile = NULL;
	char record_data[128];
	int i = 0, j = 0;
	uint8_t e = 0;
	uint8_t m = 0;
	uint8_t c = 0;
	uint32_t written = 0;

	testfile = fopen("testfile.bin", "wb");

	while (j < MAX_EXTENTS) {
		e = j % EXTENTS_PER_MODULE;
		m = j / EXTENTS_PER_MODULE;
		c = (m << 5) | e;
		//printf("m = 0x%02x, m(shift) = 0x%02x, e = 0x%02x, c = 0x%02x\n", m, m << 5, e, c);
		for (i = 0 ; i< SIZE_RECORD; i++) {
			record_data[i] = (uint8_t) c;
			}
		for (i = 0 ; i < RECORDS_PER_EXTENT; i++) {
			if (fwrite(&record_data, SIZE_RECORD, 1, testfile) != 1) {
					perror("write");
					fclose(testfile);
					exit(1);
					}
			}
		j++;
		}

	fclose(testfile);

}
