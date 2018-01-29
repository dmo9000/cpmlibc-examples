#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "testllio.h"

extern bool has_hydrogen;

int testllio(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    printf("testllio(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    printf("Starting test ...\n");
    __asm

 push af
 push bc
 push de
 push hl
 push ix
 push iy
 exx
 ex af, af'
 push af
 push bc
 push de
 push hl

 ; test routine - should wait for input 
input_loop:

 in a, (#0x00)
 and #0x02
 jp z, input_loop 

 ; test routine - CONOUT - should print two lines with a single at character 

 ld a,#64 
 out (#0x01), a
 ld a,#13 
 out (#0x01), a
 ld a,#10 
 out (#0x01), a

 ; SELDSK - select disk 0

 ld a,#00
 out (#0x0a), a

 ; SELTRK - select track 0

 ld a,#00
 out (#0x0b),a
 
 ; SETSEC - select sector 1 

 ld a,#01
 out (#0x0c), a

 ; SETDMA - set address 0x8000
 ld a,#0x00
 out (#0x0f),a
 ld a,#0x80
 out (#0x10),a
 
 ; READ - read boot sector of disk (track 0, sector 0)
 ld a,#00
 out (#0x0d),a 
 in a, (#0x0e)
 ld a,#00

 ; end test routine
 pop hl
 pop de
 pop bc
 pop af
 ex af, af'
 exx
 pop iy
 pop ix
 pop hl
 pop de
 pop bc
 pop af

    __endasm;

    printf("Ending test ...\n");
return 0;
}

