#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#ifndef __LINUX__
#include <grx.h>
#include <tty.h>
#include <cpm_sysfunc.h>
#endif /* __LINUX__ */


//static bool matrix0[50][50];
static bool matrix1[50][50];
//static bool matrix2[50][50];

void draw_cell(uint16_t x, uint16_t y);
void draw_background();
void draw_grid();
void run_simulation();
void inject_life(int count);

#define COLOR_ALIVE 0 
#define COLOR_DEAD  1 


int main(int argc, char *argv[])
{
    uint32_t seed = 0;
    int i = 0, j = 0;
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

#ifndef __LINUX__
    grx_setvidmode(MODE_320x200x256);
    grx_setpalette(0, 0, 0, 0);
    grx_setpalette(1, 127,127,127);
    grx_setpalette(2, 255,255,255);
    grx_setpalette(3, 255,0,0);
    grx_setpalette(4, 0,255,0);
    //grx_drawbox(1, 0, 0, 319, 199);
    draw_background();
    draw_grid();
    grx_flush();
#endif /* __LINUX__ */


    srandom(seed);

    for (i = 0; i < 50; i++) {
        for (j = 0; j < 50; j++) {
            matrix1[i][j] = false;
        }
    }

    inject_life(1024);

    while (1) {
        //draw_background();
        /* update matrix1 display */
        for (i = 0; i < 50; i++) {
            for (j = 0; j < 50; j++) {
                //if (matrix1[i][j] != matrix0[i][j]) {
                    draw_cell(i, j);
                //}
            }
        }
        //draw_grid();

#ifndef __LINUX__
        grx_flush();
#endif /* __LINUX__ */

        /* run algorithm */
        run_simulation();
        inject_life(128);

#ifndef __LINUX__
        //while (cpm_getchar_noecho() == 0) {
        //   }
#endif /* __LINUX__ */

    }


#ifndef __LINUX__
    grx_resetpalette();
    grx_setvidmode(MODE_TEXTMODE);
#endif /* __LINUX__ */

    exit(0);
}

void draw_cell(uint16_t x, uint16_t y)
{
    bool cellstate = matrix1[x][y];
    switch(cellstate) {
    case false:
#ifndef __LINUX__
        grx_drawbox(COLOR_DEAD, (x * 4) + 1, (y * 4) + 1,
                    (x*4) + 3, (y*4) + 3);
#endif /* __LINUX__ */
        break;
    case true:
#ifndef __LINUX__
        grx_drawbox(COLOR_ALIVE, (x * 4) + 1, (y * 4) + 1,
                    (x*4) + 3, (y*4) + 3);
#endif /* __LINUX__ */
        break;
    }
}

void draw_background()
{
    
    grx_drawbox(1, 0, 0, 319, 199);

}

void draw_grid()
{
    int i = 0;
    for (i = 0; i < 50 ; i++) {
#ifndef __LINUX__
        grx_drawline(0, 0, i * 4, 200, i*4);
        grx_drawline(0, i * 4, 0, i*4, 199);
#endif /* __LINUX__ */
    }

#ifndef __LINUX__
    grx_drawline(0, 200, 0, 200, 200);
    grx_drawline(0, 0, 200, 200, 200);
#endif /* __LINUX__ */


}

int get_neighbour_count(int x, int y)
{
    int nc = 0;
    int i = 0, j = 0;

    for (i = x - 1; i < x+2; i++) {
        for (j = y - 1; j < y+2; j++) {

            if ((i >=0 && i <=50) && (j >=0 && j <= 50) &&
                    (i != x && j != y)) {
                /* range/self check ok */

                if (matrix1[i][j]) {
                    nc++;
                }

            }
        }
    }
    return nc;
}



void run_simulation()
{
    int i = 0, j = 0;
    int nc = 0;

  //  memcpy(&matrix0, &matrix1, sizeof(matrix1));
 //   memcpy(&matrix2, &matrix1, sizeof(matrix1));

    for (i = 0; i < 50; i++) {
        for (j = 0; j < 50; j++) {
            nc = get_neighbour_count(i, j);
            switch(nc) {
            case 2:
            case 3:
                /* no change */
               // matrix2[i][j] = matrix1[i][j];
                break;
            default:
                /* death via overcrowding or loneliness */
                matrix1[i][j] = false;
                break;
            }
        }
    }

//    memcpy(&matrix1, &matrix2, sizeof(matrix2));

    for (i = 0; i < 50; i++) {
        for (j = 0; j < 50; j++) {
           // matrix2[i][j] = matrix1[i][j];
            nc = get_neighbour_count(i, j);
            if (matrix1[i][j] == false) {
                /* come alive if neighbour count is == 3 */
                if (nc == 3) {
                    matrix1[i][j] = true;
                } else {
                    matrix1[i][j] = matrix1[i][j];
                }
            }
        }
    }

//    memcpy(&matrix1, &matrix2, sizeof(matrix2));
    return;

}


void inject_life(int count)
{
    int i = 0;
    uint16_t x1 = 0;
    uint16_t y1 = 0;

    for (i = 0; i < count; i++) {
pick_again:
        x1 = random() % 50;
        y1 = random() % 50;
        if (!matrix1[x1][y1]) {
            matrix1[x1][y1] = true;
            } else {
            goto pick_again;
            }
    }
}
