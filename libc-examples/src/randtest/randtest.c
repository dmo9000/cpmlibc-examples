#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <grx.h>


int main(int argc, char *argv[])
{
    uint32_t seed = 0;
    int i = 0;
    uint8_t c = 0;
    uint8_t r = 0, g = 0, b = 0; 
    uint16_t x1 = 0, y1 = 0;
    uint16_t x2 = 0, y2 = 0;
    uint16_t x3 = 0, y3 = 0;

    if (argc > 1) {
        seed = strtoul(argv[1], NULL, 10); 
        } else {
        seed = 0;
        }

    srandom(seed);
    
    grx_setvidmode(MODE_320x200x256);
    for (i = 0 ; i < 255; i++) {
        r = (uint8_t) random() % 255;
        g = (uint8_t) random() % 255;
        b = (uint8_t) random() % 255;
        grx_setpalette(i, r, g, b);
        }

    for (i = 0; i < 65536; i++) {
        x1 = (uint16_t) random() % 320;
        y1 = (uint16_t) random() % 200;
        x2 = (uint16_t) random() % 320;
        y2 = (uint16_t) random() % 200;

        if (x2 < x1) {
            x3 = x2; 
            x2 = x1;
            x1 = x3; 
            }    

        if (y2 < y1) {
            y3 = y2; 
            y2 = y1;
            y1 = y3; 
            }    

        c = (uint8_t) random() % 255;
        grx_drawbox(c, x1, y1, x2, y2);
        grx_flush();
        }

    grx_resetpalette();
    grx_setvidmode(MODE_TEXTMODE);

    exit(0);
}
