#include <stdio.h>
#include "modules.h"

extern char DFS;
#define NULL 0

int modules(char *payload)
{
    int jumpIndex = 0;
    jmpTable *cmdptr = NULL;    

    printf("\n");
    cmdptr = (jmpTable*) &jmptbl[jumpIndex];
    while (cmdptr->cmdptr) {
        printf("%16s  %c  0x%04x\n", cmdptr->command, DFS, cmdptr->cmdptr);
        jumpIndex++; 
        cmdptr = (jmpTable *) &jmptbl[jumpIndex];
        }
    printf("\n");

    payload = NULL;
    return 0;
}

