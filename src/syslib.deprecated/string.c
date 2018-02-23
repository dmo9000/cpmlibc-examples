#include <string.h>
#include <errno.h>

char *strerror(int en)
{
  switch(en) {
        case 0:
            return (const char *) "Success";
            break;
        case ENOENT:
            return (const char *) "No such file or directory";
            break;
        case EIO:
            return (const char *) "I/O error";
            break;
				case E2BIG:
						return (const char *) "Argument list too long";
						break;
        case EBADF:
            return (const char *) "Bad file descriptor";
            break;
        case EINVAL:
            return (const char *) "Illegal address";
            break;
        case ENFILE:
            return (const char *) "Too many open files";
            break;
				case ENOTTY:
						return (const char *) "Not a typewriter";
						break;
           }

   return "Unknown error!";

}

char *
strsep(char **stringp, const char *delim)
{
	char *s;
	const char *spanp;
	int c, sc;
	char *tok;
	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

char *strrchr(const char *s, int c)
{
	char *ptr = (char *) s;
	char *retptr = NULL;

	while (ptr[0] != '\0') {
		if (ptr[0] == c) {
			retptr = (char *) ptr;
			}
			ptr ++;
		}
	return retptr;
}
