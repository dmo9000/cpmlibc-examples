#define ENOENT  	2           /* no such file or directory */
#define	EIO				5	    	    /* I/O error */
#define	E2BIG		 	7						/* Argument list too long */
#define EBADF   	9           /* bad file descriptor */
#define	ENOMEM		12					/* Out of memory */
#define EINVAL  	22          /* negative offset or offset beyond end of file? Invalid address */
#define ENFILE  	23          /* too many open files (file table overflow) */
#define	ENOTTY		25					/* Not a typewriter */

extern int errno;
