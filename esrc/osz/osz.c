#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "cpmbdos.h"
#include "stdio.h"
#include "syslib/cpm_sysfunc.h"
#include "syslib/ansi_term.h"
#include "bogomips.h"
#include "dump.h"
#include "clkchk.h"
#include "modules.h"
#include "cls.h"
#include "reset.h"
#include "detect.h"
#include "shipctl.h"
#include "cat.h"
#include "fcat.h"
#include "fcntl.h"
#include "debug.h"
#include "isr0.h"


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

void __sdcc_heap_init();
extern uint16_t __sdcc_heap_end;
extern uint16_t __sdcc_heap_free;
extern const uint16_t stktop;

//#define DEBUG_SIGCHECK
#define STACK_RESERVED	1024

struct _archtbl
{
    char signature[17];
    char DFS;
    short cpm_major;
    short cpm_minor;
    char *arch;
    tty_type tty;
    
};

typedef struct _archtbl ArchTable;

const ArchTable archtbl[] = {
    {"C300010000DB00C9", 0xb3, 0, 0, "Hydrogen Reference Board", TTY_VT100 },
    {"C303FA0000C306EE", 0x7c, 1, 3, "Z80Pack CP/M 1.3", TTY_VT100 },
    {"C303FA0000C306ED", 0x7c, 1, 4, "Z80Pack CP/M 1.4", TTY_VT100 },
    {"C303FA0000C306EC", 0x7c, 2, 2, "Z80Pack CP/M 2.2", TTY_VT100 },
    {"C303FCFB00C306E8", 0x7c, 2, 2, "Z80Pack CP/M 3.0", TTY_VT100 },
    {"C303AD8100C3069F", 0x95, 2, 2, "Amstrad CP/M 2.2", TTY_AMSTRAD2 },
    {"C303FC0000C306F6", 0x95, 3, 0, "Amstrad CP/M Plus", TTY_AMSTRAD3 },
    {"0000000000000000", 0x00, 0, 0, NULL}
};

const jmpTable jmptbl[] = {
    {bogomips, "bogomips", false, true},
//    {ttystress, "ttystress", false, false},
//    {clkchk, "clkchk", false, true},
    {dump, "dump", false, false},
    {quit, "quit", true, false},
    {modules, "modules", false, false},
    {cls, "cls", false, true},
//    {reset, "reset", false, false},
    {detect, "detect", false, false},
//    {shipctl, "shipctl", false, false},
//    {cat, "cat", false, false},
    {fcat, "fcat", false, false},
    {debug, "debug", false, false},
    {0x0, "", false, false}
};


typedef uint32_t mathRegister;
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
//extern enum tty_type TERM;

int cpm_major = 0;
int cpm_minor = 0;


#define MAX_STRING 80

#define VER_MAJOR   0
#define VER_MINOR   42

char DFS = '|';
bool errorFlag = false;
char *x;			/* ugly global pointer */
char parseString[MAX_STRING];
extern uint16_t heapaddr;

mathRegister
parseCommand ()
{
    char *ptr = (char *) parseToken ();
    printf ("-> %s\n", ptr);
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
            //printf("%u: %s ? %s\n", jmpIndex, jmptbl[jmpIndex].command, parseString);
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
		uint16_t *stack_pointer = NULL;
		char  *ptr1 = NULL,  *ptr2 = NULL;
		size_t n = 0;

		__sdcc_heap_init(); 

    tickptr = clocktick;
    tickptr2 = (unsigned int) tickptr;

    DFS = '|';
    sprintf ((char *) &signature, "%02X%02X%02X%02X%02X%02X%02X%02X",
             sigptr[0], sigptr[1], sigptr[2], sigptr[3],
             sigptr[4], sigptr[5], sigptr[6], sigptr[7]);
    printf ("\nOS/Z80 %u.%u\nSignature [%s]\n", VER_MAJOR, VER_MINOR,
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
#endif 
        signature[4] = archptr->signature[4];
        if (!strcmp (archptr->signature, signature))
            break;;
        archindex++;
        archptr = (ArchTable *) & archtbl[archindex];
    }

    printf ("Found %s\n", (archptr->arch ? (const char *) archptr->arch : (const char *) "UNKNOWN"));
    DFS = archptr->DFS;
    cpm_major = archptr->cpm_major;
    cpm_minor = archptr->cpm_minor;
    if (archptr->cpm_major)
    {
        has_cpm = true;
        printf ("Detected CP/M level is %u.%u\n", cpm_major, cpm_minor);
        set_term(archptr->tty);    
        printf("Terminal type is: %s\n", get_ttytype(archptr->tty));
    }
    else
    {
        printf ("No CP/M available.\nRunning in standalone mode.\n");
    }
    detect(NULL);

    //TERM=archptr->tty;
    
   // set_term(archptr->tty);    
    //printf("Terminal type is: %s\n", get_ttytype(archptr->tty));

    /* currently not filled in */

		printf("__heap_end  : 0x%04x\n", &__sdcc_heap_end);
		printf("__heap_free : 0x%04x\n", &__sdcc_heap_free);
		printf("__&stacktop  : 0x%04x\n", &stktop);
		stack_pointer = (uint16_t*) &stktop;
		printf("__stack_pointer  : 0x%04x\n", *stack_pointer);
		printf("heap: (0x%04x - 0x%04x) = 0x%04x (%u bytes)\n", (*stack_pointer - STACK_RESERVED), &__sdcc_heap_end, 
							((*stack_pointer - STACK_RESERVED) - &__sdcc_heap_end),
							((*stack_pointer - STACK_RESERVED) - &__sdcc_heap_end)
							);

		printf("Clearing heap ...\n");
		ptr1 = (char  *) &__sdcc_heap_end;
		ptr2 = *stack_pointer;
		n = (size_t) (ptr2 - STACK_RESERVED) - ptr1;
		printf("start = 0x%04x\n", ptr1);
		printf("length = 0x%04x (%u)\n", n, n);
		printf("finish = 0x%04x\n", ptr1 + n);
		memset(ptr1, 0xfe, n);

    printf("argc: %d\n", argc);
    for (i = 1 ; i < argc; i++) {
        if (argv[i] != NULL) {
            printf("[%s] ", (const char *) argv[i]);
            }
        }
		printf("\n");

    while (1)
    {
        mathRegister result = 0;
        command_was_executed = false;
        printf ("> ");
        memset (&chr, 0, READLINE_BUF_SIZE);

        cpm_gets (chr);


        //printf("\n[%u] ", strlen(chr));
        cpm_putchar ('\n');
        x = chr;
        //result = parseCommand();
        result = parseSum ();
        if (!errorFlag && !command_was_executed)
        {
            printf ("%lu\n", result);
        }
        errorFlag = false;

    }


}

void
clocktick ()
{
    printf ("*");
    return;

}


int
bogomips (char *payload)
{
    //printf("BogoMips test not (yet) implemented.\n");
    bogomips_main ();
		payload = NULL;
    return 0;

}


int
quit (char *payload)
{
    if (!has_cpm)
    {
        printf ("Only supported under CP/M.\n");
        return -1;
    }
		payload = NULL;
    exit (0);
    /* should not be reached */
    while (1)
    {
    }
}



