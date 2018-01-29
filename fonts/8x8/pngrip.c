/*
 * A simple libpng example program
 * http://zarb.org/~gc/html/libpng.html
 *
 * Modified by Yoshimasa Niwa to make it much simpler
 * and support all defined color_type.
 *
 * To build, use the next instruction on OS X.
 * $ brew install libpng
 * $ clang -lz -lpng15 libpng_test.c
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <stdint.h>
#include <string.h>
#include "rip.h"

#define MAX_COLORS 256
int16_t GetColorIndex(uint8_t r, uint8_t g, uint8_t b);

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

static rip MyRip;
static ripcolor MyRipColors[MAX_COLORS];
FILE *ripfh = NULL;


static uint8_t chars[256][8];

void read_png_file(char *filename, char *outfilename) {
    FILE *fp = fopen(filename, "rb");
    png_uint_32 num_palette;
    png_colorp palette;
    int i = 0;


    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) abort();

    png_infop info = png_create_info_struct(png);
    if(!info) abort();

    if(setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    png_read_info(png, info);

    width      = png_get_image_width(png, info);
    height     = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if(bit_depth == 16)
        png_set_strip_16(png);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
            color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    png_read_image(png, row_pointers);
    printf("png_read_image() done\n");

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        printf("good, found indexed image\n");
    } else {
        printf("fail, found non-indexed image\n");
        fclose(fp);
        exit(1);
    }

    printf("bit_depth = %u\n", bit_depth);

    if (bit_depth != 8) {
        printf("bit-depth error\n");
        exit(1);
    }

    if (!png_get_PLTE(png, info, &palette, &num_palette)) {
        printf("error reading palette\n");
        exit(1);
    };

    printf("width = %u, height = %d\n", width, height);
    printf("num_palette = %u\n", num_palette);

//    ripfh = fopen(outfilename, "wb");

    MyRip.width = width;
    MyRip.height = height;
    MyRip.colors = num_palette;

//    fwrite(&MyRip, sizeof(rip), 1, ripfh);

    for (i = 0; i < num_palette; i++) {
       // printf("%u: %u, %u, %u\n", i, palette[i].red, palette[i].green, palette[i].blue);
        MyRipColors[i].r = i;
        MyRipColors[i].r = palette[i].red;
        MyRipColors[i].g = palette[i].green;
        MyRipColors[i].b = palette[i].blue;
        //fwrite(&MyRipColors[i], sizeof(ripcolor), 1, ripfh);
    }

    fclose(fp);
}

void dump_matrix(char *outfilename)
{
	char *ptr = (uint8_t *) &chars;
	FILE *fh = NULL;
	int i = 0;

	fh = fopen(outfilename, "wb");
	if (!fh) {
			perror("fopen");
			exit(1);
			}

	for (i = 0; i < 256; i++) {
				fwrite(ptr, 8, 1, fh);
				ptr+= 8;
		}

	fclose(fh);
	
}

void process_character(int c) {

    int16_t myIndex = 0;
		int row = 0, column = 0;
    uint16_t byte = 0;
		int x = 0, y = 0;
		int h = 0;
		int v = 0;
		int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
		int by = 0;
		int bitmask = 256;
		int setbits = 0;
		row = c / 8;
		column = c % 8;
	
		y1 = (row * 8);
		y2 = y1 +  8;

		x1 = column * 8;
		x2 = x1 + 8;

//	printf("c = %d\n", c);
		v = 0;
    for(int y = y1; y < y2; y++) {
        png_bytep row = row_pointers[y];
				h = 0;
				bitmask = 256;
				setbits = 0;
				//printf("[");
        for(int x = x1; x < x2; x++) {
            png_bytep px = &(row[x * 4]);
						by = 0;
            // Do something awesome for each pixel here...
            //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
            //myIndex = GetColorIndex(px[0], px[1], px[2]);
						byte = px[0] + px[1] + px[2];
						bitmask = bitmask / 2;
						if (byte) {
							//printf(".");
						} else {
							//printf("@");	
							setbits = setbits |= bitmask;
						}
						h++;
        }
		chars[c][v] = setbits;
//		printf("] = %u \n", setbits);
        v++;
    }
}

int main(int argc, char *argv[])
{
		int i = 0;

    if(argc != 3) abort();


    memset(&chars, 0, 8*256);

    read_png_file(argv[1], argv[2]);
		for (i = 33 ; i < 128; i++) {
	    process_character(i);
			}

///    process_character(65);	
		dump_matrix(argv[2]);

		//fclose(ripfh);

    return 0;
}


int16_t GetColorIndex(uint8_t r, uint8_t g, uint8_t b)
{
	int i = 0;
	for (i = 0; i < MAX_COLORS; i++) {
	if (MyRipColors[i].r == r && 
			MyRipColors[i].g == g &&
			MyRipColors[i].b == b) {
				return i;
				} 
			}
			
 return -1;
}
