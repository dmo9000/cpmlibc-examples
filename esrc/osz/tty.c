#include <stdbool.h>
#include <stdio.h>
#include "tty.h"

//extern enum tty_type TERM;

char *get_ttytype(int id)
{
    switch (id) {
    case 0:
        return "TTY_RAW";
        break;
    case 1:
        return "TTY_VT52";
        break;
    case 2:
        return "TTY_VT100";
        break;
    case 3:
        return "TTY_ANSI";
        break;
    case 4:
        return "TTY_AMSTRAD2";
        break;
    case 5:
        return "TTY_AMSTRAD3";
        break;
    }
    return "TTY_UNKNOWN";
}

