#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cpm_sysfunc.h>
#include "tty.h"

uint16_t param0 = 0xff;

uint16_t cpm_test (uint16_t t);
void cpm_settrk (uint16_t t);
void cpm_setsec (uint16_t s);
void cpm_writesec ();

void
usage ()
{
    printf ("usage: format [ A: | B: | I: | J: ]\n");
    exit (1);
}

int
main (int argc, char *argv[])
{
    static unsigned char buffer[128];
    uint16_t r = 0;
    int sectors = 0, tracks = 0, seclen = 0;
    int s = 0, t = 0;
    int drv = 0;
    uint32_t offset = 0;
    int j = 0;
    int fat_tracks = 0;

    if (argc != 2)
    {
        usage ();
    }

    /* CP/M changes command tail to all capitals */

    if (argv[1][0] != 'A' && argv[1][0] != 'B' && argv[1][0] != 'I'
            && argv[1][0] != 'J')
    {
        usage ();
    }

    if (argv[1][1] != ':')
    {
        usage ();
    }

    drv = argv[1][0] - 65;
    cpm_resetDrives ();
    cpm_setCurDrive (drv);

    switch (drv)
    {
    case 0:
    case 1:
        /* floppy drive - not supported! */
        printf ("Formatting floppy drive %c:\n", 65 + drv);
        seclen = 128;
        sectors = 26;
        tracks = 77;
        fat_tracks = 4;
        break;
    case 8:
    case 9:
        /* 4MB hard disks */
        printf ("Formatting hard drive %c:\n", 65+ drv);
        seclen = 128;
        sectors = 128;
        tracks = 255;
        fat_tracks = 2;
        break;
    default:
        printf ("Unsupported drive %d:\n", 65 +drv);
        return 1;
        break;
    }

    show_cursor(false);

    cpm_setDMAAddr ((uint16_t) & buffer);
    for (t = 0; t < fat_tracks; t++) {
        cpm_settrk (t);
        for (s = 1; s <= sectors; s++) {
            printf ("Formatting FAT track %03u, sector %03u, offset = 0x%08lx\r", t, s, offset);
            memset (&buffer, 0, 128);
            if (offset <= 0x8000)
            {
                /* need to write directory entries here, otherwise we just write all zeroes */
                for (j = 0; j < 128; j += 32)
                {
                    buffer[j] = 0xE5;
                }
            }

            cpm_setsec (s);
            cpm_writesec ();
            offset += 128;
        }
    }

    printf ("\n");
    show_cursor(true);

    return 0;
}


/* WARNING: these are very specific to Z80pack, and may well corrupt the filesystem on other CP/M implementions  */

void
cpm_settrk (uint16_t t)
{

    param0 = t;
    __asm
    ld a, (#_param0)
    out (#0x0b), a
    __endasm;
}

void
cpm_setsec (uint16_t s)
{
    param0 = s;
    __asm
    ld a, ( #_param0)
    out ( #0x0c), a
    __endasm;
}

void
cpm_writesec ()
{

	/* write mode, non-deferred */

    __asm;
    ld a, #0x01
    ld c, #0x01
    out (#0x0d), a
    __endasm;
}
