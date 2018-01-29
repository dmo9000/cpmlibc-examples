#include <cprintf.h>
#include <stdlib.h>
#include <string.h>
#include <libc.h>
#include "syslib/cpm_sysfunc.h"

int ttystress(char *payload)
{
    int i = 0, j = 0;


    __asm
        im 1
        ei
    __endasm;


    while (i < 0xffff) {
        cprintf("0x%04x", i);
        for (j = 0; j < 33 ; j++) {
            cprintf(".");
            }
        cprintf("\n");
        i++;
        }
    cprintf("\n");
    return 0;

    __asm
        di
    __endasm;

}

