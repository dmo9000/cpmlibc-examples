#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <ctype.h>
#include "cpmbdos.h"
#include "stdio.h"
#include "cpm_sysfunc.h"
#include "ansi_term.h"
//#include "tty_enum.h"
//#include "isr0.h"


/*
#define CHAR_ESCAPE             0x1B
#define CHAR_ESCAPE_CLS         0x48
#define CHAR_ESCAPE_SETCX       0x49
#define CHAR_ESCAPE_SETCY       0x4A
#define CHAR_ESCAPE_CURSOR      0x4B
*/

bool has_hydrogen = false;
bool has_cpm = false;
//extern enum tty_type TERM;

int cpm_major = 0;
int cpm_minor = 0;
#define MAX_STRING 80
#define READLINE_BUF_SIZE 80

char DFS = '|';

extern char *getargv(int d);
extern int ffdump (char *filename, bool paginate);

int
main (int argc, char *argv[])
{
    int i = 0;
    int c = 0;
    bool paginate = false;
    char *filename = NULL;
    int opterr = 0;
    char optopt = 0;

    if (argc <2) {
        printf("usage: fdump <filename>\n");
        exit (1);
    }

    while ((c = getopt (argc, argv, "P")) != -1) {
        switch (c)
        {
        case 'P':
            printf("-- pagination enabled--\n");
            paginate = true;
            break;
        case '?':
            if (optopt == 'c') {
                printf ("Option -%c requires an argument.\n", optopt);
            }
            else if (isprint (optopt)) {
                printf ("Unknown option `-%c'.\n", optopt);
                exit(1);
            }
            else {
                printf ("Unknown option character `\\x%x'.\n",
                        optopt);
                exit(1);
            }
            return 1;
        case -1:
            /* END OF FILE? */
            goto run_program;
            break;
        default:
            printf("exiting with c= %d [%c\n", c, c);
            exit (1);
        }

    }

run_program:
    //printf("optind = %d\n", optind);
    filename = argv[optind];
    ffdump((const char *) filename, paginate);

    return 0;
}

