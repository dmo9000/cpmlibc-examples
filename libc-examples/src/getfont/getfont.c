#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <grx.h>

static uint8_t fontbuffer[8*256];
int main(int argc, char *argv[])
{

    FILE *font_out = NULL;
    off_t offset = 0;
    uint8_t *ptr = (uint8_t *) &fontbuffer;


    if (argc < 2) {
        printf("usage: getfont <filename>\n");
        exit(1);
    }

    grx_getfont((void *) &fontbuffer);

    font_out = fopen(argv[1], "wb");

    if (font_out) {
        while (offset < 2048) {
            fseek(font_out, offset, SEEK_SET);
            // printf("writing offset %lu\n", offset);
            if (fwrite(ptr, 128, 1, font_out) != 1) {
                printf("error: %s\n", strerror(errno));
                exit(1);
            }
            ptr += 128;
            offset += 128;
        }
    }
    fclose(font_out);
    // printf("%lu bytes written\n", offset);

    exit(0);
}
