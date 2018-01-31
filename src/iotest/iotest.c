#include <stdio.h>
#ifdef __SDCC__
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ansi_term.h>
#endif /* __SDCC__ */
/* HiTech-C seems to be missing these */

#ifndef __SDCC__
typedef unsigned long uint32_t;
#define SEEK_SET  0 /* seek relative to beginning of file */
#define SEEK_CUR  1 /* seek relative to current file position */
#define SEEK_END  2 /* seek relative to end of file */
#endif /* __SDCC__ */

//#include "tty_enum.h"

int main(int argc, char *argv[])
{
    static char mybuf[128];
    uint32_t offset = 0;
    int rd = 0;
    int i = 0, j = 0;
    FILE *myfile = NULL;

    if (!argv[1]) {
        printf("no filename provided\n");
        exit(1);
    }

    set_term(TERM_VT100);
    clear_screen();

    myfile = fopen(argv[1], "rb");

    if (!myfile) {
        printf("error opening %s\n", argv[1]);
        perror("fopen");
        exit(1);
    }

    clear_screen();
    while (!feof(myfile) && !ferror(myfile)) {
        fseek(myfile, offset, SEEK_SET);
        set_cursor(0, 0);
        rd = fread(&mybuf, 128, 1, myfile);
        printf("offset = %lu, rd = %d\n", offset, rd);
        if (rd != 1) {
						/* completed */
            //printf("fread() didn't get 128 bytes\n");
            exit(1);
        }

        for (i = 0 ; i < 1; i++) {
            for (j = 0; j < 16; j++) {
                printf("%02x ", mybuf[(i*16) + j]);
                }
                printf("\n");
            }

        /* write(0, &mybuf, rd); */
        offset += (rd * 128);
    }

    printf("final offset = %lu\n", offset);
    fclose(myfile);

    exit(0);

}



