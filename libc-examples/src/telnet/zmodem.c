#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <tcp.h>
#ifdef __CPM__
#include "cpm_sysfunc.h"
#include "ansi_term.h"
#endif
#include "osdep.h"
#include "../../build-id.h"

#ifdef __CPM__
#define FILENAME_MAX    12
#endif /* __CPM__ */
#include "zmodem.h"
#include "telnet.h"
#include "tty.h"


#define BUFFER_OFFSET   6
#define ZMODEM_BUFSIZE  128

int rx_frame_type = ZMODEM_NONE;
int tx_frame_type = ZMODEM_NONE;

int CRC_MODE = 0;
#define SECTOR_SIZE         128
#define CRCMODE_NONE        0
#define CRCMODE_STREAMING   1
int ck_ctr = 0;
uint32_t crc_check = 0;

#define DO_CHECKSUM             /* checksum can be disabled for testing purposes by undefining this */


static uint32_t crc = 0xFFFFFFFF;

/*
 *  see:
 *
        http://www.zimmers.net/anonftp/pub/cbm/csbruce/crc32.c

 * for the origin of the CRC code */

//const uint32_t crcTable[256] = {
const uint32_t crc32tbl[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
    0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
    0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
    0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
    0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
    0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
    0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
    0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
    0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
    0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
    0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
    0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
    0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
    0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
    0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
    0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
    0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
    0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
    0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
    0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
    0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
    0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};




void dump_buf(const char *foo, int direction, int limit, off_t offset);
bool packet_has_xon(const char *foo, int len);
off_t write_range(off_t start, off_t end, const char * b, FILE *f);
void zmodem_safe_exit();
int blocking_tcp_recv(int fd, const char * ptr, int size);
int zmodem_data_decode(const char *b, uint8_t start, uint8_t end);
int zmodem_output_byte(uint8_t g, uint8_t o);
void exit_check();
int output_block(uint8_t len);
uint8_t get_percentage();
void reset_zmodem_state();

static char filename[FILENAME_MAX];
static off_t zm_fileLength = 0;
static off_t zm_mTime = 0;
static uint32_t zm_fileMode = 0;
static uint8_t zm_xSerial = 0;
static uint8_t zm_rFiles = 0;
static off_t zm_rBytes = 0;
static bool transfer_in_progress = false;
static FILE *outFile = NULL;
static uint16_t blocks_written = 0;
static uint16_t blocks_total = 0;

static bool telnet_flag_ff = false;
static uint8_t idx = 0;                     /* offset into current block being written */
static bool in_zdle = false;
static bool in_crc = false;

static uint8_t l = 0;                   /* current byte in data decoding buffer */
static uint8_t l1 = 0;                   /*  previous byte in data decoding buffer */
static uint8_t rzmodem[ZMODEM_BUFSIZE];
static uint8_t outputbuffer[SECTOR_SIZE];
static int checksum_bytes = 0;
static char checksum[11];
static int ckidx = 0;
static uint32_t ibytes = 0;
static uint32_t obytes = 0;
static bool crc_stop = false;
static bool final_block = false;
static int total_blocks = 0;

const char nineslice[3][9] = {
    { 0xda, 0xc4, 0xbf, 0xb3, 0x20, 0xb3, 0xc0, 0xc4, 0xd9 },
    { 0x2b, 0x2d, 0x2b, 0x7c, 0x20, 0x7c, 0x2b, 0x2d, 0x2b },
    { 0x96, 0x9a, 0x9c, 0x95, 0x20, 0x95, 0x93, 0x9a, 0x99 }
};

int drawbox(int, int, int, int);
int si = 0;


#define ANSI_GREEN  "\x1b\x5b""32m"
#define ANSI_NORMAL  "\x1b\x5b""0m"


void chomp_buffer()
{

    memmove(&rzmodem, &rzmodem+1, ZMODEM_BUFSIZE-1);
    rzmodem[ZMODEM_BUFSIZE-1] = '\0';

}

void reset_zmodem_state()
{
    memset(&filename, 0, FILENAME_MAX);
    zm_fileLength = 0;
    zm_mTime = 0;
    zm_fileMode = 0;
    zm_xSerial = 0;
    zm_rFiles = 0;
    zm_rBytes = 0;
    transfer_in_progress = false;
    outFile = NULL;
    blocks_written = 0;
    blocks_total = 0;

    telnet_flag_ff = false;
    idx = 0;                     /* offset into current block being written */
    in_zdle = false;
    in_crc = false;

    l = 0;                   /* current byte in data decoding buffer */
    l1 = 0;                   /*  previous byte in data decoding buffer */
    memset(&rzmodem, 0, ZMODEM_BUFSIZE);
    memset(&outputbuffer, 0, SECTOR_SIZE);
    checksum_bytes = 0;
    memset(&checksum, 0, 11);
    ckidx = 0;
    ibytes = 0;
    obytes = 0;
    crc_stop = false;
    final_block = false;
    total_blocks = 0;
    crc = 0xFFFFFFFF;
    return;
}


void zmodem_safe_exit()
{
    printf("(%u blocks were written in total)\n", blocks_written);
    show_cursor(true);
    if (outFile) fclose(outFile);
    exit(1);

}

int zmodem_detect(int s, const char *rb, int rbsize, const char *nb, int nboffs, int nbsize)
{
    int i = 0;
    bool incoming = false;
    char *ptr = NULL;
    memset(&rzmodem, 0, ZMODEM_BUFSIZE);
    memcpy(&rzmodem, rb, rbsize); 
    ptr = (char *) &rzmodem; 
    ptr += rbsize;
    memcpy(ptr, (char *) nb + nboffs, nbsize - nboffs);
    //dump_buf((char *) &rzmodem, DATA_INCOMING, 64, 0);

    for (i = 0; i <= 128; i++) {
        if (rb[i+4] == ZPAD && rb[i+5] == ZPAD) {
            if (rb[i+6] == ZDLE) {
                //printf("\n-- ZMODEM HEADER DETECTED AT OFFSET 0x%04x--\n", i);
                //dump_buf(rb, DATA_INCOMING, 64, 0);
                incoming = true;
                break;
            }
        }
    }

    if (!incoming) {
        printf("No ZRQINIT found\n");
        exit(1);
        return false;
        }

    /* this indicates a ZRQINIT frame type */

    rx_frame_type = ZRQINIT;

    if (rb[i] == 'r' && rb[i+1] == 'z' &&  rb[i+2] == 0x0d) {
    }

    switch(rb[i+7]) {
    case ZHEX:

        /*
        A hex header begins with the sequence ZPAD, ZPAD, ZDLE, ZHEX.
        The extra ZPAD character allows the sending program to detect
        an asynchronous header (indicating an error condition) and then
        get the rest of the header with a non-error specific routine
        */
        if (rb[i + 8] == '0' && rb[i + 9] == '0') {
            clear_screen();
            drawbox(0,0,79,11);
            drawbox(3,7,76,9);
            set_cursor(45, 0);
            printf(" zmodem build-id #%s ", BUILD_ID);
            //set_cursor(2,1);
            //printf("<--  ZRQINIT\n");
        } else {
            printf("NO ZRQINIT!\n");
            exit(1);
        }
        show_cursor(false);
        if (!zmodem_recv(s)) {
            printf("+++ ZMODEM transfer failed.\n");
            zmodem_safe_exit();
        } else {
            //printf("+++ ZMODEM transfer completed.\n");
        }
        break;
    case ZBIN:
        printf("(ZBIN - currently unsupported)\n");
        exit(1);
        break;
    default:
        printf("UNKNOWN HEADER FORMAT, CODE 0x%02x\n", rb[i+7]);
        return 0;
        break;
    }

    /* all clear - return control to remote tty */
    return 1;
}

void exit_check()
{
    uint8_t c = 0;
    c = cpm_getchar_noecho();
    if (c == 27) {
        printf("\n");
        zmodem_safe_exit();
    }
    return;
}


int blocking_tcp_recv(int fd, const char *ptr, int size)
{
    int remain = size;
    int rd = 0;
    uint8_t c = 0;
    int32_t start = clock();


    exit_check();

    rd = tcp_recv(fd, ptr, remain);
    while (remain > 0) {
        if (rd > size) {
            /* shouldn't ever happen */
            printf("blocking_read(): read too many bytes (%d)!\n", rd);
        }
        if (rd) {
            ptr += rd;
            remain -= rd;
        }
        exit_check();

        if (clock() > start + 250) {
            /* 5 seconds */
            //printf("\r\n+++ 5s timeout in blocking_tcp_recv() = %d\n", clock() - start);
            //printf("size = %d, remain = %d\n", size, remain);
            return (size - remain);
        }

        if (remain > 0) {
            rd = tcp_recv(fd, ptr, remain);
        }
    }

    return (size - remain);
}



int zmodem_recv(int fd)
{
    uint8_t rxf = 0;
    int i = 0, j = 0;
    int rd = 0;
    int ft = 0;
    int pt = 0;
    const char *fnptr = NULL;
    char *metadata_ptr = NULL;
    const char *pptr = NULL;
    uint16_t total = 0;
    bool has_xon = false;
    char *p1 = NULL, *p2 = NULL;
    off_t end_offset = 0;
    bool halt_processing = false;
    int need_bytes = 0;
    bool transfer_completed = false;
    int final_blksize = 0;

    memset(&checksum, 0, 11);
    memset(&checksum, '?', 8);


//    printf("*** zmodem_recv(%d) -> [rxf %u] offset %u *** \n", fd, rx_frame_type, offset);

    /* send ZRINIT packet */

    memset(&rzmodem, 0, 128);
    memset(&rzmodem, 0x30, 20);

    /* i'm not really sure what these magic values represent. I found them somewhere and apparently they mean ZRINIT */

    rzmodem[0] = ZPAD;
    rzmodem[1] = ZPAD;
    rzmodem[2] = ZDLE;
    rzmodem[3] = ZHEX;
    rzmodem[5] = 0x31;
    rzmodem[12] = 0x32;
    rzmodem[13] = 0x33;
    rzmodem[14] = 0x62;
    rzmodem[15] = 0x65;
    rzmodem[16] = 0x35;
    rzmodem[17] = 0x30;
    rzmodem[18] = 0x0D;
    rzmodem[19] = 0x0A;

    /* lets tack on an IAC DO BINARY here */

    rzmodem[20] = IAC;
    rzmodem[21] = IAC_DO;
    rzmodem[22] = BINARY;

    /* ZRINIT - 20 bytes + 3 IAC DO BINARY */
    rd = tcp_send(fd, (const char *) &rzmodem, 23);

    //set_cursor(2,2);
    //printf(" --> ZRINIT\n");
    //dump_buf((const char *) &rzmodem, DATA_OUTGOING, 32, 0);

    transfer_in_progress = true;
    while (transfer_in_progress) {

        static int chomp_count = 0;
        /* we are going to start here by trying to read a packet header off the wire */

        has_xon = 0;
        total = 0;

        memset(&rzmodem, 0, 128);
        pptr = (const char *) &rzmodem;

        /* a ZMODEM header is typically 20 bytes, sometimes with some other crap at the end.
         * sometimes that crap is the start of a file */

        rd = blocking_tcp_recv(fd, (const char *) pptr, 20);

        /* save the packet size */

        pptr += rd;
        total = rd;

        while (rzmodem[0] != ZPAD) {
            /* sync with ZPAD byte */
            chomp_buffer();
            chomp_count++;
            set_cursor(0, 22);
           // printf("rd = %d, chomp_count = %d\n", rd, chomp_count);
            if (chomp_count >= 4) {
                set_cursor(0, 12);
                printf("+++ all chomped out, no ZPAD after %d bytes\n", chomp_count);
                zmodem_safe_exit();
                }
            }

        has_xon = packet_has_xon((const char *) &rzmodem, rd);

        /* we should have enough information here to decode the frame type, whether BINARY or otherwise */

        /* FIXME: we are chasing a bug here where there may be bytes left in the buffer from the last frame */

        ft = get_frame_type((const char *) &rzmodem);
        if (ft != 0x43) {
            set_cursor(0, 12);
            dump_buf((char *) &rzmodem, DATA_INCOMING, 32, 0);
            set_cursor(0, 20);
            /* if we were going to receive HEX packets other than the ZRQINIT, we should check for XON here */
            printf("Whoah, was expecting a binary frame type (0x43). Got 0x%02x. Bailing out.\n", ft);
            zmodem_safe_exit();
        }


        pt = get_packet_type((const char *) &rzmodem);

//        printf("[frame-type: %02x packet-type: %02x size: %d\n", ft, pt, total);
        total  = 20;

        pt = get_packet_type((const char *) &rzmodem);

        //printf("[frame-type: %02x packet-type: %02x size: %d\n", ft, pt, total);
        total  = 20;

        switch (pt) {
        case ZDATA:
            //set_cursor(2, 10);
            //printf("<--  ZDATA\n");
            /* the readable data starts at offset 0xc (12), and we only 20 bytes total,
               so dispatch the 8 data bytes to the output buffer */

            set_cursor(0, 12);
//            dump_buf((char *) &rzmodem, DATA_INCOMING, 64, 0);
//           printf("\n");
//            printf("rd = %d, chomp_count = %d\n", rd, chomp_count);

            crc = 0xFFFFFFFF;
            printf("\n");
            if (!zmodem_data_decode((unsigned char *) &rzmodem, 12, 20 - chomp_count)) {
                printf("error in zmodem_data_decode()\n");
                zmodem_safe_exit();
            }

            /* from here on out, read 128 bytes from the network, send 128 bytes to zmodem_data_decode until we
                consider EOF reached. Foolproof! I think */

            while (zm_rBytes > 0) {
                memset(&rzmodem, 0, ZMODEM_BUFSIZE);

                if (zm_rBytes <= 128) {
                    final_block = true;
                    //printf("\r\nReached final block!\r\n");
                    rd = blocking_tcp_recv(fd, (const char *) &rzmodem, 128);
                    if (rd ==  0) {
                        printf("No bytes available for final packet.\n");
                    }
                } else {

                    rd = blocking_tcp_recv(fd, (const char *) &rzmodem, 128);
                    /* short read, timed out */
                    if (rd != 128) {
                        printf("+++ couldn't get 128 bytes from network, only %d\n", rd);
                        if (rd == 0) {
                            printf("+++ assuming we're done and closing out\n");
                            transfer_completed = true;
                            final_block = true;
                            goto flush_final;
                        }
                    }
                }
                //printf("* got 128 bytes from network\n");
                /* decode entire buffer */
                if (!zmodem_data_decode((const unsigned char *) &rzmodem, 0, rd)) {
                    printf("error in zmodem_data_decode()\n");
                    zmodem_safe_exit();
                }

flush_final:
                transfer_completed = true;
                if (final_block && idx) {
                    int saved_idx = idx;
                    /* final block */
                    //printf("final_blksize = %d, rd = %d\n", idx, rd);
                    output_block(idx);
                    fclose(outFile);
                    outFile = NULL;
                    clear_screen();
                   for (i = saved_idx ; i < rd; i++) {
                        if (rzmodem[i]) {
                            cpm_putchar((const char) rzmodem[i]);
                            }
                        }

                    memset(&rzmodem, 0, ZMODEM_BUFSIZE);
                    goto get_next_packet;
                    //zmodem_safe_exit();
                }
            }

            printf("Transfer completed?\n");
            memset(&rzmodem, 0, ZMODEM_BUFSIZE);
            fclose(outFile);
            outFile = NULL;
            goto get_next_packet;
            break;
        case ZFILE:
            //set_cursor(2, 3);
            //printf("<--  ZFILE\n");
            /* move the read cursor to the end of the 20 bytes we already have */
            pptr = (unsigned char *) &rzmodem + total;
            rd = blocking_tcp_recv(fd, (const char *) pptr, 1);
            /* grab the rest of the file name, ie. extension */
            while (pptr[0] != '\0') {
                pptr++;
                rd = blocking_tcp_recv(fd, (const char *) pptr, 1);
            }

            /* now read until the end of the metadata */
            pptr++;

            rd = blocking_tcp_recv(fd, (const char *) pptr, 1);
            while (pptr[0] != '\0') {
                pptr++;
                rd = blocking_tcp_recv(fd, (const char *) pptr, 1);
            }

            fnptr = (const char *) &rzmodem + 12;
            metadata_ptr = (const char*) (fnptr + strlen((const char*) fnptr));
            metadata_ptr++;

            /* parse words in the text */

            p1 = (const char *) metadata_ptr;
            p2 = memchr((char *) metadata_ptr, ' ', total - (metadata_ptr- (char *) (&rzmodem)));
            for (i = 0; i < 6; i++) {
                if (p2) {
                    /* if not the final token, stub the end on this one */
                    p2[0] = '\0';
                }
                switch (i) {
                case 0:
                    zm_fileLength = (off_t) strtoul(p1, NULL, 10);
                    break;
                case 1:
                    zm_mTime = (off_t) strtoul(p1, NULL, 10);
                    break;
                case 2:
                    zm_fileMode = (uint32_t) strtoul(p1, NULL, 10);
                    break;
                case 3:
                    zm_xSerial = atoi(p1);
                    break;
                case 4:
                    zm_rFiles = atoi(p1);
                    break;
                case 5:
                    zm_rBytes = (off_t) strtoul(p1, NULL, 10);
                    break;
                }

                if (p2) {
                    /* if this is not the final token, jump over the stump and search for the next string terminator */
                    p1 = p2 + 1;
                    p2 = memchr(p1, ' ', total - ((char *) metadata_ptr-((char *) &rzmodem)));
                }

            }


            blocks_total = (uint16_t) ((zm_fileLength / 128) + (((zm_fileLength % 128) ? 1 : 0)));

            /* CRC:
                 Here, the old specs say there is a 3 byte ZCRCW (null terminated), followed by a two-byte checksum
                 I count 7 additional bytes needing to be read, so maybe the 2-byte checksum is the 16-bit format and not the 32-bit format */

            pptr++;
            if (blocking_tcp_recv(fd, (const char *) pptr, 7) != 7) {
                printf("ZFILE: couldn't read ZCRCW and CRC32\n");
                zmodem_safe_exit();
            }

            if (pptr[0] != ZDLE || pptr[1] != ZCRCW) {
                printf("/* ZDLE+ZCRCW MISSING FROM ZFILE PACKET */\n");
                zmodem_safe_exit();
            }

            /* ^ maybe we should consider storing that somewhere else. hmmm */

            /* TODO:
              potential buffer overflow here I suppose. We should split at the dot and strlen() both parts to make sure they fit into 8.3
               hmmm. that should be a library function methinks */

            sprintf((const char*) &filename, "%s", fnptr);

            /* TODO */

            /* show file info */
            set_cursor(3, 2);
            printf("Filename:         %s\n", filename);
            set_cursor(3, 3);
            printf("Length:           %lu\n", zm_fileLength);
            //printf("Update time:      %lu <inaccurate ; ignored>\n", zm_mTime);
            //printf("UNIX Mode:        %lu\n", zm_fileMode);
            //printf("Serial:           %u\n",  zm_xSerial);
            //printf("Remaining files:  %u\n", zm_rFiles);
            set_cursor(3,4);
            printf("Remaining bytes:  %lu\n", zm_rBytes);

            /* open the file now ... */

            outFile = fopen(filename, "wb");

            /* send ZRPOS packet in reply - 20 bytes */

            memset(&rzmodem, 0, 128);
            memset(&rzmodem, 0x30, 20);

            rzmodem[0] = ZPAD;
            rzmodem[1] = ZPAD;
            rzmodem[2] = ZDLE;
            rzmodem[3] = ZHEX;
            rzmodem[5] = 0x39;
            rzmodem[14] = 0x61;
            rzmodem[15] = 0x38;
            rzmodem[16] = 0x37;
            rzmodem[17] = 0x63;
            rzmodem[18] = 0x0D;
            rzmodem[19] = 0x0A;

            /* ZRINIT - 20 bytes */

            //dump_buf((const char *) &rzmodem, DATA_OUTGOING, 20, 0);
            rd = tcp_send(fd, (const char *) &rzmodem, 20);
            //set_cursor(2, 9);
            //printf(" --> ZRPOS\n");
            break;
        case ZEOF:
            printf("+++ ZEOF !\n");
            zmodem_safe_exit();
            break;
        default:
            printf("+++ unhandled RZ packet type = %d\n", pt);
            dump_buf((const char *) &rzmodem, DATA_INCOMING, rd, 0);
            zmodem_safe_exit();
            break;
        }


get_next_packet:
        /* get next packet */
        if (transfer_completed) {
            /* FIXME: should peek in the buffer here and dump any post-zmodem bytes to the terminal - somewhat done - see the section for final block handling */
            //set_cursor(0, 11);
            reset_zmodem_state();
            return 1;
        }

        total = 0;
        pptr = (const char *) &rzmodem;


    }

    zmodem_safe_exit();
    return 0;

}


int zmodem_data_decode(const char *b, uint8_t start, uint8_t end)
{
    uint8_t i = 0;

    //printf("zmodem_data_decode(0x%04x, %u -> %u) idx = %u, remaining = %u\n", b, start, end, idx, zm_rBytes);
    //dump_buf(b, DATA_INCOMING, 128, 0);
    for (i = start; i < end; i++) {
        ibytes++;
        if (!zmodem_output_byte(b[i], i)) {
            printf("zmodem_output_byte(%u, %u) failed\n", l1, l);
            zmodem_safe_exit();
        }
    }
    //printf("NOW idx = %u\n", idx);
    return 1;

}

int zmodem_output_byte(uint8_t g, uint8_t o)
{

    uint8_t percentage = 0;
    static char *ckptr = (const char *) &checksum;

    l1 = l;
    l = g;

    if (checksum_bytes > 4) {
        printf("checksum_bytes too long! should be <= 4\n");
        zmodem_safe_exit();
    }


    /* I'm taking a punt that telnet control codes need to be processed *before* the zmodem protocol.
       Perhaps we should collapse them in a telnet-transport wrapper around blocking_tcp_recv() */

    if (l == 0xFF && telnet_flag_ff != true) {
        telnet_flag_ff = true;
        return 1;
    }

    if (l == 0xFF && l1 == 0xFF && telnet_flag_ff == true) {
        telnet_flag_ff = false;
    }


    switch(in_zdle) {
    /* we are not in ZDLE escape sequence */
    case false:
        switch(l) {
        case 0x18:
            /* ZDLE escape sequence start - change state, but don't output any byte yet */
            in_zdle = true;
            return 1;
            break;
        default:
            /* do nothing - pass through verbatim */
            in_zdle = false;
            break;
        }
        break;
    /* we are in ZDLE escape sequence */
    case true:
        switch (l) {
        case ZCRCE:
        /*  0x68     CRC next, frame ends, header packet follows */
        case ZCRCG:
        /*  0x69     CRC next, frame continues nonstop */
        case ZCRCQ:
        /*  0x6a     CRC next, frame continuous, ZACK expected */
        case ZCRCW:
            /*  0x6b     CRC next, ZACK expected, end of frame */
            in_zdle = false;
            checksum_bytes = 4;
            in_crc = true;
            return 1;
            break;
        case 0x58:
            l = 0x18;
            in_zdle = false;
            break;
        default:
            //printf("\rZDLE escaped character 0x%02x = %02x\n", l, l - 0x40);
            l -= 0x40;
            in_zdle = false;
            break;
        }
    }


queue_for_output:


    /* If we are in a CRC sequence, trap all the bytes, ZDLE escaped and all. There should be a total of 4 for CRC32. */
    /* And 2 for CRC16. Although we are told not to use/trust CRC16 */

    if (in_crc) {
        if (checksum_bytes) {
            ckptr = (char *) &checksum + (ckidx*2);
            sprintf((char *) ckptr, "%02x", l);
            crc_check = (crc_check >> 8);
            crc_check |= (uint32_t) ((uint32_t) l << (uint32_t) 24);
            checksum_bytes--;
            ckidx++;
            checksum[10] = 0x0;
            if (crc_stop && ckidx == 4) {
                printf("\nCheck the last byte of the CRC.\n");
                printf("[%s]\n", checksum);
                zmodem_safe_exit();
            }
            return 1;
        } else {
            /* in this case, the next byte is the one following the CRC and should be sent for output */
            ckidx = 0;
            checksum[10] = 0x0;

            /* now that we have the checksum from the ZMODEM stream, and we have a crc32 we have generated ourselves, let's compare them */

#ifdef DO_CHECKSUM

            crc = (crc32tbl[(crc^(0x69))&0xff]^(crc>>8));
            crc = ~crc;
            set_cursor(50, 2);
            printf("CRC [%08lx:%08lx]", crc_check, crc);

            if (!final_block && blocks_written < (blocks_total-1)) {
                if (!(crc == crc_check)) {
                    set_cursor(0,21);
                    printf("BAD CRC32 at block %u!\n", blocks_written);
                }
            }
            /* reset CRC32 counter */
            crc = 0xFFFFFFFF;
            ck_ctr = 0;
            crc_check = 0;

#endif /* DO CHECKSUM */

            in_crc = false;
        }
    }

    /* if we made it to here, we are not in a CRC32, and the input data has been ZDLE-decoded, so we should be good to add it to the outbuffer
       buffer and queue it for writing to disk */

    /* tweak the CRC32 with the value of l */

#ifdef DO_CHECKSUM
//    crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^((int) l ) & 0xFF )];
//    set_cursor(0, 13);
//    printf("before(%02x)=0x%08lx\n", (int) l, crc);
    crc = (crc32tbl[(crc^((int) l))&0xff]^(crc>>8));
//    printf("after (%02x)=0x%08lx\n", (int) l, crc);
    //exit(1);
    ck_ctr++;
#endif /* DO_CHECKSUM */

    /*
    if (obytes < 4) {
        set_cursor(0, 17 + obytes);
        printf("%lu = 0x%02x\n", obytes, l);
        }
    */

    outputbuffer[idx] = (uint8_t) l;
    obytes++;
    idx++;


    if (idx == 128) {
        int meter = 0;
        set_cursor(3, 6);
        percentage = get_percentage();
        /* PURPLE */
//        printf("\x1b\x5b""35m");
        printf("%%%3u ", (uint8_t) get_percentage());
        if (blocks_written <= blocks_total) {
            printf("[%03u/%03u] ",  blocks_written+1,
                   blocks_total);
//               (zm_fileLength / 128) + (((zm_fileLength % 128) ? 1 : 0)));
//            printf("<%04d> ", ck_ctr);
            printf("REMAINING: %8lu", zm_rBytes);
//            printf("%7lu %7lu", ibytes, obytes);
//            printf("\x1b\x5b""0m");
            set_cursor(4, 8);
            printf((const char *) ANSI_GREEN);
            for (meter = 0; meter < ((percentage / 4) * 3); meter++) {
                cpm_putchar(0xDD);
            }
        }
        printf((const char *) ANSI_NORMAL);

        fseek(outFile, (long ) ((long) blocks_written * (long) 128), SEEK_SET);
        if (fwrite((const char *) &outputbuffer, 128, 1, outFile) != 1) {
            printf("* BUFFER FLUSH FAILED!\n");
            zmodem_safe_exit();
        }
        /* reset output buffer pointer for next block */
        idx = 0;
        blocks_written ++;


        // decrement zm_rBytes here
        zm_rBytes -= 128;
        memset(&outputbuffer, 0, 128);
    }
    return 1;

}


int output_block(uint8_t len)
{
    uint16_t fileblocks = 0;
    fileblocks = (uint16_t) ((zm_fileLength / 128) + (((zm_fileLength % 128) ? 1 : 0)));

    /*
        set_cursor(2, 6);
        printf("\x1b\x5b""34m");
        printf("%%%100 [%03u/%03u] ",  blocks_written+1,
               fileblocks);
        printf("[%-10s] remain: %6u", (char *) checksum, zm_rBytes);
        printf("%7lu %7lu", ibytes, obytes);
        printf("\x1b\x5b""0m");
    */

    fseek(outFile, (long ) ((long) blocks_written * (long) 128), SEEK_SET);
    if (fwrite((const char *) &outputbuffer, len, 1, outFile) != 1) {
        printf("* BUFFER FLUSH FAILED!\n");
        zmodem_safe_exit();
    } else {
        idx = 0;
        blocks_written ++;
        zm_rBytes -= len;
    }
    return 1;

}


int get_packet_type(const char *ptr)
{
    const char *p = ptr;
    int i = 0;

    if (p[0] == XON) {
        //printf("Ignoring XON\n");
        p++;
        i++;
    }

    while (p[0] == ZPAD && i < 2) {
        i++;
        p++;
    }

    if (i == 0) {
        /* didn't move - not a ZMODEM frame */
        return -1;
    }

    if (i != 1 && i != 2) {
        return -1;
    }

    if (p[0] != ZDLE) {
        return -1;
    }

    return p[2];

}

int get_frame_type(const char *ptr)
{
    char *p = (const char *) ptr;
    int i = 0;

    if (p[0] == XON) {
        //printf("Spurious XON? received\n");
        i++;
        p++;
    }

    while ((p[0] == ZPAD) && i < 2) {
        i++;
        p++;
    }

    if (i == 0) {
        /* didn't move - not a ZMODEM frame */
        return 0;
    }

    if (i != 1 && i != 2) {
        return 0;
    }

    if (p[0] != ZDLE) {
        return 0;
    }

    return p[1];
}

void dump_buf(const char *buf, int direction, int limit, off_t start_addr)
{
    static char ascbuf[17];
    int c = 0;
    int i = 0;
    int line = 0;
    uint8_t x = 0;
    limit = ((limit / 16)+1) * 16;

    if (limit > 2030) {
        limit = 2030;
    }

    memset(&ascbuf, 0, 17);
    printf("[%c] %04x: ", (direction ? '>': '<'), start_addr + (i / 16) * 16);
    for (i = 0; i < limit; i++) {
        printf("%02x ", buf[i]);
        //if (buf[i] >= 32 && buf[i] <= 127) {
        if (isgraph(buf[i])) {
            /* printable or not? */
            ascbuf[c] = buf[i];
        } else {
            ascbuf[c] = '.';
        }
        c++;
        if (c > 15) {
            c = 0;
            printf("| %s\n", ascbuf);
            line++;
            memset(&ascbuf, 0, 17);
            if (line > 7) {
                printf("-- Hit any Key --\n");
                x= _getchar_noecho();
                while (x == 0) {
                    x= _getchar_noecho();
                }
                if (x == 'q' || x =='Q') {
                    return;
                }
                line = 0;
            }
            if (i < limit-1) {
                printf("[%c] %04x: ", (direction ? '>' : '<'), start_addr + ((i / 16)+1) * 16);
            }
        }
    }

    printf("\n");
}

bool packet_has_xon(const char * ptr, int len)
{
    if (ptr == NULL) return false;
    if (ptr[len-1] == XON) {
        return true;
    }
    return false;
}


uint8_t get_percentage()
{
    float x = (float) 100.0 / blocks_total;
    x = (float) blocks_written * (float) x;
    return (uint8_t) x;
}


int drawbox(int x1, int y1, int x2, int y2)
{

    int i = 0;

    //printf("drawbox(%d, %d, %d, %d)\n", x1, y1, x2, y2);
    /* top span */
    set_cursor(x1, y1);
    cpm_putchar(nineslice[si][0]);
    for (i = x1+1 ; i <= x2 -1 ; i++) {
        cpm_putchar(nineslice[si][1]);
    }
    cpm_putchar(nineslice[si][2]);

    /* vertical spans */

    for (i = y1 + 1 ; i <= y2 - 1; i++) {
        set_cursor(x1, i);
        cpm_putchar(nineslice[si][3]);
        set_cursor(x2, i);
        cpm_putchar(nineslice[si][5]);
    }

    /* bottom span */

    set_cursor(x1, y2);
    cpm_putchar(nineslice[si][6]);
    for (i = x1+1 ; i <= x2 -1 ; i++) {
        cpm_putchar(nineslice[si][7]);
    }
    cpm_putchar(nineslice[si][8]);

    return 0;

}

