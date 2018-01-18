#include <cprintf.h>
#include "modules.h"

extern char DFS;
#define NULL 0

int modules(char *payload)
{
    int jumpIndex = 0;
    jmpTable *cmdptr = NULL;    

    cprintf("\n");
    cmdptr = &jmptbl[jumpIndex];
    while (cmdptr->cmdptr) {
        cprintf("%16s  %c  0x%04x\n", cmdptr->command, DFS, cmdptr->cmdptr);
        jumpIndex++; 
        cmdptr = &jmptbl[jumpIndex];
        }
    cprintf("\n");

    return 0;
}

