#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "syslib/cpm_sysfunc.h"
#include "dump.h"

extern char DFS;
extern bool has_hydrogen;
extern bool continue_or_exit ();


int dump(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    unsigned char *ptr = NULL;
    //printf("dump(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }
    if (addr > 0xfff0) {
        printf("dump: address out of range\n");
        return -1;
    }

    while (lines < 23 && addr <= 0xfff0) {
        printf("0x%04x %c ", addr, DFS);
        for (i = 0 ; i < 16; i++) {
            ptr = (char *) addr + i;
            printf("%02x", ptr[0]);
            ptr++;
        }
        ptr -= 16;
        printf(" %c ", DFS);

        for (i = 0 ; i < 16; i++) {
            ptr = (char *) addr + i;
            if (ptr[0] >= 32 && ptr[0] <= 127) {
                //printf("%c", ptr[0]);
                cpm_putchar(ptr[0]);
            } else {
                printf(".");
            }
            ptr++;
        }
        /* line warp */
        printf("\n");

        addr += 16;
        lines++;

        if (lines >= 23) {
            if (continue_or_exit()) {
                lines = 0;
                printf("\n");
            }
        }
    }


    printf("\n");

    return 0;
}

