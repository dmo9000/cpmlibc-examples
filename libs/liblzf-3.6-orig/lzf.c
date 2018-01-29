/*
 * Copyright (c) 2006      Stefan Traby <stefan@hello-penguin.com>
 *
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 *
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License ("GPL") version 2 or any later version,
 * in which case the provisions of the GPL are applicable instead of
 * the above. If you wish to allow the use of your version of this file
 * only under the terms of the GPL and not to allow others to use your
 * version of this file under the BSD license, indicate your decision
 * by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete the
 * provisions above, a recipient may use your version of this file under
 * either the BSD or the GPL.
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include "lzf.h"

#define fprintf(a,...) 	printf(__VA_ARGS__);
#define HAVE_GETOPT_H

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

//#define BLOCKSIZE (1024 * 64 - 1)
#define BLOCKSIZE 128
#define MAX_BLOCKSIZE BLOCKSIZE


typedef unsigned char u8;

static off_t nr_read, nr_written;

const static char *imagename = "lzf";
static enum { compress, uncompress, lzcat } mode = compress;
static int verbose = 0;
static int force = 0;
//const static long blocksize = BLOCKSIZE;
static long blocksize = 128;
static char *my_oname = NULL;

static const char *opt =
    "-c   compress\n"
    "-d   decompress\n"
    "-f   force overwrite of output file\n"
    "-h   give this help\n"
    "-v   verbose mode\n"
    "-b # set blocksize\n"
    "\n";

static void
usage (int rc)
{
    printf ("\n"
            "lzf, a very lightweight compression/decompression utility written by Stefan Traby.\n"
            "uses liblzf written by Marc Lehmann <schmorp@schmorp.de> You can find more info at\n"
            "http://liblzf.plan9.de/\n"
            "\n"
            "usage: lzf [-dufhovb] [file ...]\n"
            "       unlzf [file ...]\n"
            "       lzcat [file ...]\n"
            "\n%s",
            (const char *) opt);

    exit (rc);
}

static inline ssize_t
rread (int fd, void *buf, size_t len)
{
    ssize_t rc = 0, offset = 0;
    char *p = buf;
    printf("rread(%d, 0x%04x, %d)\n", fd, buf, len);

    while (len && (rc = read (fd, &p[offset], len)) > 0)
    {
        printf("read = %d\n", rc);
        offset += rc;
        len -= rc;
    }

    nr_read += offset;
    printf("nr_read = %ld\n", nr_read);

    if (rc < 0)
        return rc;

    return offset;
}

/* returns 0 if all written else -1 */
static inline ssize_t
wwrite (int fd, void *buf, size_t len)
{
    ssize_t rc;
    char *b = buf;
    size_t l = len;

    printf("wwrite(%ld)\n", fd);
    printf("len = %u\n", len);
    while (l)
    {

        rc = write (fd, b, l);
        if (rc < 0)
        {
            fprintf (stderr, "lzf: write error: ");
            perror ("foo");
            return -1;
        }

        l -= rc;
        b += rc;
    }

    nr_written += len;
    return 0;
}

/*
 * Anatomy: an lzf file consists of any number of blocks in the following format:
 *
 * \x00   EOF (optional)
 * "ZV\0" 2-byte-usize <uncompressed data>
 * "ZV\1" 2-byte-csize 2-byte-usize <compressed data>
 * "ZV\2" 4-byte-crc32-0xdebb20e3 (NYI)
 */


#define TYPE0_HDR_SIZE 5
#define TYPE1_HDR_SIZE 7
#define MAX_HDR_SIZE 7
#define MIN_HDR_SIZE 5

static int
compress_fd (int from, int to)
{
    ssize_t us, cs, len;
    u8 buf1[MAX_BLOCKSIZE + MAX_HDR_SIZE + 16];
    u8 buf2[MAX_BLOCKSIZE + MAX_HDR_SIZE + 16];
    u8 *header;

    //dprintf("compress_fd - from %d -> %d\n", from, to);
    printf("compress_fd(%d)\n", from);
    printf("dest = %d\n", to);

    nr_read = nr_written = 0;
    printf("blocksize = %d\n", blocksize);
    while ((us = rread (from, &buf1[MAX_HDR_SIZE], blocksize)) > 0)
    {
        cs = lzf_compress (&buf1[MAX_HDR_SIZE], us, &buf2[MAX_HDR_SIZE], us > 4 ? us - 4 : us);
        if (cs)
        {
            header = &buf2[MAX_HDR_SIZE - TYPE1_HDR_SIZE];
            header[0] = 'Z';
            header[1] = 'V';
            header[2] = 1;
            header[3] = cs >> 8;
            header[4] = cs & 0xff;
            header[5] = us >> 8;
            header[6] = us & 0xff;
            len = cs + TYPE1_HDR_SIZE;
        }
        else
        {   // write uncompressed
            header = &buf1[MAX_HDR_SIZE - TYPE0_HDR_SIZE];
            header[0] = 'Z';
            header[1] = 'V';
            header[2] = 0;
            header[3] = us >> 8;
            header[4] = us & 0xff;
            len = us + TYPE0_HDR_SIZE;
        }

        if (wwrite (to, header, len) == -1)
            return -1;
    }

    return 0;
}

static int
uncompress_fd (int from, int to)
{
    u8 header[MAX_HDR_SIZE];
    u8 buf1[MAX_BLOCKSIZE + MAX_HDR_SIZE + 16];
    u8 buf2[MAX_BLOCKSIZE + MAX_HDR_SIZE + 16];
    u8 *p;
    int l, rd;
    ssize_t rc, cs, us, bytes, over = 0;

    nr_read = nr_written = 0;
    while (1)
    {
        rc = rread (from, header + over, MAX_HDR_SIZE - over);
        if (rc < 0)
        {
            fprintf (stderr, "lzf: read error: ");
            perror ("bar");
            return -1;
        }

        rc += over;
        over = 0;
        if (!rc || header[0] == 0)
            return 0;

        if (rc < MIN_HDR_SIZE || header[0] != 'Z' || header[1] != 'V')
        {
            fprintf (stderr, "lzf: invalid data stream - magic not found or short header\n");
            return -1;
        }

        switch (header[2])
        {
        case 0:
            cs = -1;
            us = (header[3] << 8) | header[4];
            p = &header[TYPE0_HDR_SIZE];
            break;
        case 1:
            if (rc < TYPE1_HDR_SIZE)
            {
                goto short_read;
            }
            cs = (header[3] << 8) | header[4];
            us = (header[5] << 8) | header[6];
            p = &header[TYPE1_HDR_SIZE];
            break;
        default:
            fprintf (stderr, "lzf: unknown blocktype\n");
            return -1;
        }

        bytes = cs == -1 ? us : cs;
        l = &header[rc] - p;

        if (l > 0)
            memcpy (buf1, p, l);

        if (l > bytes)
        {
            over = l - bytes;
            memmove (header, &p[bytes], over);
        }

        p = &buf1[l];
        rd = bytes - l;
        if (rd > 0)
            if ((rc = rread (from, p, rd)) != rd)
                goto short_read;

        if (cs == -1)
        {
            if (wwrite (to, buf1, us))
                return -1;
        }
        else
        {
            if (lzf_decompress (buf1, cs, buf2, us) != us)
            {
                fprintf (stderr, "%s: decompress: invalid stream - data corrupted\n", imagename);
                return -1;
            }

            if (wwrite (to, buf2, us))
                return -1;
        }
    }

    return 0;

short_read:
    fprintf (stderr, "%s: short data\n", imagename);
    return -1;
}

static int
open_out (const char *name)
{
    int fd;
    int m = O_EXCL;

    if (force)
        m = 0;

    // this overload not supported by cpmlibc
    //fd = open (name, O_CREAT | O_WRONLY | O_TRUNC | m, 600);
//  fd = open (name, O_CREAT | O_WRONLY | O_TRUNC | m);
    fd = open (name, O_WRONLY | O_TRUNC);

    printf("open_out(%s) = %d\n", name, fd);

    /* We might want to use this for __CPM__ too */
#if defined(__MINGW32__)
    _setmode(fd, _O_BINARY);
#endif
    return fd;
}

static int
compose_name (const char *fname, char *oname)
{
    char *p;

    if (mode == compress)
    {
        if (strlen (fname) > PATH_MAX - 4)
        {
            printf ("%s: %s.lzf: name too long", (const char *) imagename, fname);
            return -1;
        }

        strcpy (oname, fname);
        strcat (oname, ".lzf");
    }
    else
    {
        if (strlen (fname) > PATH_MAX)
        {
            fprintf (stderr, "%s: %s: name too long\n", (const char *) imagename, fname);
            return -1;
        }

        strcpy (oname, fname);
        p = &oname[strlen (oname)] - 4;
        if (p < oname || strcmp (p, ".lzf"))
        {
            fprintf (stderr, "%s: %s: unknown suffix\n", (const char *)  imagename, fname);
            return -1;
        }

        *p = 0;
    }

    return 0;
}

static int
run_file (const char *fname)
{
    int fd, fd2;
    int rc;
    struct stat mystat;
    char oname[PATH_MAX + 1];

    if (mode != lzcat) {
        if (!oname) {
            printf("lzf: output filename must be specified on CP/M\n");
            return -1;
        } else {
            printf("output filename set to %s\n", (const char *) my_oname);
        }
    }

    memset(&oname, 0, PATH_MAX+1);
    strncpy(oname, my_oname, ( strlen(my_oname) < PATH_MAX ? strlen(my_oname) : PATH_MAX));

    rc = stat (fname, &mystat);
    fd = open (fname, O_RDONLY);
#if defined(__MINGW32__)
    _setmode(fd, _O_BINARY);
#endif
    if (rc || fd == -1)
    {
        fprintf (stderr, "lzf: [%s]: ", fname);
        perror ("baz");
        return -1;
    }

    if (!S_ISREG (mystat.st_mode))
    {
        fprintf (stderr, "lzf: %s: not a regular file.\n", fname);
        close (fd);
        return -1;
    }

    if (mode == lzcat)
    {
        rc = uncompress_fd (fd, 1);
        close (fd);
        return rc;
    }

    fd2 = open_out (oname);
    if (fd2 == -1)
    {
        fprintf (stderr, "lzf: %s: ", oname);
        perror ("open_out:");
        close (fd);
        return -1;
    }

    if (mode == compress)
    {
        rc = compress_fd (fd, fd2);
        if (!rc && verbose);
        fprintf (stderr, "%s:  %u%% -- written to %s\n",
                 fname, (uint8_t) (nr_read == 0 ? 0 : 100.0 - nr_written / ((double) nr_read / 100.0)), (const char *) oname);
    }
    else
    {
        rc = uncompress_fd (fd, fd2);
        if (!rc && verbose)
            fprintf (stderr, "%s:  %u%% -- written to %s\n",
                     fname, (uint8_t) (nr_written == 0 ? 0 : 100.0 - nr_read / ((double) nr_written / 100.0)), (const char *) oname);
    }

    printf("chmod(%s)\n", oname);
    chmod (oname, mystat.st_mode);

    printf("closing fds...\n");

    close (fd);
    close (fd2);

    /* WE WON'T UNLINK ON CP/M */

    //if (!rc)
    // unlink (fname);

    return rc;
}

int
main (int argc, char *argv[])
{
    char *p = argv[0];
    int optc;
    int rc = 0;

    printf("MAX_BLOCKSIZE = %u\n", MAX_BLOCKSIZE);

    if (my_oname) {
        printf("error: my_oname was supposed to be NULL\n");
        exit(1);
    }

    blocksize = 128;

    while ((optc = getopt (argc, argv, "CDFHVO:")) != -1)
    {
        switch (optc)
        {
        case 'C':
            mode = compress;
            break;
        case 'D':
            mode = uncompress;
            break;
        case 'F':
            force = 1;
            break;
        case 'H':
            usage (0);
            break;
        case 'V':
            verbose = 1;
            break;
        case 'O':
            my_oname = (char *) optarg;
            break;
        default:
            usage (1);
            break;
        }
    }



    if (!my_oname) {
        printf("error: on CP/M systems, you need to use the -o option\n");
        exit(1);
    }

    if (optind == argc)
    {   // stdin stdout
        if (!force)
        {
            if ((mode == uncompress || mode == lzcat) && isatty (0))
            {
                fprintf (stderr, "lzf: compressed data not read from a terminal. Use -f to force decompression.\n");
                exit (1);
            }
            if (mode == compress && isatty (1))
            {
                fprintf (stderr, "lzf: compressed data not written to a terminal. Use -f to force compression.\n");
                exit (1);
            }
        }

        if (mode == compress) {
            /* compress from stdin - not going to be supported under CP/M (yet) */
            rc = compress_fd (0, 1);
        } else {
            /* decompress from stdin - not going to be supported under CP/M (yet) */
            rc = uncompress_fd (0, 1);
        }

        exit (rc ? 1 : 0);
    }

    while (optind < argc) {
        rc |= run_file (argv[optind++]);
    }
    exit (rc ? 1 : 0);
}

