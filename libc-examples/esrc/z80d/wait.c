#include <cprintf.h>
#include <stdlib.h>
#include <string.h>
#include <libc.h>
#include "wait.h"

extern bool has_hydrogen;

int wait(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    cprintf("wait(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    __asm
        im 1
        ei
        halt
        di  
    __endasm;

return 0;
}

