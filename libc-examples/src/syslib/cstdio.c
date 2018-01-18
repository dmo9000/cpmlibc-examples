#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <cpm_sysfunc.h>

int getchar()
{

    return cpm_getchar();

}


int err(int e, const char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);

    if (e) {
        vprintf(fmt, args);
        perror("");
        }
    va_end (args);
    return 0;
}

int errx(int e, const char *fmt, ...)
{
    
    va_list args;
    va_start(args, fmt);

    if (e) {
        vprintf(fmt, args);
        }
    va_end (args);
    return 0;
}


int ferror(FILE *stream)
{
   errno = 0;
    if (! stream || stream->_file == -1) {
        return 1;
    }
    return 0;
}


void exit  (int c) 
{

    /* apparently RST 0 should reset CP/M, but it breaks on our system 
       (works on real CP/M) so I'm using JP 0 which works on both */

    //cprintf("exit(%u)\n", c);
    c++;
    c--; 
    __asm
       jp 0
    //rst 0

    __endasm ; 
    while (1) { } 

}

void perror(const char *s)
{

	printf("perror %s: %s", (const char *) s, (const char *) strerror(errno));

}

int isatty(int fd)
{
	if (fd < 3) {
		/* on CP/M we are just going to punt that stdin, stdout and stderr are always open for now */
		return 1;
		}

	/* FIXME: just a stub for now */

	errno = EINVAL;
	return 0;
}

int unlink(const char *path)
{
	printf("++ unlink(%s) not supported\n", path);
	errno = EIO; 
	return -1;
}


char *getenv(char *name)
{
    char *keepcompilerquiet1 = (const char*) name;
	/* not supported on CP/M, there is no environment variable AFAIK */
	return NULL;
}

int chmod(char *path, mode_t mode)
{
    char *keepcompilerquiet1 = (char*) path;
    mode_t keepcompilerquiet2 = (mode_t) mode;
	/* not supported on CP/M, there is no environment variable AFAIK */
	errno = EIO;
	return -1;

}
