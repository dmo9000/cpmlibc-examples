#include <cprintf.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libc.h>
#include "detect.h"

extern int lldetect();
extern bool has_hydrogen;

int detect(char *payload)
{

    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    int rc = 0;
    rc = lldetect();
    if (rc == '!') {
        /* BANG! Hydrogen System Controller Detected */
        printf("Hydrogen System Controller detected.\n");
        has_hydrogen = true;
        } else {
        printf("Hydrogen System Controller NOT detected.\n");
        }
    
    return 0;
}

