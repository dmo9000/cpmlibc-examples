#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <grx.h>
#include <tty.h>
#include <cpm_sysfunc.h>
#include "rip.h"

#define MAX_COLORS 256

static rip MyRip;
static ripcolor MyRipColors[MAX_COLORS];
extern enum tty_type TERM;
static uint8_t buffer[128];

int main(int argc, char *argv[])
{
    FILE *ripfile = NULL;
    int i = 0;
    int index = 0;
    int x = 0, y = 0;
	off_t remain = 0;
	off_t dsize = 0;
	uint8_t pal = 0;
	int blocks = 0;
    off_t bytes_read = 0;
    int remain_in_block = 128;
    off_t offset = 0;
    uint16_t nremain = 0;
    

    TERM=TERM_VT100;
    clear_screen();

    if (argc == 2) {
        printf("opening ripfile [%s]\n", (const char *) argv[1]);
        ripfile = fopen(argv[1], "rb");
    }

    if (!ripfile) {
        printf("Couldn't open %s\n", argv[1]);
        exit(1);
    }

    printf("reading header\n");
    fread(&MyRip, sizeof(rip), 1, ripfile);
    printf("width = %u, height = %u, colors = %u\n", MyRip.width, MyRip.height, MyRip.colors);
    printf("closing ripfile\n");

    for (i = 0; i < MyRip.colors; i++) {
//        printf("[%u:%u]", i, sizeof(rip)+ (i*sizeof(ripcolor)));
        fseek(ripfile, sizeof(rip)+ (i*sizeof(ripcolor)), SEEK_SET);
        fread(&MyRipColors[i], sizeof(ripcolor), 1, ripfile);
        //printf("%u: %u, %u, %u\n", i, MyRipColors[i].r, MyRipColors[i].g, MyRipColors[i].b);
    }


    printf("%u colors read\n", MyRip.colors);
    /*	
    grx_flush();
        index = 0;
        for (y = 0 ; y < 16; y++) {
            for (x = 0 ; x < 16; x++) {
                grx_drawbox(index, x*8, y*8, (x+1) *8, (y+1) * 8);
                index ++;
            }
        }
        grx_flush();
    */
		
        offset = ftell(ripfile);
		remain = (MyRip.width * MyRip.height);
		dsize = remain;

		printf("width = %u\n", MyRip.width);
		printf("height = %u\n", MyRip.height);
		printf("offset = %lu, remain = %u\n", offset, remain);

		x = 0; 
		y = 0;

        /*
        while (cpm_getchar_noecho() == 0) {
            // FIXME: busy waiting 
            }
        */

        show_cursor(false);

        grx_setvidmode(MODE_320x200x256);
    for (i = 0; i < MyRip.colors; i++) {
        grx_setpalette(i, MyRipColors[i].r, MyRipColors[i].g, MyRipColors[i].b);
    }


        fseek(ripfile, offset, SEEK_SET);
        offset = ftell(ripfile);

        remain_in_block = 128 - (offset % 128); 

		while (remain > 0) {
			fseek(ripfile, offset, SEEK_SET);
            offset = ftell(ripfile);
    		bytes_read = fread(&buffer, remain_in_block, 1, ripfile);

            //printf("fread(%lu, %u) = %u\n", offset, remain_in_block, bytes_read);
            if (bytes_read != 1) {
                    //printf("bad fread: %lu %u %u\n", offset, bytes_read, remain_in_block);
                    goto finish_up;
                    }

			remain -= remain_in_block;

            if (remain > 0) {

                nremain = (uint16_t) remain_in_block;
                grx_pixstream(x, y, (const char *) &buffer, nremain);
			for (i = 0; i < remain_in_block; i++) {
			//		pal = buffer[i];
			//		grx_setpixel(pal, x, y);
					x++;
					if (x >= MyRip.width) {
						x = 0;
						y++;
                        grx_flush();
						}
					}
		    offset += remain_in_block;
            remain_in_block = 128 - (offset % 128); 
                }
			} 

finish_up:
		grx_flush();
    fclose(ripfile);


    while (cpm_getchar_noecho() == 0) {
            /* FIXME: busy waiting */
            }

    grx_setvidmode(MODE_TEXTMODE);
    show_cursor(true);


    exit(0);
}
