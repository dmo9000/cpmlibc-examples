#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <tty.h>
#include "cpmbdos.h"
#include "cpm_sysfunc.h"
#include "ansi_term.h"

extern bool has_hydrogen;

/* optimal chunk size is 128 for now */
#define CHUNK_SIZE 128


int
fcat (char *filename, off_t start_offset, off_t end_offset)
{
    static struct stat statbuf;
    FILE *testfile = NULL;
    static char buffer[CHUNK_SIZE];
    off_t offset = 0;
    off_t rv = 0;
    int rc = 0;
    ssize_t rd = -1;
    int fd = 0;
    off_t i = 0;
    int saved_errno = 0;
    size_t expected_size = 0;
    char finalchar = 0;
    if (filename[0] == '\0')
    {
        printf ("usage: fcat <filename>\n");
        return 1;
    }

    memset (&buffer, 0x00, CHUNK_SIZE);

    //rc = stat ((const char *) filename, &statbuf);
    printf("rc = %d, statbuf.st_size = %lu\n", rc, statbuf.st_size);
    expected_size = statbuf.st_size;

    if (end_offset == 0) {
            end_offset = expected_size;
            }

	printf("start offset = %lu, end offset = %lu\n", start_offset, end_offset);
    testfile = fopen ((const char *) filename, "rb");
    if (testfile)
    {
  //      printf ("* Opened '%s' FILE*=0x%04x\n", filename, testfile);
  //      printf ("---\n");

        rc = fseek(testfile, 0, SEEK_SET);
        if (rc == -1) {
            printf ("\n + ... A:fseek error:offset = %ld, FILE*= 0x%04x, rc = %d, fd = %d, buffer = 0x%04x,\n\terrno = %d, strerror = %s\n",
                    offset, testfile, rc, testfile->_file, &buffer, errno, strerror (errno));
            goto close_filehandle;
        }

        rd = fread (&buffer, CHUNK_SIZE, 1, testfile);
       // printf("fread rd = %d\n", rd);
			//	printf("end_offset = %ld\n", end_offset);

        while (rd > 0)
        {
            for (i = 0; i < CHUNK_SIZE * rd ; i++)
            {
                if (buffer[i] == EOF) break;

                //printf("%u:%u\n", offset +i, end_offset);

                if (offset + i <= end_offset) {
                    cpm_putchar(buffer[i]);
                    finalchar = buffer[i];
                } else {
                    /* reached end offset */
                    //printf("reached end offset = %lu, lastbyte=[0x%02x]\n", end_offset, finalchar);
                    goto close_filehandle; 
                }
            }

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
            saved_errno = errno;
            if (feof(testfile)) {
//                printf("\n%s[EOF]%s\n", TTY_FOREGROUND_RED, TTY_FOREGROUND_WHITE);
            } else {
                printf
                ("fread error:rd = %ld, buffer = 0x%04x, errno = %d, strerror = %s\n",
                 rd, &buffer, saved_errno, strerror (saved_errno));
            }
            goto close_filehandle;
            break;
        case -1:
            /* should not be here, fread() should always return greater than zero? */
            printf
            ("fread error:rd = %ld, buffer = 0x%04x, errno = %d, strerror = %s\n",
             rd, &buffer, errno, strerror (errno));
            goto close_filehandle;
            break;
        default:
            printf ("fread with bad length! rd = %d\n", rd);
            goto close_filehandle;
            break;
        }



close_filehandle:

 //                 printf("reached end offset = %lu, lastbyte=[0x%02x]\n", end_offset, finalchar);
 //       printf ("\n---\n");
 //       printf ("* Closing FILE*= 0x%04x\n", testfile, fd);
        fd = fclose (testfile);
        if (fd != 0)
        {
            printf ("fclose:fh=0x%04x, errno = %d, strerror = %s\n", testfile, fd, errno,
                    strerror (errno));
        }
    }
    else
    {
        printf ("cat: %s: %s\n", filename, strerror (errno));
    }

    return 0;
}
