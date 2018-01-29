#include <stddef.h>
#ifndef NULL
#define NULL ( (void *) 0)
#endif /* NULL */

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memchr(const void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strchr(const char *s, int c);
char *strerror(int errnum);
char *strstr(const char *haystack, const char *needle);
char *strsep(char **stringp, const char *delim);
char *strrchr(const char *s, int c);
char *strcat(char *dest, const char *src);

