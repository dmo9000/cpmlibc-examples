#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "halt.h"

extern bool has_hydrogen;

int halt(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    printf("halt(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    __asm
        im 1
        ei
    __endasm;

while (1)
{


    __asm

    halt

    __endasm;
    printf(".");

}


    __asm
    di  
    __endasm;

return 0;
}

