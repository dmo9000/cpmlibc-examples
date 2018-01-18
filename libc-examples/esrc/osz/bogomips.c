/*
 *                Standalone BogoMips program
 *
 * Based on code Linux kernel code in init/main.c and
 * include/linux/delay.h
 *
 * For more information on interpreting the results, see the BogoMIPS
 * Mini-HOWTO document.
 *
 * version: 1.3 
 *  author: Jeff Tranter (Jeff_Tranter@Mitel.COM)
 *
 * djm 20/3/2000
 * Made a little bit more relevent for 8 bit machines in the printing
 * stage...should be right!
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


extern unsigned int llclock();

extern bool has_cpm;
extern bool has_hydrogen;

#define PORTABLE_BOGOMIPS
//#define Z80_DELAY
 
int wallclocktime = 0;

#ifdef CLASSIC_BOGOMIPS
/* the original code from the Linux kernel */
static __inline__ void delay(int loops)
{
  __asm__(".align 2,0x90\n1:\tdecl %0\n\tjns 1b": :"a" (loops):"ax");
}
#endif

#ifdef QNX_BOGOMIPS
/* version for QNX C compiler */
void delay(int loops);
#pragma aux delay = \
     "l1:"       \
     "dec eax"   \
     "jns l1"    \
     parm nomemory [eax] modify exact nomemory [eax];
#endif

#ifdef PORTABLE_BOGOMIPS
/* portable version */
static void delay(unsigned long loops)
{
  long i;
  //printf("delay(%lu)\n", loops);
  for (i = loops; i != 0 ; i--) 
    ;
}
#endif

/*
 * This is a chronic cheat! The code is completely different and runs
 * many times quicker..<ahem>
 *
 * We could just forget it really!
 */

#ifdef Z80_DELAY
static void delay(long loops)
{
//#asm
__asm
.del_loop
	call	l_declong
	ld	a,h
	or	l
	or	e
	or	d
	jp	nz,del_loop
//#endasm
__endasm;
}
#endif

/*
int clock()
{
    //unsigned int *ptr = (unsigned char  *) 0xfffa;
    //unsigned int *ptr = 0x00f7;
    //unsigned int wallclocktime = (unsigned int ) *ptr;
    //unsigned int wallclocktime = 666; 
    wallclocktime = llclock(); 
    return wallclocktime;
}
*/

#define CLOCKS_PER_SEC 50 

int
bogomips_main(char *payload)
{
  unsigned long loops_per_sec = 1;
  unsigned long ticks;
  unsigned long bogo;
  unsigned long sub;

	payload = NULL;

  if (!has_hydrogen) {
    printf("No clock source available.\n");
    return 0;
    }
  
  printf("\nCalibrating delay loop.. ");
  //fflush(stdout);
  //
  
  __asm
    im 1
    ei 
  __endasm;

  
  while ((loops_per_sec <<= 1)) {
    ticks = clock();
    delay((unsigned long) loops_per_sec);
    ticks = clock() - ticks;
    if (ticks >= CLOCKS_PER_SEC) {
      loops_per_sec = (loops_per_sec / ticks) * CLOCKS_PER_SEC;
      bogo = loops_per_sec/500000L;
      sub = loops_per_sec/5000L;
      sub %=100;
      printf("\nok - %lu.%s%lu BogoMips\n\n",
			bogo,
			(sub<10)?"0":"",
			sub
		);
      return 0;
    }
  }

  __asm
    di
  __endasm;

  printf("failed\n");
  return -1;
}
