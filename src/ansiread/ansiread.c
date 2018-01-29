#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

extern int errno;


#define CONSOLE_WIDTH		80
#define CONSOLE_HEIGHT	25

#define CHUNK_SIZE      128 
#define BUF_SIZE 		1024	
#define MAX_ANSI		64	/* maximum length allowed for an ANSI sequence */
#define MAX_PARAMS	16

static char filebuf[4096];
static char ansibuf[MAX_ANSI];
int parameters[MAX_PARAMS];

#define SEQ_ERR							0
#define SEQ_NONE						1
#define SEQ_ESC_1B					2
#define SEQ_ANSI_5B					3
#define SEQ_ANSI_IPARAM			4
#define SEQ_ANSI_CMD_M			5
#define SEQ_ANSI_CMD_J			6
#define SEQ_ANSI_CMD_A			7
#define SEQ_ANSI_CMD_B			8	
#define SEQ_ANSI_CMD_C			9	
#define SEQ_ANSI_CMD_D			10	
#define SEQ_ANSI_CMD_H			11	
#define SEQ_ANSI_EXECUTED		12	
#define SEQ_NOOP					 	13

static char *states[] = {
    "SEQ_ERR",
    "SEQ_NONE",
    "SEQ_ESC_1B",
    "SEQ_ANSI_5B",
    "SEQ_ANSI_IPARAM",
    "SEQ_ANSI_CMD_M",
    "SEQ_ANSI_CMD_J",
    "SEQ_ANSI_CMD_A",
    "SEQ_ANSI_CMD_B",
    "SEQ_ANSI_CMD_C",
    "SEQ_ANSI_CMD_D",
    "SEQ_ANSI_CMD_H",
    "SEQ_ANSI_EXECUTED",
    "SEQ_NOOP"
};

int saved_cursor_x = 0;
int saved_cursor_y = 0;

int cursor_x = 0;
int cursor_y = 0;

char ansi_mode = 0;
char last_ansi_mode = 0;
int ansioffset = 0;
int paramidx = 0;
off_t offset = 0;
off_t current_escape_address = 0;

int decode_1B(char);
int decode_5B(char);
int decode_command(char);
int decode_integer_parameter(char);

int ansi_decode_cmd_m();									/* text attributes, foreground and background color handling */
int ansi_decode_cmd_J();									/* "home" command (2J) */
int ansi_decode_cmd_A();									/* move down n lines */
int ansi_decode_cmd_B();									/* move down n lines */
int ansi_decode_cmd_D();									/* move left n columns */
int ansi_decode_cmd_C();									/* move right n columns */
int ansi_decode_cmd_H();									/* set cursor position */

void init_parameters();
const char *ansi_state(int s);

int main(int argc, char *argv[])
{
    FILE *ansfile = NULL;
    char *bufptr = NULL;
    char *scanptr = NULL;
    char *endptr = NULL;
    size_t elements_read = 0;
    size_t bytes_read = 0;
    char c = 0;
    char last_c = 0;
    int i = 0;

    if (argc < 2) {
        printf("usage: ansiread <filename.ans>\n");
        exit(1);
    }

    ansfile = fopen(argv[1], "rb");

    if (!ansfile) {
        printf("cannot open: %s: %s\n", argv[1], strerror(errno));
        exit(1);
    }

    bufptr = (char *) &filebuf;
    elements_read = fread((char *) bufptr, CHUNK_SIZE, 1, ansfile);
    bytes_read = (elements_read * CHUNK_SIZE);
    printf("[%d] bytes read to buffer\n", bytes_read);
    endptr = (char *) bufptr + (bytes_read);

    last_ansi_mode = SEQ_NONE;
    ansi_mode = SEQ_NONE;

    while (offset < bytes_read) {
        /* get next character from stream */
        scanptr = bufptr + offset;
        last_c = c;
        c = scanptr[0];
        switch(ansi_mode) {
        case SEQ_ERR:
            printf("--\n0x%04x: error in state, ansi_mode = [%s], last_ansi_mode = [%s], character = 0x%02X '%c'\n", current_escape_address,
                   (const char *) ansi_state(ansi_mode), (const char *) ansi_state(last_ansi_mode), last_c, (last_c < 32 ? '.' : last_c));
            printf("ANSIBUF[%d@0x%08x]: ", ansioffset, current_escape_address);
            for (i = 0; i < ansioffset; i++) {
                if (ansibuf[i] < 32) {
                    printf("\\x%02X", ansibuf[i]);
                } else {
                    printf("%c", ansibuf[i]);
                }
            }
            printf("\n");
            exit(1);
            break;
        case SEQ_NONE:
            /* record address of current escape sequence */
            last_ansi_mode = ansi_mode;
            ansi_mode = decode_1B(c);
            offset++;
            break;
        case SEQ_ESC_1B:
            last_ansi_mode = ansi_mode;
            ansi_mode = decode_5B(c);
            offset++;
            break;
        case SEQ_ANSI_5B:
            last_ansi_mode = ansi_mode;
            ansi_mode = decode_command(c);
            offset++;
            break;
        case SEQ_ANSI_IPARAM:
            last_ansi_mode = ansi_mode;
            ansi_mode = decode_integer_parameter(c);
            offset++;
            break;
        case SEQ_ANSI_EXECUTED:
				case SEQ_NOOP:
            printf("%08d-%08d: ", current_escape_address, 
								current_escape_address + ansioffset - 1);
            printf("%s: < ", ansi_state(ansi_mode));
            for (i = 0; i < ansioffset; i++) {
                if (ansibuf[i] < 32) {
                    printf("\\x%02X", ansibuf[i]);
                } else {
                    printf("%c", ansibuf[i]);
                }
            }
            printf(" >\n");
            last_ansi_mode = ansi_mode;
            ansi_mode = SEQ_NONE;
            break;
        default:
            printf("--\n0x%04x: error in state [UNHANDLED], ansi_mode = [%s], last_ansi_mode = [%s], character = 0x%02X\n", offset,
                   (const char*) ansi_state(ansi_mode), (const char *) ansi_state(last_ansi_mode), last_c);
            printf("ANSIBUF[0x%02X]: ", ansioffset);
            for (i = 0; i < ansioffset; i++) {
                if (ansibuf[i] < 32) {
                    printf("\\x%02X", ansibuf[i]);
                } else {
                    printf("%c", ansibuf[i]);
                }
            }
            printf("\n");
            exit(0);
            break;
        }
    }
    printf("\n");
    printf("[reached end of buffer]\n");
    fclose(ansfile);
    exit (0);
}

int decode_1B(char c)
{
    int i = 0;

    // printf("decode_1B(0x%02X, ansi_mode = %s, last_ansi_mode = %s)\n", c, ansi_state(ansi_mode), ansi_state(last_ansi_mode));
    if (ansi_mode == SEQ_NONE && c == 0x1B) {
        ansioffset = 0;
        memset(&ansibuf, 0, MAX_ANSI);
        ansibuf[ansioffset] = c;
        ansioffset++;
        init_parameters();
        return SEQ_ESC_1B;
    }

    return SEQ_NONE;
}

int decode_5B(char c)
{
    //printf("decode_5B(0x%02X, ansi_mode = %s, last_ansi_mode = %s)\n", c, ansi_state(ansi_mode), ansi_state(last_ansi_mode));
    if (ansi_mode == SEQ_ESC_1B && c == '[') {
        ansibuf[ansioffset] = c;
        ansioffset++;
        current_escape_address = offset;
        return SEQ_ANSI_5B;
    }

    return SEQ_ERR;
}

int decode_command(char c)
{
//    printf("decode_command(0x%02X, ansi_mode = %s, last_ansi_mode = %s)\n", c, ansi_state(ansi_mode), ansi_state(last_ansi_mode));
    if (ansi_mode == SEQ_ANSI_5B) {
        if (isdigit(c)) {
            if (parameters[paramidx] == -1) {
                parameters[paramidx] = (c - 48);
            } else {
                printf("decode_command: starting new integer parameter, but parameter[%d] was not -1\n", paramidx);
                return SEQ_ERR;
            }
            //     printf("decode_command: first digit = 0x%02X '%c'\n", c, c);
            ansibuf[ansioffset] = c;
            ansioffset++;
            return SEQ_ANSI_IPARAM;
        }

        /* process character commands */

        ansibuf[ansioffset] = c;
        ansioffset++;
        switch (c) {
        case 's':
            saved_cursor_x = cursor_x;
            saved_cursor_y = cursor_y;
            return SEQ_NONE;
            break;
        case 'u':
            cursor_x = saved_cursor_x;
            cursor_y = saved_cursor_y;
            return SEQ_NONE;
            break;
        case ';':
            printf("stored [%d:%d], new parameter -> %d\n", paramidx, parameters[paramidx], paramidx+1);
            paramidx++;
						 parameters[paramidx] = 0;
            return SEQ_ANSI_IPARAM;
            break;
        case 'A':
            return ansi_decode_cmd_A();
            break;
        case 'B':
            return ansi_decode_cmd_B();
            break;
        default:
            printf("+++ decoding command: character = 0x%02X '%c'\n", c, c);
            return SEQ_ERR;
            break;
        }


    }
    return SEQ_ERR;
}


int decode_integer_parameter(char c)
{
    if (ansi_mode != SEQ_ANSI_IPARAM) {
        return SEQ_ERR;
    }

    ansibuf[ansioffset] = c;
    ansioffset++;

    if (!isdigit(c)) {
        switch (c) {
        case 'H':
            /* end of previous command parameter, increment and dispatch 'H' command */
            paramidx++;
            ansi_mode = SEQ_ANSI_CMD_H;
            return ansi_decode_cmd_H();
            break;
        case ';':
            /* end of previous command parameter, go back to collect another parameter */
           //printf("decode_integer_parameter:   parameter[%d] = %d\n", paramidx, parameters[paramidx]);
            paramidx++;
            parameters[paramidx] = 0;
            return SEQ_ANSI_IPARAM;
            break;
        case 'A':
            /* end of previous command parameter, increment and dispatch 'B' command: move cursor up # lines */
            paramidx++;
            ansi_mode = SEQ_ANSI_CMD_B;
            return ansi_decode_cmd_A();
            break;
        case 'B':
            /* end of previous command parameter, increment and dispatch 'B' command: move cursor down # lines */
            paramidx++;
            ansi_mode = SEQ_ANSI_CMD_B;
            return ansi_decode_cmd_B();
            break;
        case 'C':
            /* end of previous command parameter, increment and dispatch 'C' command: move cursor right # columns */
            paramidx++;
            ansi_mode = SEQ_ANSI_CMD_C;
            return ansi_decode_cmd_C();
            break;
        case 'D':
            /* end of previous command parameter, increment and dispatch 'D' command: move cursor left # columns */
            paramidx++;
            ansi_mode = SEQ_ANSI_CMD_D;
            return ansi_decode_cmd_D();
            break;
        case 'J':
            /* end of previous command parameter, increment and dispatch 'J' command */
            paramidx++;
            ansi_mode = SEQ_ANSI_CMD_J;
            return ansi_decode_cmd_J();
        case 'm':
            /* end of previous command parameter, increment and dispatch 'm' command */
            paramidx++;
            ansi_mode = SEQ_ANSI_CMD_M;
            return ansi_decode_cmd_m();
        case 's':
            /* save position */
            saved_cursor_x = cursor_x;
            saved_cursor_y = cursor_y;
            ansi_mode = SEQ_NONE;
            break;
        case 'u':
            /* save position */
            cursor_x = saved_cursor_x;
            cursor_y = saved_cursor_y;
            ansi_mode = SEQ_NONE;
            break;
        default:
            printf("decode_integer_parameter:   non-digit = 0x%02X '%c'\n", c, c);
            return SEQ_ERR;
            break;
        };
    }

    // printf("decode_integer_parameter:  next digit[%d] = 0x%02X '%c'\n", paramidx, c, c);
    parameters[paramidx] *= 10;
    parameters[paramidx] += (c - 48);
    return SEQ_ANSI_IPARAM;

}

const char *ansi_state(int s)
{

    return (const char *) states[s];

}


void init_parameters()
{
    int i = 0;
    for (i = 0 ; i < MAX_PARAMS; i++) {
        parameters[i] = -1;
    }
    paramidx = 0;
}


int ansi_decode_cmd_m()
{
    int i = 0;
    //printf("ansi_decode_cmd_m(%d parameters)\n", paramidx);
    for (i = 0 ; i < paramidx; i++) {
        //   printf("parameter %d -> %d\n", i, parameters[i]);
        switch(parameters[i]) {
        case 0:
            /* reset */
            printf(">>> [%d] reset\n", i);
            break;
        case 1:
            /* bold on */
            printf(">>> [%d] enable bold\n", i);
            break;
        case 2:
            /* dark on */
            printf(">>> [%d] enable dark\n", i);
            break;
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
            printf(">>> [%d] set foreground color = %d\n", i, parameters[i] - 30);
            break;
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
            printf(">>> [%d] set background color = %d\n", i, parameters[i] - 40);
            break;
        default:
            printf("ansi_decode_cmd_m() - unknown parameter value = %d\n", parameters[i]);
            return SEQ_ERR;
            break;
        }

    }
    /* processed entirety of 'm' command */
    return SEQ_ANSI_EXECUTED;
}

int ansi_decode_cmd_J()
{
    //printf("ansi_decode_cmd_J(%d parameters)\n", paramidx);
    if (paramidx != 1) {
        printf("ansi_decode_cmd_J() - invalid parameter count = %d", paramidx);
        return SEQ_ERR;
    }
    /* check the parameter0 is a '2', ie. for '2J' */
    switch (parameters[0]) {
    case 2:
        //printf("> ANSI escape 2J\n");
        cursor_x = 0;
        cursor_y = 0;
        /* processed entirety of 'J' command */
        return SEQ_ANSI_EXECUTED;
        break;
    }
    printf("> ANSI escape [%d]J???\n", parameters[0]);
    return SEQ_ERR;

}

int ansi_decode_cmd_A()
{
    //printf("ansi_decode_cmd_A(%d parameters)\n", paramidx);
    //printf("> move cursor up %d lines\n", parameters[0]);

    if (parameters[0] == -1) {
        //printf("> move cursor up %d lines ; invalid parameter %d\n", parameters[0], parameters[0]);
        /* tricky one, not sure if it's legal, but it seems to be ignored anyway */
        return SEQ_NOOP;
    }

		if (parameters[0] < 1) {
					return SEQ_NOOP;
					}

    cursor_y -= parameters[0];
    if (cursor_y < 0) {
        cursor_y = 0;
    }
    /* processed entirety of 'A' command */
    return SEQ_ANSI_EXECUTED;
}

int ansi_decode_cmd_B()
{
    //printf("ansi_decode_cmd_B(%d parameters)\n", paramidx);
    //printf("> move cursor down %d lines\n", parameters[0]);

    if (parameters[0] == -1) {
        //printf("> move cursor down %d lines ; invalid parameter %d\n", parameters[0], parameters[0]);
        /* tricky one, not sure if it's legal, but it seems to be ignored anyway */
        return SEQ_NOOP;
    }

		if (parameters[0] < 1) {
					return SEQ_NOOP;
					}

    cursor_y += parameters[0];
    if (cursor_y > (CONSOLE_HEIGHT - 1)) {
        cursor_y = CONSOLE_HEIGHT - 1;
    }
    /* processed entirety of 'B' command */
    return SEQ_ANSI_EXECUTED;
}

int ansi_decode_cmd_D()
{
    //printf("ansi_decode_cmd_D(%d parameters)\n", paramidx);
    //printf("> move cursor left %d columns\n", parameters[0]);

    if (parameters[0] == -1) {
        printf("> move cursor left %d columns ; invalid parameter %d\n", parameters[0], parameters[0]);
        /* tricky one, not sure if it's legal, but it seems to be ignored anyway */
        return SEQ_NOOP;
    }

		if (parameters[0] < 1) {
					return SEQ_NOOP;
					}

    cursor_x -= parameters[0];
    if (cursor_x < 0) {
        cursor_x = 0;
    }
    /* processed entirety of 'B' command */
    return SEQ_ANSI_EXECUTED;
}

int ansi_decode_cmd_C()
{
    //printf("ansi_decode_cmd_C(%d parameters)\n", paramidx);
    //printf("> move cursor right %d columns\n", parameters[0]);

    if (parameters[0] == -1) {
        printf("> move cursor right %d columns ; invalid parameter %d\n", parameters[0], parameters[0]);
        /* tricky one, not sure if it's legal, but it seems to be ignored anyway */
        return SEQ_NOOP;
    }

		if (parameters[0] < 1) {
					return SEQ_NOOP;
					}

    cursor_x += parameters[0];
    if (cursor_x > CONSOLE_WIDTH - 1) {
        cursor_x = CONSOLE_WIDTH - 1;
    }
    /* processed entirety of 'B' command */
    return SEQ_ANSI_EXECUTED;
}



int ansi_decode_cmd_H()
{
    //printf("ansi_decode_cmd_H(%d parameters)\n", paramidx);
    if (paramidx < 2) {
        if (parameters[0] > 0) {
            cursor_y = parameters[0] - 1;
        }
        return SEQ_ANSI_EXECUTED;
    }

		/* SANITY CHECKING */

		if (parameters[0] == -1) {
//			printf("+++ fatal: setcursor received [;H, parameters[0] == -1\n");
//			printf("+++ parameter1 = %d\n", parameters[1]);
//			exit(1);
			parameters[0] = 0;
			}
    printf(">>> setcursor(%d,%d)\n", parameters[1], parameters[0]);

    if (parameters[1] > 0) {
        cursor_x = parameters[1] - 1;
    }

    if (parameters[0] > 0) {
        cursor_y = parameters[0] - 1;
    }
    return SEQ_ANSI_EXECUTED;
}
