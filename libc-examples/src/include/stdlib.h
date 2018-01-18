#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stddef.h>
#include <z80/malloc.h>

long strtol(char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
void exit(int status);
int atoi(const char *nptr);
/*
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
*/
int abs(int j);
long int random(void);
void srandom(unsigned int seed);


/*
 * TODO: moved to z80/malloc.h
 *
struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
  int magic; // For debugging only. TODO: remove this in non-debug mode.
};
*/

#define META_SIZE sizeof(struct block_meta)

#define malloc(...) 	_z80malloc(__VA_ARGS__)
#define free(...)  		_z80free(__VA_ARGS__)
#define realloc(...) 	_z80realloc(__VA_ARGS__)
#define calloc(...) 	_z80calloc(__VA_ARGS__)

#ifndef NULL
#define NULL 0
#endif /* NULL */


/* random numbers */

long int random(void);

#endif /* __STDLIB_H__ */
