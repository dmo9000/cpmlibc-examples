#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

void __sdcc_heap_init();
extern uint16_t __sdcc_heap_end;
extern uint16_t __sdcc_heap_free;
extern const uint16_t stktop;

#define STACK_RESERVED 	1024

size_t heap_init()
{
		size_t n;
		char *ptr1 = NULL;
		char *ptr2 = NULL;
		uint16_t *stack_pointer = NULL;
    /* currently not filled in */

//    printf("__heap_end  : 0x%04x\n", &__sdcc_heap_end);
//    printf("__heap_free : 0x%04x\n", &__sdcc_heap_free);
//    printf("__&stacktop  : 0x%04x\n", &stktop);
    stack_pointer = (uint16_t*) &stktop;
//    printf("__stack_pointer  : 0x%04x\n", *stack_pointer);
//    printf("heap: (0x%04x - 0x%04x) = 0x%04x (%u bytes)\n", (*stack_pointer - STACK_RESERVED), &__sdcc_heap_end,
 //             ((*stack_pointer - STACK_RESERVED) - &__sdcc_heap_end),
 //             ((*stack_pointer - STACK_RESERVED) - &__sdcc_heap_end)
  //            );

//    printf("Clearing heap ...\n");
    ptr1 = (char  *) &__sdcc_heap_end;
    ptr2 = *stack_pointer;
    n = (size_t) (ptr2 - STACK_RESERVED) - ptr1;
//    printf("start = 0x%04x\n", ptr1);
//    printf("length = 0x%04x (%u)\n", n, n);
//    printf("finish = 0x%04x\n", ptr1 + n);
    memset(ptr1, 0xfe, n);
		return (n);
}

void *sbrk(intptr_t increment) 
{
	if (increment) {
			errno = ENOMEM;
			return (void *) -1;
			}
	errno = 0;
	return (void *) &__sdcc_heap_end; 
}

