#include <cprintf.h>
#include <stdlib.h>
#include <string.h>
#include <libc.h>
#include "syslib/cpm_sysfunc.h"
#include "dump.h"

extern char DFS;
extern bool has_hydrogen;

int dump(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    unsigned char *ptr = NULL;
    //cprintf("dump(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }
    if (addr > 0xffff) {
        cprintf("dump: address out of range\n");
        return -1;
    }

    while (lines < 23 && addr <= 0xfff0) {
        cprintf("0x%04x %c ", addr, DFS);
        for (i = 0 ; i < 8; i++) {
            ptr = addr + i;
            cprintf("%02x", ptr[0]);
            ptr++;
        }
        ptr -= 8;
        cprintf(" %c ", DFS);

        for (i = 0 ; i < 8; i++) {
            ptr = addr + i;
            if (ptr[0] >= 32) {
                //cprintf("%c", ptr[0]);
                cpm_putchar(ptr[0]);
            } else {
                cprintf(".");
            }
            ptr++;
        }
        /* line warp */
        cprintf("\n");

        addr += 8;
        lines++;

        if (lines >= 23) {
            if (continue_or_exit()) {
                lines = 0;
                printf("\n");
            }
        }
    }


    cprintf("\n");

    return 0;
}

