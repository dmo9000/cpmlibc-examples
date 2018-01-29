#include <libc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include "cpmbdos.h"
#include "cprintf.h"
#include "syslib/cpm_sysfunc.h"
#include "syslib/ansi_term.h"
#include "bogomips.h"
#include "dump.h"
#include "clkchk.h"
#include "modules.h"
#include "call.h"
#include "halt.h"
#include "wait.h"
#include "box.h"
#include "cls.h"
#include "reset.h"
#include "debug.h"
#include "seldsk.h"
#include "testllio.h"
#include "detect.h"
#include "isr0.h"

//#define DEBUG_SIGCHECK

#define CHAR_ESCAPE             0x1B
#define CHAR_ESCAPE_CLS         0x48
#define CHAR_ESCAPE_SETCX       0x49
#define CHAR_ESCAPE_SETCY       0x4A
#define CHAR_ESCAPE_CURSOR      0x4B

bool command_was_executed = false;

int bogomips (char *payload);
int quit (char *payload);
int ttystress (char *payload);
void clocktick ();

struct _archtbl
{
    char signature[17];
    char DFS;
    short cpm_major;
    short cpm_minor;
    char *arch;
};

typedef struct _archtbl ArchTable;

const ArchTable archtbl[] = {
    {"C300010000DB00C9", 0xb3, 0, 0, "Hydrogen (Bare metal)"},
    {"C303FA0000C306EE", 0x7c, 1, 3, "Z80Pack CPM1.3"},
    {"C303FA0000C306ED", 0x7c, 1, 4, "Z80Pack CPM1.4"},
    {"C303FA0000C306EC", 0x7c, 2, 2, "Z80Pack CPM2.2"},
    {"C303FCFB00C306E8", 0x7c, 3, 0, "Z80Pack CPM3.0"},
    {"C303FC0000C306F6", 0x95, 3, 0, "Amstrad CPM+"},
    {"0000000000000000", 0x00, 0, 0, NULL}
};

const jmpTable jmptbl[] = {
    {bogomips, "bogomips", false, true},
    {ttystress, "ttystress", false, false},
    {clkchk, "clkchk", false, true},
    {dump, "dump", false, false},
    {quit, "quit", true, false},
    {modules, "modules", false, false},
    {call, "call", false, false},
    {halt, "halt", false, true},
    {wait, "wait", false, false},
    {box, "box", false, true},
    {cls, "cls", false, true},
    {reset, "reset", false, false},
    {debug, "debug", false, false},
    {seldsk, "seldsk", false, false},
    {testllio, "testllio", false, false},
    {detect, "detect", false, false},
    {0x0, "", false, false}
};


typedef u_int32_t mathRegister;
mathRegister parseToken ();
mathRegister parseCommand ();
mathRegister parseSum ();
mathRegister parseProduct ();
mathRegister parseFactor ();

#define Z80_BASEADDR    0x0
#define GUAVA_BASE  0x00F0
#define GUAVA_MAGIC "GUAVA"
#define MAGIC_GUAVA "\303\000\001"
#define MAGIC_CPM2  "\303\003\372"
#define MAGIC_CPM3 "\303\003\374"


bool has_hydrogen = false;
bool has_cpm = false;

int cpm_major = 0;
int cpm_minor = 0;


#define MAX_STRING 80

#define VER_MAJOR   0
#define VER_MINOR   33

char DFS = '|';

bool errorFlag = false;

char *x;			/* ugly global pointer */
char parseString[MAX_STRING];

void
sys_init (void)
{
    cpm_sysfunc_init ();
}


mathRegister
parseCommand ()
{
    char *ptr = parseToken ();
    cprintf ("-> %s\n", ptr);
    return 0;
}

bool
isWhitespace (char c)
{
    switch (c)
    {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        return true;
    default:
        return false;
        break;
    }

}

bool
isTerminator (char c)
{
    switch (c)
    {
    case '\0':
        return true;
    default:
        return false;
        break;
    }

}

mathRegister
parseToken ()
{
    int i = 0;
    char *p = x;
    memset (&parseString, 0, MAX_STRING);
    while (!isWhitespace (x[i]) && !isTerminator (x[i]) && i < MAX_STRING)
    {
        parseString[i] = x[i];
        i++;
    }
    parseString[i] = '\0';
    x += i;
    while (isWhitespace (x[0]) && !isTerminator (x[0]))
    {
        x++;
    }
    return 0;
}

mathRegister
parseSum ()
{

    mathRegister pro1 = parseProduct ();
    while (*x == '+')
    {
        mathRegister pro2;
        ++x;
        pro2 = parseProduct ();
        pro1 = pro1 + pro2;
    }
    return pro1;

}

mathRegister
parseProduct ()
{
    mathRegister fac1 = parseFactor ();
    while (*x == '*')
    {
        mathRegister fac2;
        ++x;
        fac2 = parseFactor ();
        fac1 = fac1 * fac2;
    }
    return fac1;

}

mathRegister
parseFactor ()
{

    mathRegister sum1 = 0;
    char *ptr = NULL;
    int jmpIndex = 0;
    int (*jumpPtr) (char *payload);
    if (*x >= '0' && *x <= '9')
    {
        while (*x >= '0' && *x <= '9')
        {
            sum1 = (sum1 * 10) + (*x - '0');
            *x++;
        }
        return sum1;
    }
    else if (*x == '(')
    {
        mathRegister sum;
        ++x;			/* consume ( */
        sum = parseSum ();
        ++x;			/* consume ) */
        return sum;
    }
    else
    {
        parseToken ();
        jumpPtr = jmptbl[jmpIndex].cmdptr;

        if (!strlen (parseString))
        {
            errorFlag = true;
            return 0;
        }

        while (jumpPtr > 0)
        {
            //cprintf("%u: %s ? %s\n", jmpIndex, jmptbl[jmpIndex].command, parseString);
            if (strncmp
                    (jmptbl[jmpIndex].command, parseString,
                     strlen (jmptbl[jmpIndex].command)) == 0)
            {
                command_was_executed = true;
                return jumpPtr (x);
            }
            jmpIndex++;
            jumpPtr = jmptbl[jmpIndex].cmdptr;
        }

        printf ("syntax error: %s\n", parseString);
        errorFlag = true;
        return 0;
    }
}

#define READLINE_BUF_SIZE 80

mathRegister
main (int argc, char *argv[])
{
    unsigned char c = 0, d = 0;
    unsigned char chr[READLINE_BUF_SIZE];
    unsigned char signature[17];
    ArchTable *archptr = NULL;
    int archindex = 0;
    char *sigptr = 0;
    mathRegister i = 0;
    void (*tickptr) ();
    unsigned int tickptr2;
    int myfd = 0;
    sys_init ();

    tickptr = clocktick;
    tickptr2 = (unsigned int) tickptr;

    //myfd = open("testfile.txt", O_CREAT|O_WRONLY|O_TRUNC);
    //close(myfd);


    DFS = '|';
    sprintf ((char *) &signature, "%02X%02X%02X%02X%02X%02X%02X%02X",
             sigptr[0], sigptr[1], sigptr[2], sigptr[3],
             sigptr[4], sigptr[5], sigptr[6], sigptr[7]);
    cprintf ("\nOS/Z80 %u.%u\nSignature [%s]\n", VER_MAJOR, VER_MINOR,
             signature);



    archptr = (ArchTable *) & archtbl[archindex];
    /* copy the DSK byte from the header before each comparison */
    while (archptr->arch != NULL)
    {
        signature[8] = archptr->signature[8];
        signature[9] = archptr->signature[9];
#ifdef DEBUG_SIGCHECK
        printf ("<");
        for (i = 0; i < 16; i += 2)
        {
            printf ("%c%c", signature[i], signature[i + 1]);
        }
        printf ("\n");

        printf (">");
        for (i = 0; i < 16; i += 2)
        {
            printf ("%c%c", archptr->signature[i], archptr->signature[i + 1]);
        }
        printf (" %s\n", archptr->arch);
#endif /* DEBUG_SIGCHECK */

        signature[4] = archptr->signature[4];
        if (!strcmp (archptr->signature, signature))
            break;;
        archindex++;
        archptr = (ArchTable *) & archtbl[archindex];
    }

    cprintf ("Found %s\n", (archptr->arch ? archptr->arch : "UNKNOWN"));
    DFS = archptr->DFS;
    cpm_major = archptr->cpm_major;
    cpm_minor = archptr->cpm_minor;
    if (archptr->cpm_major)
    {
        has_cpm = true;
        cprintf ("Detected CP/M level is %u.%u\n", cpm_major, cpm_minor);
    }
    else
    {
        cprintf ("No CP/M available.\nRunning in standalone mode.\n");
    }
    detect(NULL);

    /* currently not filled in */
    if (argc) {
        printf("argc: %d\n", argc);
        }

    /* clear section of memory for test suite */

    memset (0x8000, 0, 1024);
    cprintf ("\n");

    while (1)
    {
        mathRegister result = 0;
        command_was_executed = false;
        cprintf ("> ");
        memset (&chr, 0, READLINE_BUF_SIZE);

        cpm_gets (chr);


        //cprintf("\n[%u] ", strlen(chr));
        cpm_putchar ('\n');
        x = chr;
        //result = parseCommand();
        result = parseSum ();
        if (!errorFlag && !command_was_executed)
        {
            cprintf ("%lu\n", result);
        }
        errorFlag = false;

        /*
                __asm
                di
                __endasm;
        */

    }

    exit (0);
    return (EXIT_SUCCESS);

}

void
clocktick ()
{
    cprintf ("*");
    return;

}


int
bogomips (char *payload)
{
    //cprintf("BogoMips test not (yet) implemented.\n");
    bogomips_main ();
    return 0;

}


int
quit (char *payload)
{
    if (!has_cpm)
    {
        printf ("Only supported under CP/M.\n");
        return -1;
        exit (0);
    }

    exit (0);
    /* should not be reached */
    while (1)
    {
    }
}


void
clear_screen ()
{
    cprintf ("%c[H%c[2J", CHAR_ESCAPE, CHAR_ESCAPE);
}

bool
continue_or_exit ()
{

    unsigned char chr[READLINE_BUF_SIZE];
    printf ("  More? [Y] continue [N] exit: ");
    memset (&chr, 0, READLINE_BUF_SIZE);
    cpm_gets (chr);
    return ((chr[0] == 'Y' || chr[0] == 'y') ? true : false);
}
