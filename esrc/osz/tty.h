#define CHAR_ESCAPE       0x1B

#define TERM_RAW          0x00
#define TERM_VT52         0x01
#define TERM_VT100        0x02
#define TERM_ANSI         0x03
#define TERM_AMSTRAD2     0x04
#define TERM_AMSTRAD3     0x05


char *get_ttytype(int);
void set_reverse(bool);

