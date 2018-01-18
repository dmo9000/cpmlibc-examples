struct _rip {
            uint16_t magic;
            uint16_t width;
            uint16_t height;
            uint16_t colors;
            };

struct _ripcolor {
            uint8_t i;
            uint8_t r;
            uint8_t g;
            uint8_t b;
            };

typedef struct _rip rip;
typedef struct _ripcolor ripcolor;

#define MAX_RIPCOLORS   256
