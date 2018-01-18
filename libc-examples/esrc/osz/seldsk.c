#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "seldsk.h"

extern bool has_hydrogen;

#define MAX_DISKS 16

int seldsk(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    if (addr >=0 && addr< MAX_DISKS) {
            printf("seldsk: disk id #%u selected\n", addr);
        } else {
            printf("seldsk: disk id #%u is out of range\n", addr);
        }

return 0;
}

