#include <cprintf.h>
#include "clkchk.h"

int clkchk(char *payload)
{

    
    unsigned int clk = clock();
    cprintf("clkchk() = %u\n", clk);
    __asm

    ei 
    nop
    nop
    nop
    di

    __endasm;
    return 0;
}

