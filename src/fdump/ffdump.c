#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "cpmbdos.h"
#include "cpm_sysfunc.h"
#include "ansi_term.h"

extern bool has_hydrogen;

/* optimal chunk size is 128 for now */
#define CHUNK_SIZE 16


int
ffdump (char *filename, bool paginate)
{

    FILE *testfile = NULL;
    static char buffer[CHUNK_SIZE];
    off_t address = 0;
    off_t offset = 0;
    off_t rv = 0;
    int rc = 0;
    ssize_t rd = -1;
    int fd = 0;
    int i = 0;
    int lines = 0;

    memset (&buffer, 0x00, CHUNK_SIZE);

    testfile = fopen ((const char *) filename, "rb");
    if (testfile)
    {
        //printf ("* Opened '%s' FILE*=0x%04x\n", filename, testfile);
        //printf ("---\n");

        rc = fseek(testfile, 0, SEEK_SET);
        if (rc == -1) {
            printf ("\n + ... A:fseek error:offset = %ld, FILE*= 0x%04x, rc = %d, fd = %d, buffer = 0x%04x,\n\terrno = %d, strerror = %s\n",
                    offset, testfile, rc, testfile->_file, &buffer, errno, strerror (errno));
            goto close_filehandle;
        }

        rd = fread (&buffer, CHUNK_SIZE, 1, testfile);
        //printf("fread rd = %ld\n", rd);

        while (rd > 0)
        {
            printf("%04d:0x%04x: ", lines, address);
            for (i = 0; i < CHUNK_SIZE * rd ; i++)
            {
                printf("%02x", buffer[i]);
            }

            printf(" | ");

            for (i = 0; i < CHUNK_SIZE * rd ; i++)
            {
                if (buffer[i] >= 32 && buffer[i] <=127) {
                    cpm_putchar(buffer[i]);
                } else {
                    cpm_putchar('.');
                }
            }

            printf("\n");

            lines++;
            if (paginate) {
                if (lines >= 23) {
                    if (!continue_or_exit()) {
                        exit(0);
                    }
                    printf("\n");
                    lines = 0;
                }
            }

            address += (rd * CHUNK_SIZE);
            offset += (rd * CHUNK_SIZE);
            rc = fseek(testfile, offset, SEEK_SET);

            if (rc == -1) {
                printf ("\n + ... B:fseek error:offset = %ld, FILE*= 0x%04x, rc = %d, fd = %d, buffer = 0x%04x,\n\terrno = %d, strerror = %s\n",
                        offset, testfile, rc, testfile->_file, &buffer, errno, strerror (errno));
                goto close_filehandle;
            }

            rd = fread (&buffer, CHUNK_SIZE, 1, testfile);
        }

        switch (rd)
        {
        case 0:
            /* if 0, that means no elements were read - could be end of file or error */
            if (feof(testfile)) {
//                printf("\n+[EOF]+\n");
            } else {
                printf
                ("fread error:rd = %ld, fd = %d, buffer = 0x%04x, errno = %d, strerror = %s\n",
                 rd, fd, &buffer, errno, strerror (errno));
            }
            goto close_filehandle;
            break;
        case -1:
            printf
            ("fread error:rd = %ld, fd = %d, buffer = 0x%04x, errno = %d, strerror = %s\n",
             rd, fd, &buffer, errno, strerror (errno));
            goto close_filehandle;
            break;
        default:
            printf ("fread with bad length! rd = %d\n", rd);
            goto close_filehandle;
            break;
        }



close_filehandle:
        //printf ("\n---\n");
        //printf ("* Closing FILE*= 0x%04x\n", testfile, fd);
        fd = fclose (testfile);
        if (fd != 0)
        {
            printf ("fclose:fh=0x%04x, errno = %d, strerror = %s\n", testfile, fd, errno,
                    strerror (errno));
        }
    }
    else
    {
        printf ("* Couldn't open file '%s' (%s)\n", filename, strerror (errno));
    }

    return 0;
}
