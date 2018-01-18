#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <grx.h>
#include "cpm_sysfunc.h"


static uint8_t vid_dmabuffer[128];

/*
#define GRX_RETURN          0
#define GRX_SETPALETTE      1
#define GRX_DRAWBOX         2
#define GRX_FLUSH           3
#define GRX_SETPIXEL        4
#define GRX_RESETPALETTE    5
#define GRX_PIXSTREAM       6
#define GRX_SETVIDMODE      7
#define GRX_GETFONT         8
#define GRX_SETFONT         9
#define GRX_DRAWLINE        10 
*/

extern void llgrx(void);

void grx_resetpalette()
{
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_RESETPALETTE;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();

}

void grx_setpalette(uint8_t i, uint8_t r, uint8_t g, uint8_t b)
{
//    printf("grx_setpalette(%u, %u, %u, %u)\n", i, r, g, b);
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_SETPALETTE;
    vid_dmabuffer[1] = i;
    vid_dmabuffer[2] = r;
    vid_dmabuffer[3] = g;
    vid_dmabuffer[4] = b;
    //printf("DMA buffer is 0x%04x\n", &vid_dmabuffer);
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}

void grx_flush()
{
//    printf("grx_flush()\n");
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_FLUSH;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}


void grx_setpixel(uint8_t i, uint16_t x1, uint16_t y1)
{
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_SETPIXEL;
    vid_dmabuffer[1] = i;
    vid_dmabuffer[2] = x1 & 0x00ff;
    vid_dmabuffer[3] = (x1 & 0xff00) >> 8;
    vid_dmabuffer[4] = y1 & 0x00ff;
    vid_dmabuffer[5] = (y1 & 0xff00) >> 8;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
}

void grx_drawline(uint8_t i, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int j = 0;
    //printf("grx_drawline(%u, (%u,%u)-(%u,%u)\n", i, x1, y1, x2, y2);
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_DRAWLINE;
    vid_dmabuffer[1] = i; 
    vid_dmabuffer[2] = x1 & 0x00ff;
    vid_dmabuffer[3] = (x1 & 0xff00) >> 8;
    vid_dmabuffer[4] = y1 & 0x00ff;
    vid_dmabuffer[5] = (y1 & 0xff00) >> 8;
    vid_dmabuffer[6] = x2 & 0x00ff;
    vid_dmabuffer[7] = (x2 & 0xff00) >> 8;
    vid_dmabuffer[8] = y2 & 0x00ff;
    vid_dmabuffer[9] = (y2 & 0xff00) >> 8;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}


void grx_drawbox(uint8_t i, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int j = 0;
    //printf("grx_drawbox(%u, (%u,%u)-(%u,%u)\n", i, x1, y1, x2, y2);
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_DRAWBOX;
    vid_dmabuffer[1] = i;
    vid_dmabuffer[2] = x1 & 0x00ff;
    vid_dmabuffer[3] = (x1 & 0xff00) >> 8;
    vid_dmabuffer[4] = y1 & 0x00ff;
    vid_dmabuffer[5] = (y1 & 0xff00) >> 8;
    vid_dmabuffer[6] = x2 & 0x00ff;
    vid_dmabuffer[7] = (x2 & 0xff00) >> 8;
    vid_dmabuffer[8] = y2 & 0x00ff;
    vid_dmabuffer[9] = (y2 & 0xff00) >> 8;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}

void grx_pixstream(uint16_t x1, uint16_t y1, const char *buffer, uint16_t l)
{
    uint8_t limit = 120;
    uint16_t ptr = (uint16_t) buffer;


//    printf("grx_pixstream(%u, %u, 0x%04x, %u)\r\n", x1, y1, buffer, l);

    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_PIXSTREAM;
    vid_dmabuffer[1] = x1 & 0x00ff;
    vid_dmabuffer[2] = (x1 & 0xff00) >> 8;
    vid_dmabuffer[3] = y1 & 0x00ff;
    vid_dmabuffer[4] = (y1 & 0xff00) >> 8;
    vid_dmabuffer[5] = ptr & 0x00ff;
    vid_dmabuffer[6] = (ptr & 0xff00) >> 8;
    vid_dmabuffer[7] = l & 0x00ff;
    vid_dmabuffer[8] = (l & 0xff00) >> 8;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}

void grx_setvidmode(uint8_t mode)
{
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_SETVIDMODE;
    vid_dmabuffer[1] = mode;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}


void grx_getfont(uint8_t *buffer)
{
    uint16_t ptr = (uint16_t) buffer;
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_GETFONT;
    vid_dmabuffer[1] = ptr & 0x00ff;
    vid_dmabuffer[2] = (ptr & 0xff00) >> 8;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}

void grx_setfont(uint8_t *buffer)
{
    uint16_t ptr = (uint16_t) buffer;
    memset(&vid_dmabuffer, 0, 128);
    vid_dmabuffer[0] = GRX_SETFONT;
    vid_dmabuffer[1] = ptr & 0x00ff;
    vid_dmabuffer[2] = (ptr & 0xff00) >> 8;
    cpm_setDMAAddr((uint16_t) &vid_dmabuffer);
    llgrx();
    return;
}
