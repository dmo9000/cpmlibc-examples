#include <cprintf.h>
#include <stdlib.h>
#include <string.h>
#include <libc.h>
#include "reset.h"

extern bool has_hydrogen;

int reset(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    cprintf("reset(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    __asm
        di
        nop 
        jp 0
        nop
        halt
    __endasm;
    while (1) { }

return 0;
}

