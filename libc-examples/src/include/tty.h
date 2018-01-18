#ifndef __TTY_H__
#define __TTY_H__

#define CHAR_ESCAPE       0x1B
#define TERM_RAW          0x00
#define TERM_VT52         0x01
#define TERM_VT100        0x02
#define TERM_ANSI         0x03
#define TERM_AMSTRAD2     0x04
#define TERM_AMSTRAD3     0x05


void tty_reset();
char *get_ttytype(int);
void set_ttytype(enum tty_type t);
void set_reverse(bool);
void term_reset();
void clear_screen();
void set_cursor(int x, int y);
void show_cursor(bool state);



#define TTY_ATTR_RESET			"\x1B[0m"
#define TTY_ATTR_REVERSE_ON     "\x1B[7m"
#define TTY_ATTR_REVERSE_OFF    "\x1B[27m"
#define TTY_FOREGROUND_BLACK    "\x1B[30m"
#define TTY_FOREGROUND_RED      "\x1B[31m"
#define TTY_FOREGROUND_GREEN    "\x1B[32m"
#define TTY_FOREGROUND_YELLOW   "\x1B[33m"
#define TTY_FOREGROUND_BLUE     "\x1B[34m"
#define TTY_FOREGROUND_PURPLE   "\x1B[35m"
#define TTY_FOREGROUND_CYAN     "\x1B[36m"
#define TTY_FOREGROUND_WHITE    "\x1B[37m"
#define TTY_FOREGROUND_HALF_WHITE  "\x1B[2;37m"

#endif /* __TTY_H__ */

