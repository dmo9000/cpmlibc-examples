#include <unistd.h>
#include <stdio.h>
#include <ansi_term.h>

//extern enum tty_type TERM; 


void clear_screen();

int main()
{

    /* only VT100/ANSI support for now, sorry! */
//		TERM=TTY_VT100;
    /* reset */

    set_term(TTY_VT100);

    term_reset();
    clear_screen();

    return 0;
}


