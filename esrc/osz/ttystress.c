#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syslib/cpm_sysfunc.h"

int ttystress(char *payload)
{
    int i = 0, j = 0;


	/*
    __asm
        im 1
        ei
    __endasm;
	*/


		payload = NULL;
    while (i < 0xffff) {
        printf("0x%04x", i);
        for (j = 0; j < 73 ; j++) {
            printf(".");
            }
        printf("\n");
        i++;
        }
    printf("\n");
    return 0;

}

