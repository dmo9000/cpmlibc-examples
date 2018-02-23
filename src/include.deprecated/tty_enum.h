#ifndef __TTY_ENUM_H__
#define __TTY_ENUM_H__


#include "tty.h"

typedef enum tty_type {
    TTY_RAW         = TERM_RAW,
    TTY_VT52        = TERM_VT52,
    TTY_VT100       = TERM_VT100,
    TTY_ANSI        = TERM_ANSI,
    TTY_AMSTRAD2    = TERM_AMSTRAD2,
    TTY_AMSTRAD3    = TERM_AMSTRAD3 
    } tty_type;

#endif /* __TTY_ENUM_H__ */
