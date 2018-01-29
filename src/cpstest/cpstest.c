#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <hydrogen.h>
#include <ansi_term.h>
#include <cpm_sysfunc.h>

int main(int argc, char *argv[])
{
    uint32_t s_ticks = 0, c_ticks = 0, m_ticks = 0;
    uint32_t counter = 0;
    uint32_t lcounter = 0;
    uint32_t results[10];

    int i = 0;

    if (lldetect() != '!') {
        printf("No clock source available.\n");
        return 0;
    } else {
        printf("Valid clock source detected\n");
    }

    show_cursor(false);

    for (i = 0 ; i < 10 ; i++) {
        m_ticks = 0;
        s_ticks = clock();
        while (m_ticks < 50) {
            c_ticks = clock();
            m_ticks = c_ticks - s_ticks;
            //printf("ticks = %u -> %lu,%lu,%lu\n", i, s_ticks, c_ticks, m_ticks);
            cpm_putchar('.');
            counter++;
            lcounter++;
        }
        //printf("-TOCK-\n");
        results[i] = counter;
        counter = 0;
        cpm_putchar('|'); 
    }
    printf("\n");
    printf("Total character count: %lu\n", lcounter);
    for (i = 0; i < 10; i++) {
        printf("pass %d: %lu\n", i, results[i]);
        }

    show_cursor(true);

    exit(0);
}
