#include <stdio.h>
#include <string.h>
#include "clkchk.h"

extern unsigned int clock();


int clkchk(char *payload)
{

    unsigned int clk = clock();
    printf("clkchk() = 0x%04x (%u)\n", clk, clk);
		payload = NULL;
    return 0;
}

