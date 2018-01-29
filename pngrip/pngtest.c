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

void read_png_file(char *filename) {
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
        printf("error\n");
        exit(1);
    }

    if (!png_get_PLTE(png, info, &palette, &num_palette)) {
        printf("error reading palette\n");
        exit(1);
    };

    printf("width = %u, height = %d\n", width, height);
    printf("num_palette = %u\n", num_palette);

    ripfh = fopen("testimg.rip", "wb");

    MyRip.width = width;
    MyRip.height = height;
    MyRip.colors = num_palette;

    fwrite(&MyRip, sizeof(rip), 1, ripfh);

    for (i = 0; i < num_palette; i++) {
       // printf("%u: %u, %u, %u\n", i, palette[i].red, palette[i].green, palette[i].blue);
        MyRipColors[i].r = i;
        MyRipColors[i].r = palette[i].red;
        MyRipColors[i].g = palette[i].green;
        MyRipColors[i].b = palette[i].blue;
        fwrite(&MyRipColors[i], sizeof(ripcolor), 1, ripfh);
    }


    fclose(fp);
}

void write_png_file(char *filename) {
    int y;

    FILE *fp = fopen(filename, "wb");
    if(!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
        png,
        info,
        width, height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    for(int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fp);
}

void process_png_file() {

    int16_t myIndex = 0;
    for(int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for(int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            // Do something awesome for each pixel here...
     //       printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);

            myIndex = GetColorIndex(px[0], px[1], px[2]);
            if (myIndex == -1) {
                printf("Couldn't find index for color (%u, %u, %u)\n",
                       px[0], px[1], px[2]);
                exit(1);
            } else {
			//			printf("Index = %u\n", myIndex);
						fputc(myIndex, ripfh);	
						}
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3) abort();

    read_png_file(argv[1]);
    process_png_file();
 //   write_png_file(argv[2]);

		fclose(ripfh);

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
