#include <cprintf.h>
#include <stdlib.h>
#include <string.h>
#include <libc.h>
#include "syslib/cpm_sysfunc.h"
#include "box.h"

extern bool has_hydrogen;

#define CHAR_ESCAPE				0x1B
#define CHAR_ESCAPE_CLS			0x48
#define CHAR_ESCAPE_SETCX		0x49
#define CHAR_ESCAPE_SETCY		0x4A
#define CHAR_ESCAPE_CURSOR		0x4B

#define CHAR_ANSICSI            0x5B

#define false 0
#define true 1

/*
void clear_screen()
{
    cprintf("%c%c", CHAR_ESCAPE, CHAR_ESCAPE_CLS);
}
*/


void setcursor(int x, int y)
{

   cprintf("%c%c%u;%uH", CHAR_ESCAPE, CHAR_ANSICSI, x+1, y+1); 

}

void showcursor(bool state) 
{
    switch (state) {
        case false:
            cprintf("%c%c%c", CHAR_ESCAPE, CHAR_ESCAPE_CURSOR, 1);
            break;
        case true:
            cprintf("%c%c%c", CHAR_ESCAPE, CHAR_ESCAPE_CURSOR, 2);
            break;
        }

}

void drawbox(int x1, int y1, int x2, int y2)
{

    int i = 0;

    /* top span */
    setcursor(x1, y1);
    cpm_putchar(0xda);
    for (i = x1+1 ; i <= x2 -1 ; i++) {
        cpm_putchar(0xc4);
        }
    cpm_putchar(0xbf);

    /* vertical spans */

    for (i = y1 + 1 ; i <= y2 - 1; i++) {
        setcursor(x1, i);
        cpm_putchar(0xb3);
        setcursor(x2, i);
        cpm_putchar(0xb3);
        }

    /* bottom span */

    setcursor(x1, y2);
    cpm_putchar(0xc0);
    for (i = x1+1 ; i <= x2 -1 ; i++) {
        cpm_putchar(0xc4);
        }
    cpm_putchar(0xd9);


}

int box(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
//    cprintf("box(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    /* CLS */

//    showcursor(false);
//    clear_screen();
    drawbox(0, 0, 39, 23);
    setcursor(1,1);
//    showcursor(true);

    

return 0;
}

