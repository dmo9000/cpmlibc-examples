void grx_resetpalette();
void grx_setpalette(uint8_t, uint8_t, uint8_t, uint8_t);
void grx_drawline(uint8_t, uint16_t, uint16_t, uint16_t, uint16_t);
void grx_drawbox(uint8_t, uint16_t, uint16_t, uint16_t, uint16_t);
void grx_setpixel(uint8_t, uint16_t, uint16_t);
void grx_flush();
void grx_pixstream(uint16_t x1, uint16_t y1, unsigned char *buffer, uint16_t l);
void grx_setvidmode(uint8_t mode);
void grx_getfont(uint8_t *fontdata);
void grx_setfont(uint8_t *fontdata);

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

#define MODE_TEXTMODE       0
#define MODE_320x200x256    1
