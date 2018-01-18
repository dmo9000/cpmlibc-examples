#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <syslib/cpm_sysfunc.h>
#include "shipctl.h"
#include "box.h"
#include "tty.h"

extern bool has_hydrogen;
extern int si;
//extern enum tty_type TERM;

char* const array[] = { "System Information", "System Diagnostics", "Vehicle Information", "Vehicle Diagnostics", "Exit", NULL };

static bool within_menu = false;
extern void clear_screen();
void show_menu();

int quitfunction(char *payload)
{
    within_menu = false;
		payload = NULL;
    return 0;
}

int nullfunction(char *payload)
{

    setcursor(1, 21);
		payload = NULL;
    return 0;
}

const menuTable MainMenu[] = {
    { nullfunction, "System Information", false, false, false },
    { nullfunction, "System Diagnostics", false, false, false },
    { nullfunction, "Vehicle Information", false, false, false },
    { nullfunction, "Vehicle Diagnostics", false, false, false },
    { nullfunction, "FTL Configuration", false, false, false },
    { quitfunction, "Exit", false, false, true }
};


#define MAIN_MENU_SIZE ((sizeof(MainMenu)/sizeof(menuTable)))

int selected = 0, last_selected = -1;


int shipctl(char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    int c = 0;
    int loopcount = 0;

    if (strncmp(payload, "0x", 2) == 0) {
        addr = strtoul(payload, NULL, 16);
    } else {
        addr = strtoul(payload, NULL, 10);
    }

		payload = NULL;

    if (!has_hydrogen) {
        si = 1;
    }

    if (get_term() == TERM_AMSTRAD2 || get_term() == TERM_AMSTRAD3) {
        si = 2;
    }


    clear_screen();
    drawbox(0, 0, 39, 22);
    drawbox(40, 0, 79, 22);
    setcursor(1,1);


    within_menu = true;
    show_menu();

    if (!MainMenu[selected].must_select) {
        MainMenu[selected].cmdptr(MainMenu[selected].command);
    }


    while (within_menu == true) {
        c = cpm_getchar_noecho();
        /*
        if (c) {
            setcursor(1, 19);
            printf("cpm_getchar_noecho() = %u\n", c);
            }
        */
        switch (c) {
        case 'q':
            selected --;
            break;
        case 'a':
            selected ++;
            break;
        case 10:
        case 13:
            if (MainMenu[selected].must_select) {
                MainMenu[selected].cmdptr(NULL);
            }
            break;
        }

        if (selected < 0) selected = 0;
        if (selected > (MAIN_MENU_SIZE-1)) selected = (MAIN_MENU_SIZE-1);

        if (selected != last_selected) {
            show_menu();
            if (!MainMenu[selected].must_select) {
                MainMenu[selected].cmdptr(MainMenu[selected].command);
            }
        }

        last_selected = selected;

        setcursor(1, 20);
//       printf("%04u pressed = [%03u], = 0x%02x, selection = %u", loopcount, c, c, selected);

        loopcount++;
    }
		clear_screen();
    setcursor(0,0);

    return 0;
}

void show_menu()
{
    int i = 0 ;
    menuTable *MenuPtr = NULL;

    setcursor(1,1);
//  printf("MENU_CHOICES = %u, MAIN_MENU_SIZE = %u\n", MENU_CHOICES, MAIN_MENU_SIZE);

    for (i = 0; i < MAIN_MENU_SIZE; i++) {
        MenuPtr = (menuTable*) &MainMenu[i];
        if (MenuPtr->command) {
            setcursor(3, (i*2) + 3);
            if (i == selected) {
                set_reverse(true);
                printf("%s", (char *) MenuPtr->command);
                set_reverse(false);
            } else {
                printf("%s", (char *) MenuPtr->command);
            }
        }
    }

}
