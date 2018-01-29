#ifndef __STDIO_H__
#define __STDIO_H__
//#include <syslib/cpm_sysfunc.h>
#include <stddef.h>
#include <stdbool.h>


#if z80
#define BUFSIZ    512
#define _NFILE    8
#else  /* z80 */
#define BUFSIZ    1024
#define _NFILE    20
#endif /*  z80 */

#define EOF 0x1A	
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2
#define LONG_MAX 		2147483647

#define uchar unsigned char

struct  _iobuf {
  char *    _ptr;
  int   _cnt;
  char *    _base;
  char   _flags[4];
  int    _file;
  bool 	_eof;
  ssize_t _limit;
}; 

typedef struct _iobuf FILE;
#define stdin           (&filehandles[0])
#define stdout          (&filehandles[1])
#define stderr          (&filehandles[2])

int myprintf(const char *fmt, ...);
#define printf(...) myprintf(__VA_ARGS__)
int fprintf(FILE *stream, const char *format, ...);
int sprintf(const char *str, const char *fmt, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int open(const char *pathname, int flags);
FILE *fopen(const char *path, const char *mode);
int fseek(FILE *stream, long offset, int whence);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
int feof(FILE *stream);
int fclose(FILE *stream);
long ftell(FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
char *getenv(const char *name);
int fflush(FILE *stream);
int puts(const char *s);
void perror(const char *s);
int ferror(FILE *stream);
int getchar(void);
//int putchar(int c);
#define putchar cpm_putchar
#endif /* __STDIO_H__ */
