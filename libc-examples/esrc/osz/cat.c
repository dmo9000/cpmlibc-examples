#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "cpmbdos.h"
#include "syslib/cpm_sysfunc.h"
#include "syslib/ansi_term.h"
#include "cat.h"

extern bool has_hydrogen;

/* optimal chunk size is 128 for now */
#define CHUNK_SIZE 128 


int
cat (char *payload)
{

    static char buffer[CHUNK_SIZE];
    off_t offset = 0;
    ssize_t rd = -1;
    int fd = 0;
    int i = 0;
    char *eof = NULL;
    off_t rv = 0;
    if (payload[0] == '\0')
    {
        printf ("usage: cat <filename>\n");
        return 1;
    }

    memset (&buffer, 0x00, CHUNK_SIZE);

    fd = open ((const char *) payload, O_RDONLY);
    if (fd != -1)
    {
//        printf ("* Opened '%s' fd=%d\n", payload, fd);
//        printf ("* Reading from fd %d\n", fd);
//        printf ("---\n");
        rv = lseek(fd, 0, SEEK_SET);


        if (rv == -1) {
            printf ("seek error:rv = %ld, fd = %d, buffer = 0x%04x,\n\terrno = %d, strerror = %s\n",
                    rv, fd, &buffer, errno, strerror (errno));
            goto close_file;
        }

        rd = read (fd, &buffer, CHUNK_SIZE);
        while (rd > 0)
        {
            eof = memchr(&buffer, 26, CHUNK_SIZE);
            if (eof) {
                eof -= (int) &buffer;
                //printf("[EOF marker in this block at %u, rd = %d]\n", eof, rd);
                rd = (ssize_t) eof;
            }

            for (i = 0; i < rd ; i++)
            {
                //printf ("%c", buffer[i]);
                cpm_putchar(buffer[i]);
            }

        
            offset += rd;
            //printf("\n+ rd = %d, offset = d\n", rd, offset);
            if (eof) {
                goto close_file;
                } 
            rv = lseek(fd, offset, SEEK_SET);

            if (rv == -1) {
                printf ("seek error:rv = %ld, fd = %d, buffer = 0x%04x,\n\terrno = %d, strerror = %s\n",
                        rv, fd, &buffer, errno, strerror (errno));
                goto close_file;
            }


            rd = read (fd, &buffer, CHUNK_SIZE);
        }

        //printf("rd = %d\n", rd);
        if (rd < 0) {
            printf("\n-- ABORT --\n");
        }

        switch (rd)
        {
        case 0:
            /* end of file reached */
            goto close_file;
            break;
        case -1:
            printf
            ("read error:rd = %ld, fd = %d, buffer = 0x%04x, errno = %d, strerror = %s\n",
             rd, fd, &buffer, errno, strerror (errno));
            goto close_file;
            break;
        default:
            printf ("read with bad length! rd = %d\n", rd);
            goto close_file;
            break;
        }



close_file:
//        printf ("\n---\n");
//        printf ("* Closing fd %d\n", fd);
        fd = close (fd);
        if (fd != 0)
        {
            printf ("fd = %d, errno = %d, strerror = %s\n", fd, errno,
                    strerror (errno));
        }
    }
    else
    {
        printf ("* Couldn't open file '%s' (%s)\n", payload, strerror (errno));
    }

    return 0;
}
