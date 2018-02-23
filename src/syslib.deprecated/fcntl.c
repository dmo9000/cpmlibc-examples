#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "cpmbdos.h"
#include "cpm_sysfunc.h"
#include "ansi_term.h"
#include "tty.h"

/*
		If you are writing an emulator at BDOS level, you need to be aware of how CP/M uses the bytes EX, S2, and CR. Some programs (such as the Digital Research linker, 
		LINK.COM) manipulate these bytes to perform "seek" operations in files without using the random-access calls.

			CR = current record,   ie (file pointer % 16384)  / 128
			EX = current extent,   ie (file pointer % 524288) / 16384
			S2 = extent high byte, ie (file pointer / 524288). The CP/M Plus source code refers to this use of the S2 byte as 'module number'.
*/

/* errno should be set appropriately */
static uint8_t dma_buffer[SSIZE_MAX];
#define FILES_BASE				 	0x03
#define FILES_MAX  					8
#define MODULE_SIZE 				524288
#define EXTENTS_PER_MODULE	32

typedef struct {
    int id;
    int oflags;
//    ssize_t offset;
    uint32_t offset;
//    int32_t offset;
    FCB fcb;
} _cfd;

FILE filehandles[FILES_MAX];

static _cfd CFD[FILES_MAX];
bool _fds_init_done = false;
int  _find_free_fd();
int  _find_free_filehandle();
void _fds_init();

#define KILOBYTE    1024
#define EXTENT_SIZE (16 * KILOBYTE)

void _fds_init()
{
    int i = 0;
//    printf("_fds_init()\n");
    for (i = 0; i < FILES_MAX; i++) {
        //fds[i] = -1;
        memset(&CFD[i], 0, sizeof(_cfd));
        CFD[i].id = -1;
        /* setup filehandles */
        filehandles[i]._file = -1;
    }
    _fds_init_done = true;
    return;
}

char * _print_fcb(FCB *fcb_ptr, bool brief) {
    int i = 0;
    static char fbuffer[64];
    char *p = (char *) &fbuffer;
    memset(&fbuffer, 0, 64);
    printf("%s", TTY_FOREGROUND_RED);
    if (brief) {
        sprintf((const char *) &fbuffer, "[%c%c%c%c%c%c%c%c.%c%c%c]", fcb_ptr->filename[0], fcb_ptr->filename[1], fcb_ptr->filename[2], fcb_ptr->filename[3], fcb_ptr->filename[4],
                fcb_ptr->filename[5], fcb_ptr->filename[6], fcb_ptr->filename[7],
                fcb_ptr->filetype[0], fcb_ptr->filetype[1], fcb_ptr->filetype[2]);
        printf("%s", TTY_FOREGROUND_WHITE);
        return (char*) p;
    }

    printf("\n");
    printf("\tdrive ->\t%u\n", fcb_ptr->drive);
    printf("\tname  ->\t%c%c%c%c%c%c%c%c\n", fcb_ptr->filename[0], fcb_ptr->filename[1], fcb_ptr->filename[2], fcb_ptr->filename[3], fcb_ptr->filename[4], fcb_ptr->filename[5], fcb_ptr->filename[6], fcb_ptr->filename[7]);
    printf("\ttype  ->\t%c%c%c\n", fcb_ptr->filetype[0], fcb_ptr->filetype[1], fcb_ptr->filetype[2]);
    printf("\t  ex  ->\t%02X\n",fcb_ptr->ex);
    printf("\tresv  ->\t%04X\n",fcb_ptr->resv);
    printf("\t  rc  ->\t%02X\n",fcb_ptr->rc);
    printf("\talb0  ->\t%04X %04X %04X %04X\n", fcb_ptr->alb[0], fcb_ptr->alb[1], fcb_ptr->alb[2], fcb_ptr->alb[3]);
    printf("\talb1  ->\t%04X %04X %04X %04X\n", fcb_ptr->alb[4], fcb_ptr->alb[5], fcb_ptr->alb[6], fcb_ptr->alb[7]);
    printf("\tsreq  ->\t%02X\n",fcb_ptr->seqreq);
    printf("\trrec  ->\t%04X\n",fcb_ptr->rrec);
    printf("\trreo  ->\t%02X\n\n",fcb_ptr->rrecob);

    printf("%s", TTY_FOREGROUND_WHITE);
    return (char*) p;
}

int  _find_free_fd()
{
    int i = 0;
    if (!_fds_init_done) {
        _fds_init();
    }
    for (i = FILES_BASE; i < FILES_MAX; i++) {
        if (CFD[i].id == -1) {
            return i;
        }
    }
    return -1;
}

int  _find_free_filehandle()
{
    int i = 0;
    if (!_fds_init_done) {
        _fds_init();
    }

    for (i = 0; i < FILES_MAX; i++) {
        if (filehandles[i]._file == -1) {
            return i;
        }
    }
    return -1;
}


int stat(char *pathname, struct stat *buf)
{

    static FCB statfcb;
    FCB *fcb_ptr = NULL;
    char _filename[9];
    char _filetype[4];
    int length = 0;
    int rval = 0;
    char *ptr = NULL;
    uint8_t current_extent = 0; /* 16K block index */
		uint8_t module_number = 0; /* 512K block index */
    uint16_t num_records = 0; /* number of 128 byte blocks */
    int i = 0;
    bool completed = false;

    if (!_fds_init_done) {
        _fds_init();
    }
    memset(&_filename, 0, 9);
    memset(&_filetype, 0, 4);
    ptr = strchr(pathname, '.');

    length = ptr - pathname;
    if (length > 8) {
        errno = ENOENT;
        return -1;
    }
    strncpy(_filename, pathname, length);
    length = strlen(ptr+1);
    if (length > 3) {
        errno = ENOENT;
        return -1;
    }
    strncpy(_filetype, ptr+1, ( length < 4 ? length : 3));

    while (completed != true) {
        fcb_ptr = (FCB*) &statfcb;
        cpm_setFCBname(_filename, _filetype, (FCB*) fcb_ptr);
        fcb_ptr->ex = (current_extent % EXTENTS_PER_MODULE);
        //fcb_ptr->ex = (current_extent);
				module_number = (uint8_t) (current_extent / EXTENTS_PER_MODULE);  
				//fcb_ptr->resv = (0x80 +  module_number) << 8;
				//fcb_ptr->resv =0x8080 +  module_number;
			if (module_number) {
				fcb_ptr->resv = 0x8081;
				} else {
				fcb_ptr->resv = 0x8080;
				}
				printf("offset = %lu\n", (uint32_t) ((num_records * 128)));
				printf("extent = %u (%u)\n", (current_extent % EXTENTS_PER_MODULE), current_extent);
				printf("module_number = %02x\n", module_number);
				printf("fcp_ptr->resv = 0x%04x\n", fcb_ptr->resv);
        rval = cpm_performFileOp(fop_open, fcb_ptr);
				printf("stat(%d, %d)\n", rval, fcb_ptr->rc);
        _print_fcb(fcb_ptr, false);

				if (module_number > 1) {
						printf("module number = %d (too large, limit 1M)\n", module_number);
						goto done_counting;
						}	

        switch(rval) {
        case 0xFF:
            errno = ENOENT;
            return -1;
            break;
				case 0:
				case 1:
				case 2:
				case 3:
            /* examine the FCB in more detail */
            //_print_fcb(fcb_ptr, false);
						switch (fcb_ptr->rc) {
							case 0x00:
								/* no blocks in extent - we are finished */
								printf("fcb_ptr->rc = %d, rval = %d\n", fcb_ptr->rc, rval);
								printf("(empty extent?)\n");
								goto done_counting;
								break;
							default:
								printf("fcb_ptr->rc = %d, rval = %d\n", fcb_ptr->rc, rval);
                printf("%s", TTY_FOREGROUND_PURPLE);
                num_records += fcb_ptr->rc;
                printf("[multi-extent %u file:%u blocks (so far)]\n", current_extent, num_records);
                current_extent++;
                printf("%s", TTY_FOREGROUND_WHITE);
								break;

            }
            break;
					default:
						printf("unhandled, rval = %d\n", rval);
						exit(1);
						break;
        }
    }

done_counting:
    printf("[records=%u, extents=%u, size=%lu bytes]\n", num_records, current_extent, (uint32_t) num_records * 128);
    buf->st_mode = 0;
    buf->st_mode |= S_IFREG;
    buf->st_mode |= S_IREAD;
    buf->st_mode |= S_IWRITE;
    buf->st_atime = 0;
    buf->st_mtime = 0;

    //buf->st_size = ((num_records * 128) / 1024) + ((((num_records * 128) % 1024) ? 1 : 0)) * 1024;
    buf->st_size = (uint32_t) num_records * 128;
    errno = 0x0;
    return 0;

}

int open(const char *pathname, int flags)
{
    /* errno should be set appropriately */
    char _filename[9];
    char _filetype[4];
    char *ptr = NULL;
    int rval = 0;
    int fd = 0;
    FCB *fcb_ptr = NULL;
    int i = 0;
    char *filename = NULL;
    char *filetype = NULL;
    int length = 0;

    if (!_fds_init_done) {
        _fds_init();
    }

    memset(&_filename, 0, 9);
    memset(&_filetype, 0, 4);

    ptr = strchr(pathname, '.');

    length = ptr - pathname;
    if (length > 8) {
        errno = ENOENT;
        return -1;
    }
    strncpy(_filename, pathname, length);
    length = strlen(ptr+1);
    if (length > 3) {
        errno = ENOENT;
        return -1;
    }
    strncpy(_filetype, ptr+1, ( length < 4 ? length : 3));

//    printf("open: filename=[%s], filetype=[%s]\n",
//               _filename, _filetype);


    fd = (int) _find_free_fd();

    if (fd < -1 || fd >= FILES_MAX) {
        return -1;
    }

    if (fd != -1) {
        fcb_ptr = (FCB*) &CFD[fd].fcb;
        // printf("[writing FCB at fd slot %d:0x%04x:0x%04x (%d bytes)]\n", fd, &fds[fd], fcb_ptr, sizeof(FCB));
        cpm_setFCBname(_filename, _filetype, (FCB*) fcb_ptr);
        rval = cpm_performFileOp(fop_open, fcb_ptr);
        //printf(" ret.val %02X\n", rval);

        if (rval != 0xFF) {
            //printf("File found, reading the FCB!\n");
            errno = 0;
            goto return_valid_fd;
        } else {
            if (((flags & O_RDWR) || (flags & O_WRONLY)) && (flags & O_TRUNC)) {
                //printf("fcntl: didn't exist but write was requested and O_TRUNC was set\n");
                /* create new file here */
                rval = cpm_performFileOp(fop_makeFile, fcb_ptr);
                //printf("create rval = %d\n", rval);
                if (rval == 0xFF) {
                    errno = EIO;
                    return -1;
                }

                rval = cpm_performFileOp(fop_open, fcb_ptr);
                /* fail if we can't open the file after creating it */

                if (rval == 0xFF) {
                    errno = EIO;
                    return -1;
                }

                goto return_valid_fd;
            }
            errno = ENOENT;
            return -1;
        }
    } else {
        errno = ENFILE;
        return -1;
    }

return_valid_fd:
    CFD[fd].id = fd;
    CFD[fd].oflags = flags;
    CFD[fd].offset = 0x0000;
    fcb_ptr->seqreq = 0x00;
    fcb_ptr->rrec = 0x0000;
    fcb_ptr->rrecob = 0x00;
    errno = 0;
    return fd;

}

int close(int fd)
{

    FCB *fcb_ptr = NULL;
    int rval = 0 ;
    if (!_fds_init_done) {
        _fds_init();
    }

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }
    /* errno should be set appropriately */
    if (CFD[fd].id != -1) {
        fcb_ptr = &CFD[fd].fcb;
//        printf("%s [closing FCB at %d:0x%04x (%d)]\n", _print_fcb(fcb_ptr, true), fd, fcb_ptr, sizeof(FCB), _print_fcb(fcb_ptr, true));
        //_print_fcb(fcb_ptr, true);
        rval = cpm_performFileOp(fop_close, fcb_ptr);
        //printf(" close->ret.val %02X\n", rval);
        //fds[fd] = -1;
        CFD[fd].id = -1;
        errno = 0;
        return 0;
    }
    errno = EBADF;
    return -1;
}

ssize_t write(int fd, void *buf, size_t count)
{
    FCB *fcb_ptr = NULL;
    uint16_t ret_ba, ret_hl;
    int rval = 0;
    int cpm_err = 0;
    int wr =0;
    int start_offset = 0;
    int limit = SSIZE_MAX;
    uint16_t required_module = 0;
    uint16_t required_extent = 0;
    uint16_t required_block = 0;

    errno =0;

    if (!_fds_init_done) {
        _fds_init();
    }
    if ((fd < 0 || fd >= FILES_MAX) || CFD[fd].id == -1) {
        errno = EBADF;
        return -1;
    }

    /* fd is validated */

    if (count > SSIZE_MAX) {
        /* apparently behaviour is supposed to be undefined here, but we will return I/O error */
        printf("Xcount = %d, SSIZE_MAX = %d\n", count, SSIZE_MAX);
        errno = EIO;
        return -1;
    }

    fcb_ptr = (FCB*) &CFD[fd].fcb;
    if (CFD[fd].offset % SSIZE_MAX) {
        start_offset = CFD[fd].offset % SSIZE_MAX;
        limit -= start_offset;
    }

//    required_extent = (uint16_t) ((uint16_t) CFD[fd].offset / (uint16_t) EXTENT_SIZE);
	  required_module = (CFD[fd].offset / 524288);
    required_extent = (CFD[fd].offset / 16384);
    required_block = (CFD[fd].offset / 128);
    required_block -= (required_extent * 0x80);

    if (fcb_ptr->ex != required_extent) {
        fcb_ptr->ex = required_extent;
        rval = cpm_performFileOp(fop_open, fcb_ptr);
    }

    /*
    if (fcb_ptr->ex >= 512) {
            printf("whoah silver! how did we end up in this situation? fcb_ptr->ex = %d\n", fcb_ptr->ex);
            printf("That is pretty big for a CP/M file.\n");
            exit(1);
            }
    */

    if (fcb_ptr->seqreq != required_block) {
        fcb_ptr->seqreq = required_block;
    }

//    printf("required extent = %d, required_block = %d\n", required_extent, required_block);
    fcb_ptr->rrec = 0x0000;
    fcb_ptr->rrecob = 0x00;

    /* careful, we are setting the DMA address straight into the buffer here */

    cpm_setDMAAddr((uint16_t) buf);
    rval = cpm_performFileOp(fop_writeSeqRecord, fcb_ptr);
    ret_ba = get_ret_ba();
    ret_hl = get_ret_hl();

//#define DEBUG_LIBCIO_WRITE
#ifdef DEBUG_LIBCIO_WRITE
    printf("%s", TTY_FOREGROUND_RED);
    printf("\r\nwrite ret.val=%02X, ret_ba=0x%04x ret_hl=0x%04x\n", rval, ret_ba, ret_hl);
    printf("\toffset ->\t%lu\n", CFD[fd].offset); 
    printf("\tex     ->\t%02X\n",fcb_ptr->ex);
    printf("\trc     ->\t%02X\n",fcb_ptr->rc);
    printf("\tsreq   ->\t%02X\n",fcb_ptr->seqreq);
    printf("%s", TTY_FOREGROUND_WHITE);
#endif /* DEBUG_LIBCIO_WRITE */


    if (rval != 0) {
        cpm_err = (ret_ba & 0xff00) >> 8;
        if (fcb_ptr->rrec > 0x00ff) {
            printf("\n# WRITE_FAIL: ret.val=%02X, ret_ba=0x%04x ret_hl=0x%04x, CPM_ERR=%d\n", rval, ret_ba, ret_hl, cpm_err);
            printf("#\tsreq ->\t%02X\n",fcb_ptr->seqreq);
            printf("#\trrec ->\t%04X\n",fcb_ptr->rrec);
            printf("#\trreo ->\t%02X\n\n",fcb_ptr->rrecob);
        }

        switch(cpm_err) {
        case 0x01:
            /* end of file - return 0 to caller, clear errno */
            errno = 0;
            return 0;
            break;
        }
        printf("cpm_err=%d\n", cpm_err);
        /* something bad happened? */
        errno = EIO;
        return -1;
    }

    rval = cpm_performFileOp(fop_updRandRecPtr, fcb_ptr);


    /* if we requested more bytes than are available, just copy those and return the value */
    if (count < limit) {
        limit = count;
    }

    /* close the file */

    rval = cpm_performFileOp(fop_close, fcb_ptr);

    /* TODO: return the number of bytes actually read */
    errno = 0;
    return limit;
}

ssize_t read(int fd, void *buf, size_t count)
{
    FCB *fcb_ptr = NULL;
    uint16_t ret_ba, ret_hl;
    int rval = 0;
    int cpm_err = 0;
    int rd = 0;
    int start_offset = 0;
    int limit = SSIZE_MAX;
		uint16_t required_module = 0;
    uint16_t required_extent = 0;
    uint16_t required_block = 0;
    if (!_fds_init_done) {
        _fds_init();
    }
    //printf("read(%d, 0x%04x, %u)\n", fd, buf, count);

    if ((fd < 0 || fd >= FILES_MAX) || CFD[fd].id == -1) {
        errno = EBADF;
        return -1;
    }

    if (count > SSIZE_MAX) {
        /* apparently behaviour is supposed to be undefined here, but we will return I/O error */
        errno = EIO;
        return -1;
    }
    fcb_ptr = (FCB*) &CFD[fd].fcb;

    if (CFD[fd].offset % SSIZE_MAX) {
        start_offset = CFD[fd].offset % SSIZE_MAX;
        limit -= start_offset;
    }

    required_module = (CFD[fd].offset / 524288);
    required_extent = (CFD[fd].offset / 16384);
    required_block = (CFD[fd].offset / 128);
    required_block -= (required_extent * 0x80);

    if (fcb_ptr->ex != required_extent) {
        fcb_ptr->ex = required_extent;
        rval = cpm_performFileOp(fop_open, fcb_ptr);
    }

    /*
       if (fcb_ptr->ex >= 512) {
            printf("whoah silver! how did we end up in this situation? fcb_ptr->ex = %d\n", fcb_ptr->ex);
            printf("That is pretty big for a CP/M file.\n");
            exit(1);
            }
    */

    if (fcb_ptr->seqreq != required_block) {
        fcb_ptr->seqreq = required_block;
    }

    fcb_ptr->rrec = 0x0000;
    fcb_ptr->rrecob = 0x00;
	fcb_ptr->resv = 0x8000 + (0x80 + required_module);
    cpm_setDMAAddr((uint16_t)dma_buffer);
    rval = cpm_performFileOp(fop_readSeqRecord, fcb_ptr);
    ret_ba = get_ret_ba();
    ret_hl = get_ret_hl();

//#define DEBUG_LIBCIO_READ
#ifdef DEBUG_LIBCIO_READ
    printf("%s", TTY_FOREGROUND_RED);
    printf(" read ret.val=%02X, ret_ba=0x%04x ret_hl=0x%04x\n", rval, ret_ba, ret_hl);
    printf("\ts1s2 ->\t%04X\n",fcb_ptr->resv);
    printf("\tex   ->\t%02X\n",fcb_ptr->ex);
    printf("\trc   ->\t%02X\n",fcb_ptr->rc);
    printf("\tsreq ->\t%02X\n",fcb_ptr->seqreq);
    printf("%s", TTY_FOREGROUND_WHITE);
#endif /* DEBUG_LIBCIO_READ */

    if (rval != 0) {
        cpm_err = (ret_ba & 0xff00) >> 8;
        if (fcb_ptr->rrec > 0x00ff) {
            printf("\n# READ_FAIL: ret.val=%02X, ret_ba=0x%04x ret_hl=0x%04x, CPM_ERR=%d\n", rval, ret_ba, ret_hl, cpm_err);
            printf("#\tsreq ->\t%02X\n",fcb_ptr->seqreq);
            printf("#\trrec ->\t%04X\n",fcb_ptr->rrec);
            printf("#\trreo ->\t%02X\n\n",fcb_ptr->rrecob);
        }

        switch(cpm_err) {
        case 0x01:
            /* end of file - return 0 to caller, clear errno */
						printf("/* read() hit EOF */\n");
            errno = 0;
            return 0;
            break;
        }

        /* something bad happened? */
        errno = EIO;
        return -1;
    }

    /* update RR record */

    rval = cpm_performFileOp(fop_updRandRecPtr, fcb_ptr);

    /* if we requested more bytes than are available, just copy those and return the value */
    if (count < limit) {
        limit = count;
    }

    /* copy to target buffer - TODO - see if we can work zero copy in here */
    memcpy(buf, &dma_buffer + start_offset, limit);
    /* TODO: return the number of bytes actually read */

    CFD[fd].offset += limit;

    errno = 0;
    
    return limit;
}

off_t lseek(int fd, off_t offset, int whence)
{

    if (!_fds_init_done) {
        _fds_init();
    }

    /* here we just verify offsets and tweak the pointer around. the real magic happens in read/write */

    //printf("\n+lseek(%d, %u, %d)\n", fd, offset, whence);

    if (CFD[fd].id == -1) {
        errno = EBADF;
        return -1;
    }
    if (whence == SEEK_SET) {
        CFD[fd].offset = (uint32_t) offset;
        errno = 0;
        return 0;
    }


    errno = EINVAL;
    return -1;
}


FILE *fopen(const char *path, const char *mode)
{
    static struct stat statbuf;
    int fd = -1;
    int fh = -1;
    int oflags = 0;
    FILE *myfhptr = NULL;
    ssize_t initial_size = 0;
    if (!_fds_init_done) {
        _fds_init();
    }

    if (strncmp(mode, "r", 1) == 0) {
        oflags = O_RDONLY;
    }

    if (strncmp(mode, "w", 1) == 0) {
        oflags = O_WRONLY | O_TRUNC;
    }

    if (strncmp(mode, "r+", 2) == 0) {
        oflags = O_RDWR;
    }

    if (strncmp(mode, "w+", 2) == 0) {
        oflags = O_RDWR | O_TRUNC;
    }


		/*
    if (stat ((const char *) path, &statbuf) == 0) {
        // save the size of the file if it is available to th
				// stream->_limit field 
        initial_size = statbuf.st_size;
        } else {
        initial_size = 0;
        }
   	*/ 

    errno = 0;
    fd = open(path, oflags);

    if (fd < 0) {
        /* pass through errno */
        return NULL;
    }
    errno = 0;
    /* get free filehandle */
    fh = _find_free_filehandle();
    if (fh == -1) {
        close(fd);
        errno = ENFILE;
        return NULL;
    }



    errno = 0;
    filehandles[fh]._file = fd;
    filehandles[fh]._eof = false;
    filehandles[fh]._limit = initial_size;
    memset(&filehandles[fh]._flags, 0, 4);
    strncpy((const char *) &filehandles[fh]._flags, (const char *) mode, 3);
    myfhptr = &filehandles[fh];
    return (FILE*) myfhptr;

}

int fclose(FILE *stream)
{

    if (! stream || stream->_file == -1) {
        errno = EBADF;
        return -1;
    }
    stream->_file = -1;
    errno = 0;
    return 0;
}

int fseek(FILE *stream, long offset, int whence)
{
    if (! stream || stream->_file == -1) {
        errno = EBADF;
        return -1;
    }

  //printf("\r\n+fseek(FILE=0x%04x[%d], %ld, %d)\n", stream, stream->_file, offset, whence);

    /* success */

    if (lseek(stream->_file, offset, whence) == -1) {
        /* pass through errno from lseek() */
        return -1;
    }

    errno = 0;
    return 0;

}

size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int i = 0;
    ssize_t wr = 0;
    char *myptr = (char *) ptr;
    if (! stream || stream->_file == -1) {
        errno = EBADF;
        return -1;
    }

    for (i = 0; i < nmemb; i++) {
        wr = write(stream->_file, myptr, size);
        if (wr == 0) {
            /* END OF FILE REACHED */
            stream->_eof = true;
        }


        if (wr == -1) {
            /* pass through errno from write() */
            return 0;
        }
    }

    return i;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    ssize_t ritems = 0;
    int i = 0;
    size_t bytes = 0;
    int rd = 0;
    char *myptr = (char*) ptr;
    char *eofptr = NULL;
    if (! stream || stream->_file == -1) {
        errno = EBADF;
        return -1;
    }

    for (i = 0; i < nmemb; i++) {
        rd = read(stream->_file, myptr, size);
        if (rd == 0) {
            /* END OF FILE REACHED */
            stream->_eof = true;
            //printf("HARD EOF REACHED\n");
            //exit(1);
            if (ftell(stream) != stream->_limit) {
                printf("HARD EOF REACHED, BUT LIMIT IS MISMATCHED\n");
                } 
        }

        if (rd == -1) {
            printf("fread() error; errno = %d [%s]\n", errno, strerror(errno));
            /* fread() returns 0 on error (or a short item count) and leaves it to the caller to determine what happened */
            return(0);
        }

        if (rd == size) {
            myptr += rd;
            ritems++;
        } else {
            if (rd < size) {
                errno = EIO;
                return ritems;
            }
            errno = EIO;
            return ritems;
        }
    }

    /* before returning, check the 'b' flag on the FILE. if it is not set, we need to search for the EOF marker in this block */


    if (memchr(&stream->_flags, 'b', 3) == NULL) {
        /* if the file was not opened in binary mode, we should respect that character 0x1A is the EOF marker */
        eofptr = memchr((const char *) ptr, 0x1A, size * nmemb);
        if (eofptr) {
            stream->_limit = CFD[stream->_file].offset + (eofptr - ptr);  
            stream->_eof = true;
            //CFD[stream->_file].offset = stream->_limit;
            //printf("current offset = %lu\n", CFD[stream->_file].offset); 
            //`printf("rewind = %d\n", (SSIZE_MAX - (eofptr - ptr)));
            CFD[stream->_file].offset -= (SSIZE_MAX - (eofptr - ptr));
            stream->_limit = CFD[stream->_file].offset;           
        } else {
        }
    } 


    errno = 0;
    return ritems;
}

int feof(FILE *stream)
{
    errno = 0;
    if (! stream || stream->_file == -1) {
        errno = EBADF;
        return -1;
    }
    if (stream->_eof) {
        return 1;
    }
    return 0;
}

long ftell(FILE *stream)
{

    return CFD[stream->_file].offset;

}
