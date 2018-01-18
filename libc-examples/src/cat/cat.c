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

bool has_hydrogen = false;
bool has_cpm = false;
//extern enum tty_type TERM;

int cpm_major = 0;
int cpm_minor = 0;
#define MAX_STRING 80
#define READLINE_BUF_SIZE 80

char DFS = '|';

extern char *getargv(int d);
extern int fcat (char *filename, off_t start_offset, off_t end_offset);
static off_t start_offset = 0;
static off_t end_offset = 0xffff;

int
main (int argc, char *argv[])
{
    int i = 0;
		int c = 0;
		char optopt = 0;
    
		if (argc <2) {
			printf("usage: cat [-Sn][-En] <filename>\n");
			exit (1);
			}

   while ((c = getopt (argc, argv, "S:E:")) != -1) {
        switch (c)
        {
        case 'S':
			start_offset = atoi(optarg);
            break;
        case 'E':
			end_offset = atoi(optarg);
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
				case 0:
						goto run_program;
						break;
        default:
            printf("exiting with c= %d [%c\n", c, c);
            exit (1);
        }

    }

run_program:
	  printf("fcat -> %s, %lu -> %lu\n", argv[optind], start_offset, end_offset);
    fcat((const char *) argv[optind], start_offset, end_offset);

	return 0;
}

