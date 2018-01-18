#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isr0.h"

extern bool has_hydrogen;

int isr0(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    printf("isr0(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    __asm
        push    ix
        ld      ix,#0x00
        add     ix,sp
        call    0x0038 
    __endasm;

return 0;
}

