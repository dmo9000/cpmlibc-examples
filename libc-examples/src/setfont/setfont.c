#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <grx.h>
#include <tty.h>
#include <ctype.h>
#include <getopt.h>

void shift_glyph(uint8_t c);

static uint8_t fontbuffer[256][8];
static uint8_t fontbuffer_cur[256][8];
static bool merged[256];
bool shift1 = false;
bool merge = false;

int main(int argc, char *argv[])
{

    FILE *font_in = NULL;
    int i = 0, j = 0;
    off_t offset = 0;
    int c = 0;
    int opterr = 0;
    char optopt = 0;

    uint8_t *ptr = (uint8_t *) &fontbuffer;
    uint8_t *ptr_cur = (uint8_t *) &fontbuffer_cur;

    if (argc < 2) {
        printf("usage: setfont <filename>\n");
        exit(1);
    }

    for (i = 0; i < 256; i++) {
        merged[i] = false;
    }

    while ((c = getopt (argc, argv, "SM")) != -1) {
        switch (c)
        {
        case 'S':
            printf("Shift+1 mode enabled.\n");
            shift1 = true;
            break;
        case 'M':
            printf("Merge mode enabled.\n");
            merge = true;
            break;
        case '?':
            if (optopt == 'c') {
                printf ("Option -%c requires an argument.\n", optopt);
                exit(1);
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
            printf("exiting with c= %d [%c]\n", c, c);
            exit (1);
        }

    }

run_program:
    printf("Running...\n");
    grx_getfont(ptr_cur);

    printf("Font retrieved from adapter...\n");

    printf("Loading file ... [%s]\n", argv[optind]);
    font_in = fopen(argv[optind], "rb");


    if (!font_in) {
        printf("error: %s\n", strerror(errno));
        exit(1);
    }


    if (font_in) {
        while (offset < 2048) {
            fseek(font_in, offset, SEEK_SET);
            //printf("reading offset %lu\n", offset);
            if (fread(ptr, 128, 1, font_in) != 1) {
                printf("error: %s\n", strerror(errno));
                exit(1);
            }
            ptr += 128;
            offset += 128;
        }

        fclose(font_in);
    }
    //  printf("%lu bytes read\n", offset);
    //
    //

    printf("Read data from file ...\n");

    if (merge) {
        for (j = 0; j < 255; j++) {
            uint16_t added = 0;
            for (i = 0 ; i < 8; i++) {
                added += fontbuffer[j][i];
            }
            if (!added) {
                printf("Skipping blank glyph %u\r", j);
                for (i = 0 ; i < 8; i++) {
                    fontbuffer[j][i] = fontbuffer_cur[j][i];
                }
            } else {
                merged[j] = true;
            }
        }
    }


    for (j = 0; j < 255; j++) {
        if (merged[j] && shift1) {
            shift_glyph(j);
        }
    }

    printf("\n");
    printf("Writing font to adapter\n");

    grx_setfont((void *) &fontbuffer);

    exit(0);
}

void shift_glyph(uint8_t c)
{
    int i = 0;
    for (i = 1; i < 8; i++) {
        fontbuffer[c][i-1] = fontbuffer[c][i];
    }
    fontbuffer[c][7] = 0;

}
