#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "syslib/cpm_sysfunc.h"
//#include "tty_enum.h"
#include "tty.h"
#include "box.h"


#define CHAR_ESCAPE				0x1B
#define CHAR_ESCAPE_CLS			0x48
#define CHAR_ESCAPE_SETCX		0x49
#define CHAR_ESCAPE_SETCY		0x4A
#define CHAR_ESCAPE_CURSOR		0x4B

#define CHAR_ANSICSI            0x5B

#define false 0
#define true 1

int si = 0;
const char nineslice[3][9] = {
                                { 0xda, 0xc4, 0xbf, 0xb3, 0x20, 0xb3, 0xc0, 0xc4, 0xd9 },
                                { 0x2b, 0x2d, 0x2b, 0x7c, 0x20, 0x7c, 0x2b, 0x2d, 0x2b },
                                { 0x96, 0x9a, 0x9c, 0x95, 0x20, 0x95, 0x93, 0x9a, 0x99 }
                             };
                     
extern bool has_amstrad;
extern bool has_hydrogen;
//extern enum tty_type TERM;
extern void clear_screen();


void setcursor(int x, int y)
{

   switch (get_term()) {
        case TERM_AMSTRAD3:
        case TERM_VT52:
            printf("%cY%c%c", CHAR_ESCAPE, y+32, x+32); 
            break;
        case TERM_ANSI:
        case TERM_VT100:
            printf("%c%c%u;%uH", CHAR_ESCAPE, CHAR_ANSICSI, y+1, x+1); 
            break;
        default:
            break;
        };
}

void showcursor(bool state) 
{
    switch (state) {
        case false:
            printf("%c%c%c", CHAR_ESCAPE, CHAR_ESCAPE_CURSOR, 1);
            break;
        case true:
            printf("%c%c%c", CHAR_ESCAPE, CHAR_ESCAPE_CURSOR, 2);
            break;
        }

}

void drawbox(int x1, int y1, int x2, int y2)
{

    int i = 0;

    /* top span */
    setcursor(x1, y1);
    cpm_putchar(nineslice[si][0]);
    for (i = x1+1 ; i <= x2 -1 ; i++) {
        cpm_putchar(nineslice[si][1]);
        }
    cpm_putchar(nineslice[si][2]);

    /* vertical spans */

    for (i = y1 + 1 ; i <= y2 - 1; i++) {
        setcursor(x1, i);
        cpm_putchar(nineslice[si][3]);
        setcursor(x2, i);
        cpm_putchar(nineslice[si][5]);
        }

    /* bottom span */

    setcursor(x1, y2);
    cpm_putchar(nineslice[si][6]);
    for (i = x1+1 ; i <= x2 -1 ; i++) {
        cpm_putchar(nineslice[si][7]);
        }
    cpm_putchar(nineslice[si][8]);


}

int box(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
//    printf("box(%s)\n", payload);
    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

    /* CLS */

    if (!has_hydrogen) {
        si = 1;
        }

    if (get_term() == TERM_AMSTRAD2 || get_term() == TERM_AMSTRAD3) {
        si = 2;
        }

//    showcursor(false);
    clear_screen();
    drawbox(0, 0, 79, 22);
    setcursor(1,1);
    setcursor(1,1);
//    showcursor(true);

    

return 0;
}

