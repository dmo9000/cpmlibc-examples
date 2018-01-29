#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_RECORDS 6004
#define SIZE_RECORD 128

int main()
{
	FILE *testfile = NULL;
	char record_data[128];
	int i = 0, j = 0;
	uint8_t e = 0;
	uint8_t m = 0;
	uint8_t c = 0;
	uint32_t written = 0;
    uint16_t rrec = 0;

	testfile = fopen("testfile.bin", "wb");

	while (rrec < MAX_RECORDS) {
		//printf("m = 0x%02x, m(shift) = 0x%02x, e = 0x%02x, c = 0x%02x\n", m, m << 5, e, c);
		for (i = 0 ; i< SIZE_RECORD; i+=2) {
			record_data[i] =(uint8_t) (((uint16_t) rrec & 0xff00) >> 8); 
            record_data[i+1] = (uint8_t) (rrec & 0x00ff); 
			}
			if (fwrite(&record_data, SIZE_RECORD, 1, testfile) != 1) {
					perror("write");
					fclose(testfile);
					exit(1);
					}
        rrec ++;
		}

	fclose(testfile);

}
