#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <tty.h>
#include <ctype.h>
#include <getopt.h>
#include <tcp.h>
#ifdef __CPM__ 
#include <syslib/cpm_sysfunc.h>
#include <syslib/ansi_term.h>
#endif
#include "osdep.h"
#include "../../build-id.h"
#include "telnet.h"
#include "zmodem.h"

#ifndef __CPM__
#define DEBUG_NETWORK
#endif 

bool binmode = false;
bool noecho = false;
bool shift1 = false;
bool merge = false;
bool iac_active = false;
int iac_code = 0;
int option_command = 0;
int option_code = 0;
static uint8_t local_echo = 1;

#define RING_BUFFER_SIZE    64          /* bytes */
#define OUTPUT_BINARY       0
#define OUTPUT_HEXDECIMAL   1

static unsigned char ring_buffer[RING_BUFFER_SIZE];

#define MAX_BUF         128
#define DEFAULT_PORT    23

unsigned char netbuf[MAX_BUF];


bool telnet_dont(int s, uint8_t optcode);
bool telnet_wont(int s, uint8_t optcode);
bool telnet_will(int s, uint8_t optcode);
bool telnet_do(int s, uint8_t optcode);
void version();

void version()
{

    printf("telnet+zmodem build-id %s\n", BUILD_ID);
    exit(0);
}

const char *IACName(int iac)
{
    switch (iac) {
    case IAC_WILL:
        return (const char *) "IAC_WILL";
        break;
    case IAC_WONT:
        return (const char *) "IAC_WONT";
        break;
    case IAC_DO:
        return (const char *) "IAC_DO";
        break;
    case IAC_DONT:
        return (const char *) "IAC_DONT";
        break;
    }
    return (const char*) "**UNKNOWN**";
}

bool telnet_dont(int s, uint8_t optcode)
{
    static char optbuf[4];
    memset(&optbuf, 0, 4);
    sprintf((char *) &optbuf, "\xFF%c%c\0", IAC_DONT, optcode);
    if (tcp_send(s, (const char*) "\xFF\xFE\x01", 3) != 3) {
        printf("error sending IAC DONT %u\n", optcode);
        return false;
    }
//    printf(">[SENT DONT %u]\r\n", optcode);
    return true;

}

bool telnet_wont(int s, uint8_t optcode)
{

    static char optbuf[4];
    memset(&optbuf, 0, 4);
    sprintf((char *) &optbuf, "\xFF%c%c\0", IAC_WONT, optcode);
    if (tcp_send(s, (const char*) &optbuf, 3) != 3) {
        printf("error sending IAC WONT %u\n", optcode);
        return false;
    }
//   printf(">[SENT WONT %u]\r\n", optcode);
    return true;
}

bool telnet_will(int s, uint8_t optcode)
{

    static char optbuf[4];
    memset(&optbuf, 0, 4);
    sprintf((char *) &optbuf, "\xFF%c%c\0", IAC_WILL, optcode);
    if (tcp_send(s, (const char*) &optbuf, 3) != 3) {
        printf("error sending IAC WILL %u\n", optcode);
        return false;
    }
    //  printf(">[SENT WILL %u]\r\n", optcode);
    return true;
}

bool telnet_do(int s, uint8_t optcode)
{

    static char optbuf[4];
    memset(&optbuf, 0, 4);
    sprintf((char *) &optbuf, "\xFF%c%c\0", IAC_DO, optcode);
    if (tcp_send(s, (const char*) &optbuf, 3) != 3) {
        printf("error sending IAC DO %u\n", optcode);
        return false;
    }
    // printf(">[SENT DO %u]\r\n", optcode);
    return true;
}

const char *dns_encode(char *hostname)
{
    static uint8_t encodebuf[128];
    int j = 0;
    uint8_t *i = (char *) hostname;
    uint8_t *rb = i;
    uint8_t bc = 0;
    bool terminated = false;
    char *p = (char *) &encodebuf;


    while (!terminated) {
        /* iterate through hostname, incrementing pointer and byte count until we hit a NULL or a stop */
        while (i[0] != '\0' && i[0] != '.') {
            /* if we hit a NUL, we are at the end, so flag it */
            if (i[0] == '\0') {
                terminated = true;
            }
            bc++;
            i++;
        }

        /* if we hit a NUL, we are at the end, so flag it */
        if (i[0] == '\0') {
            terminated = true;
        }

        /* p pointers to our output buf - set the next byte to bc to indicate how many bytes in this label section */
        p[0] = (uint8_t) bc + 0;
        p++;

        /* copy bc number of bytes from rb, incrementing along the way */

        for (j = 0; j < bc; j++) {
            p[0] = tolower(rb[0]);
            rb ++;
            p++;
        }
        /* reset counters */
        bc = 0;
        /* jump over the dot in the input */
        rb++;
        i++;
    }
    /* write the final label */

    p[0] = (uint8_t) bc + 0;
    p++;
    for (j = 0; j < bc; i++) {
        p[0] = tolower(rb[0]);
        rb ++;
        p++;
    }


    return (const char *) &encodebuf;

}


const char *dns_lookup(char *lookupname)
{

    static uint8_t dnsaddr[16];
    char *labelled_name = NULL;
    char *p1 = NULL, *p2 = NULL;
    static uint8_t dnsreq[128];
    int fd = 0;
    uint16_t rl = 0;
    int rc = 0;
    int i = 0;

    memset(&dnsreq, 0, 128);
    //printf("dns_lookup(%s)\n", lookupname);
    /* IDENTIFIER: bytes 0-1: query id  */
    dnsreq[0] = 0;
    dnsreq[1] = 0;
    /* FLAGS AND CODES: byte 2-3: 1 bit(0) indicating query, 4 bits(0) indicating STANDARD QUERY, 1 bit(0) AA flag, 1 bit(0) TC (trunc) flag, 1 bit RD(1) recursion */
    dnsreq[2] = 0x01; /* should it be high bit? 0x80? */
    dnsreq[3] = 0x00;
    /* QUESTION COUNT: bytes 4-5: question - should be MSB  */
    dnsreq[4] = 0x00;
    dnsreq[5] = 0x01;
    /* ANSWER COUNT: byte 6-7: AR count */
    dnsreq[6] = 0x00;
    dnsreq[7] = 0x00;
    /* NAME SERVER: AUTHORITY RECORD COUNT byte 8-9: AR count */
    dnsreq[8] = 0x00;
    dnsreq[9] = 0x00;
    /* ADDITIONAL RECORD COUNT byte 10-11: additional record count */
    dnsreq[10] = 0x00;
    dnsreq[11] = 0x00;

    labelled_name = (char *) dns_encode(lookupname);
    //printf("lookupname = %s\n", (const char*) labelled_name);
    //
    p1 = (char *) labelled_name;
    p2 = (char *) memchr(p1, 0, 128);

 	  rl = (uint16_t) ((uint16_t) p2 - (uint16_t) p1);
    memcpy(&dnsreq + 12, p1, rl);

    /* question type  */

    dnsreq[13 + rl] = 0x00;
    dnsreq[14 + rl] = 0x01;

    /* question class */

    dnsreq[15 + rl] = 0x00;
    dnsreq[16 + rl] = 0x01;

    memmove(&dnsreq+2, &dnsreq, 126);
    dnsreq[0] = 0;
    dnsreq[1] = 12+rl+4 + 1;

    /*
    printf("header  length is %u\n", 12);
    printf("request length is %u\n", rl+4);
    printf("packet  length is %u\n", 12+rl+4 + 1);
    */

    /*
    for (i = 0; i < 2+12+rl+4; i++) {
        printf("%02x ", dnsreq[i]);
        }
    printf("\n");
    */

    fd = tcp_connect("8.8.8.8", 53);
    if (fd != -1) {
        //printf("Connected netfd %d to Google DNS!\n", fd);
        rc = tcp_send(fd, (const char *) &dnsreq, 2+12+rl+4 + 1);
        //printf("send rc = %d\n", rc);
        memset(&dnsreq, 0, 128);


        /* FIXME: we need to be able to set a timeout here, in case it hangs a long time */

        rc = tcp_recv(fd, (const char *) &dnsreq, 128);
        while (rc == 0) {
            rc = tcp_recv(fd, (const char *) &dnsreq, 128);
        }

        if (rc < 12) {
            /* error */
            return NULL;
        }

        if (dnsreq[rc-5] != 4) {
            /* response not length==4 */
            return NULL;
        }

        tcp_close(fd);
        sprintf((char *) &dnsaddr, "%u.%u.%u.%u", dnsreq[rc-4], dnsreq[rc-3], dnsreq[rc -2], dnsreq[rc-1]);
        return((const char*) &dnsaddr);

    } else {
        printf("Couldn't open connection to DNS server 8.8.8.8\n");
        /* couldn't connect to google - is the internet plugged in ?*/
    }

    /* NXDOMAIN or other error */

    return NULL;
}


int output_byte(int type, int s, uint8_t b)
{
    int i = 0;
    /* I have a feeling this memmove() is what is making terminal output so slow */
    for (i = 0; i < RING_BUFFER_SIZE - 1; i++) {
        ring_buffer[i] = ring_buffer[i+1];
        }

    //memmove(&ring_buffer, &ring_buffer+1, RING_BUFFER_SIZE - 1);
    /* **** OUTPUT **** where the byte finally hits the wire - rotate the ring buffer, adding our new byte a the end */
    ring_buffer[RING_BUFFER_SIZE-1] = b;
    if (zmodem_detect(s, (const char *) &ring_buffer)) {
        //printf("[ZMODEM TRANSFER DONE]\n");
        /* clear ring buffer to prevent transfer starting again */
        memset(&ring_buffer, 0, RING_BUFFER_SIZE);
        return 0;
    };
    switch(type) {
    case OUTPUT_BINARY:
        _putchar(b);
        //printf("[%02x:%c] ", b, b);
        break;
    case OUTPUT_HEXDECIMAL:
        printf("[%02x] ", b);
        break;
    }
    return 1;
}


int main(int argc, char *argv[])
{

    FILE *font_in = NULL;
    int s = 0;
    int i = 0, j = 0;
    off_t offset = 0;
    int c = 0;
    int opterr = 0;
    char optopt = 0;
    uint8_t kb = 0;
    bool do_io_out = false;
    int rc = 0;
    uint16_t port = DEFAULT_PORT;
    char *connect_name = NULL;
    char *p = NULL;

    if (argc < 2) {
        printf("usage: telnet <x.x.x.x> [port]\n");
        exit(1);
    }

    while ((c = getopt (argc, argv, "OV")) != -1) {
        switch (c)
        {
        case 'O':
            printf("Telnet option debugging enabled.\n");
            shift1 = true;
            break;
        case 'M':
            printf("Merge mode enabled.\n");
            merge = true;
            break;
        case 'V':
            version();
            break;
        case '?':
            if (optopt == 'c') {
                printf ("Option -%c requires an argument.\n", optopt);
                exit(1);
            }
            else if (isprint (optopt)) {
                printf ("Unknown option `-%c'.\n", optopt);
                exit(1);
            }
            else {
                printf ("Unknown option character `\\x%x'.\n",
                        optopt);
                exit(1);
            }
            return 1;
        case EOF:
            /* END OF FILE? */
            goto run_program;
            break;
        default:
            printf("exiting with c= %d [%c]\n", c, c);
            exit (1);
        }

    }


run_program:

#ifdef __CPM__
    set_term(TERM_ANSI);
    /* maybe this will help with performance for intensive ANSI! */
#endif /* __CPM__ */

    connect_name = argv[1];
    p = connect_name;
    /* convert to lower case */
    while (p[0] != '\0') {
        p[0] = tolower(p[0]);
        p++;
    }

    if (!isdigit(connect_name[0])) {
        printf("Looking up %s ... \n", argv[1]);
        connect_name = (const char *) dns_lookup(argv[1]);
        if (!connect_name) {
            printf("DNS lookup failed.\n");
            exit(1);
        }
        /* have correct "connect name" */
    }

    local_echo  = 1;

    printf("Connecting to %s:", connect_name);
    if (argc > 2) {
        port = atoi(argv[2]);
    } else {
        port = DEFAULT_PORT;
    }
    printf("%u ... ", port);


    /* clear the ring buffer */

    memset(&ring_buffer, 0, RING_BUFFER_SIZE);


    s = tcp_connect((const char *) connect_name, port);
    if (s == -1) {
        printf(" failed.\n");
        return 0;
    } else {
        printf(" connected.\n");
    }

    rc = tcp_recv(s, (const char *) &netbuf, 128);

    while (rc != 255 && rc != -1) {
        if (rc) {
#ifdef DEBUG_NETWORK
            printf("READ->%d\n", rc);
            dump_buf((const char *) &netbuf, DATA_INCOMING, rc, 0);
#endif /* DEBUG_NETWORK */
        }
        for (i = 0; i < rc; i++) {
            /* MSG_SEND */
            if (iac_code && option_code && iac_active) {
                /* for processing some long options, eg. terminal type */
                switch(netbuf[i]) {
                case MSG_SEND:
                    switch (option_code) {
                    case TERMINALTYPE:
                        // see RFC1091 - TERMINAL-TYPE suboption negotiation
                        //printf("[SENT IAC SB TERMINAL-TYPE IS DEC-VT100 IAC SE]\n", option_code);
                        // see RFC1010 for the list of allowed terminals
                        // IAC SB TERMINAL-TYPE IS ANSI IAC SE
                        //

                  /* be careful about the number of bytes you send here, or you might send some rubbish! this was happening
                   at the login prompt and it took me ages to figure out. we need better code for exchanging terminal type */

                        if (tcp_send(s, (const char*) "\xFF\xF0\xFF\xFA\x18\x00\x41\x4E\x53\x49\xFF\xF0", 12) != 12) {
                            printf("error sending IAC WILL TERMINAL-TYPE\n");
                            exit(1);
                        }
                        iac_code = 0;
                        iac_active = false;
                        option_code = 0;
                        goto next_byte;
                        break;
                    default:
                        printf("MSG_SEND for OPTION CODE %u\n", option_code);
                        exit(1);
                        break;
                    }

                default:
                    printf("UNKNOWN COMMAND for OPTION CODE\n", netbuf[i]);
                    exit(1);
                    break;
                }
            }

            /* TELNET OPTION CODES */

            if (iac_code && iac_active) {
                switch (netbuf[i]) {
                case BINARY:
                    switch (iac_code) {
                    case IAC_WILL:
                        if (tcp_send(s, (const char*) "\xFF\xFD\x00", 3) != 3) {
                            printf("error sending IAC DO BINARY\n");
                            exit(1);
                        }
                        printf("[SENT IAC DO BINARY]\n");
                        iac_code = 0;
                        iac_active = false;
                        option_code = 0;
                        goto next_byte;
                        break;
                    case IAC_DO:
                        if (tcp_send(s, (const char*) "\xFF\xFB\x00", 3) != 3) {
                            printf("error sending IAC WILL BINARY\n");
                            exit(1);
                        }
                        printf("[SENT IAC WILL BINARY]\n");
                        iac_code = 0;
                        iac_active = false;
                        option_code = 0;
                        goto next_byte;
                        break;
                        break;
                    default:
                        /* TODO */
                        printf("[UNHANDLED STATE FOR OPTION BINARY] = %s\n", (const char *) IACName(iac_code));
                        exit(1);
                    }
                    iac_code = 0;
                    iac_active = false;
                    option_code = 0;
                    goto next_byte;
                    break;
                case ECHO:
                    /* ECHO requires special handling to get right, since hidden password entry etc. depends on it */
                    switch (iac_code) {
                    case IAC_WILL:
                        //printf("<[RCVD IAC_WILL ECHO]\n");
                        if (!telnet_do(s, netbuf[i])) {
                            exit(1);
                        }
                        local_echo = 0;
                        break;
                    case IAC_WONT:
                        //printf("<[RCVD IAC_WONT ECHO]\n");
                        if (!telnet_dont(s, netbuf[i])) {
                            exit(1);
                        }
                        local_echo = 1;
                        break;
                    case IAC_DO:
                        //printf("<[RCVD IAC_DO ECHO]\n");
                        if (!telnet_will(s, netbuf[i])) {
                            exit(1);
                        }
                        local_echo = 1;
                        break;
                    case IAC_DONT:
                        //printf("<[RCVD IAC_DONT ECHO]\n");
                        if (!telnet_wont(s, netbuf[i])) {
                            exit(1);
                        }
                        local_echo = 0;
                        break;
                    default:
                        printf("UNHANDLED MODE FOR OPTION ECHO]\n");
                        exit(1);
                        break;
                    }
                    iac_code = 0;
                    iac_active = false;
                    option_code = 0;
                    goto next_byte;
                    break;
                case SUPPRESSGOAHEAD:
                    /* RFC858 */
                    //printf("RCVD SUPPRESS GO AHEAD, IAC_CODE = %u\n", iac_code);
                    if (tcp_send(s, (const char*) "\xFF\xFD\x03", 3) != 3) {
                        printf("error sending IAC DO SUPPRESSGOAHEAD\n");
                        exit(1);
                    }
                    //printf("[SENT IAC DO SUPPRESS-GO-AHEAD]\n");
                    iac_code = 0;
                    iac_active = false;
                    option_code = 0;
                    goto next_byte;
                    break;
                case CHARSET:
                case TOGGLEFLOWCONTROL:
                case STATUS:
                case XDISPLAYLOCATION:
                case NEWENVIRON:
                case LOCATION:
                case TERMINALSPEED:
                case LINEMODE:
                    switch(iac_code) {
                    case IAC_DO:
                        if (!telnet_wont(s, netbuf[i])) {
                            printf("+ error sending tcp_wont(%u)\n", netbuf[i]);
                            exit(1);
                        }
                        break;
                    case IAC_WILL:
                        if (!telnet_dont(s, netbuf[i])) {
                            printf("+ error sending tcp_dont(%u)\n", netbuf[i]);
                            break;
                        }
                    }
                    iac_code = 0;
                    iac_active = false;
                    option_code = 0;
                    goto next_byte;
                    break;
                case WINDOWSIZE:
                    /* RFC1073 */
                    //printf("[WINDOW-SIZE]\n");
                    switch (iac_code) {
                    case IAC_DO:
                        //printf("[RCVD DO NAWS (WINDOW-SIZE)]\n");
                        if (tcp_send(s, (const char*) "\xFF\xFB\x1F", 3) != 3) {
                            printf("error sending IAC WILL NAWS (WINDOW-SIZE)\n");
                            exit(1);
                        }

                        /* hardcoded 80x24 */

                        if (tcp_send(s, (const char*) "\xFF\xFA\x1F\x00\x50\x00\x18\xFF\xF0", 9) != 9) {
                            printf("error sending IAC SB NAWS (WINDOW-SIZE) 0 80 0 24 IAC SE\n");
                            exit(1);
                        }
                        //printf("[SENT IAC SB NAWS (WINDOW-SIZE) 0 80 0 24 IAC SE]\n");
                        iac_code = 0;
                        iac_active = false;
                        option_code = 0;
                        goto next_byte;
                        break;
                    case IAC_DONT:
                        goto next_byte;
                        break;
                    default:
                        printf("UNHANDLED WINDOW-SIZE REQUEST MODE: %d\n", iac_code);
                        exit(1);
                        break;
                    }
                    break;
                case TERMINALTYPE:
                    //printf("[TERMINAL-TYPE]\n");
                    option_code = netbuf[i];
                    switch (iac_code) {
                    case IAC_DO:
                        /* we will "IAC DO TERMINAL-TYPE" */
                        // printf("[RCVD DO TERMINAL-TYPE]\n");
                        if (tcp_send(s, (const char*) "\xFF\xFB\x18", 3) != 3) {
                            printf("error sending IAC WILL TERMINAL-TYPE\n");
                            exit(1);
                        }
                        //printf("[SENT WILL TERMINAL-TYPE]\n");
                        iac_code = 0;
                        iac_active = false;
                        option_code = 0;
                        goto next_byte;
                        break;
                    case IAC_SB:
                        /* suboption negoiate */
                        //printf("[SUBNEGOTIATE TERMINAL-TYPE]", netbuf[i]);
                        iac_code = IAC_SB;
                        goto next_byte;
                        break;
                    default:
                        printf("unknown IAC code = %d\n", iac_code);
                        exit(1);
                        break;
                    }
                    iac_active = false;
                    iac_code = 0;
                    option_code = 0;
                    goto next_byte;
                    break;
                case IAC_SE:
                    //printf("+[IAC SE]\n");
                    iac_active = false;
                    iac_code = 0;
                    option_code = 0;
                    goto next_byte;
                    break;
                case IAC:
                    //printf("+[IAC]\n");
                    iac_code = 0;
                    goto next_byte;
                    break;
                case 69:
                    /* no idea what this is, seems to be user defined but linux telnet DONTs it. so let's do that  */
                    if (tcp_send(s, (const char*) "\xFF\xFE\x45", 3) != 3) {
                        printf("error sending IAC DONT 69\n");
                        exit(1);
                    }
                    //printf("[SENT DONT 69]\n");
                    goto next_byte;
                    break;
                case 70:
                    /* no idea what this is, seems to be user defined but linux telnet DONTs it. so let's do that  */
                    if (tcp_send(s, (const char*) "\xFF\xFE\x46", 3) != 3) {
                        printf("error sending IAC DONT 70\n");
                        exit(1);
                    }
                    //printf("[SENT DONT 70]\n");
                    goto next_byte;
                    break;
                case 90:
                    /* no idea what this is, seems to be user defined but linux telnet DONTs it. so let's do that  */
                    if (tcp_send(s, (const char*) "\xFF\xFE\x5A", 3) != 3) {
                        printf("error sending IAC DONT 90\n");
                        exit(1);
                    }
                    //printf("[SENT DONT 90]\n");
                    goto next_byte;
                    break;
                case 91:
                    /* no idea what this is, seems to be user defined but linux telnet DONTs it. so let's do that  */
                    switch (iac_code) {
                    case IAC_DO:
                        if (tcp_send(s, (const char*) "\xFF\xFC\x5B", 3) != 3) {
                            printf("error sending IAC WONT 91\n");
                            exit(1);
                        }
                        // printf("[SENT WONT 91]\n");
                        break;
                    case IAC_WILL:

                        if (tcp_send(s, (const char*) "\xFF\xFE\x5B", 3) != 3) {
                            printf("error sending IAC DONT 91\n");
                            exit(1);
                        }
                        // printf("[SENT DONT 91]\n");
                        break;
                    }
                    iac_active = false;
                    iac_code = 0;
                    option_code = 0;
                    goto next_byte;
                    break;
                case 200:
                    /* no idea what this is, seems to be user defined but linux telnet DONTs it. so let's do that  */
                    if (tcp_send(s, (const char*) "\xFF\xFC\xC8", 3) != 3) {
                        printf("error sending IAC WONT 200\n");
                        exit(1);
                    }
                    //printf("[SENT WONT 200]\n");
                    goto next_byte;
                    break;
                default:
                    printf("[UNHANDLED OPTION %d]\n", netbuf[i]);
                    exit(1);
                }
            }

            switch(iac_active) {
            case true:
                switch (netbuf[i]) {
                case 242:
                    //printf("[CTRL-C??]\n");
                    iac_active = false;
                    iac_code = 0;
                    option_code = 0;
                    goto next_byte;
                case 0x0:
                    printf("IAC 0!\n");
                    iac_active = false;
                    iac_code = 0;
                    option_code = 0;
                    goto next_byte;
                case 0xF0:
                    /* SE - end negotiations */
                    //printf("[IAC-SE]\n");
                    iac_active = false;
                    iac_code = 0;
                    option_code = 0;
                    goto next_byte;
                    break;
                case 0xFF:
                    _putchar(0xFF);
                    iac_active = false;
                    break;
                case 251:
                    //printf("[IAC-WILL]\n");
                    iac_code = netbuf[i];
                    break;
                case 250:
                    //printf("[IAC-SB]\n");
                    iac_code = netbuf[i];
                    break;
                case 252:
                    //printf("[IAC-WONT]\n");
                    iac_code = netbuf[i];
                    break;
                case 253:
                    //printf("[IAC-DO]\n");
                    iac_code = netbuf[i];
                    break;
                case 254:
                    //printf("[IAC-DONT]\n");
                    iac_code = netbuf[i];
                    break;
                default:
                    printf("IAC CODE = %u\n", netbuf[i]);
                    exit(1);
                }
                break;
            case false:
                switch (netbuf[i]) {
                case 0x00:
                    //printf("* \\x00 byte in output!\n");
                    break;
                case 0xFF:
                    /* TELNET ESCAPE CODE */
                    iac_active = true;
                    break;
                default:
                    if (output_byte(OUTPUT_BINARY, s, netbuf[i]) == 0) {
                        //printf("+++ EOF reached on ZMODEM transfer!\n");
                        /* clear the netbuffer so we don't accidentally pick up the ZMODEM header again */ 
                        memset(&netbuf, 0, 62); 
                        printf("\n\n");
                        };
                    break;
                }
                break;
            }
next_byte:

				/* dirty hack to silence compiler error about label at end of compound statement */

				while (0) { }

        }

        i = 0;
        do_io_out = false;
        memset(&netbuf, 0, 128);
        kb = _getchar_noecho();
        while (kb) {
            /* CTRL + ] */

            if (kb == '\r') {
                netbuf[i] = '\r';
                i++;
                netbuf[i] = '\n';
                do_io_out = true;
                goto expand_cr_done;
                }

            if (kb == 0x1D) {
                printf("\nUser ended session.\n");
                show_cursor(true);
                /* CTRL + C */
                s = tcp_close(s);
                exit(0);
            }

            /* regular character */

            do_io_out = true;
            netbuf[i] = kb;

expand_cr_done:
            //printf("[%d]", local_echo);
            if (local_echo) {
                if (kb == 0xff) {
                    printf("OUTPUTTING 0xFF TELNET ESCAPE! ERROR\n");
                    exit(1);
                }
                //printf("[%c]\n", kb);
                _putchar(kb);
            }
            i++;
            kb = _getchar_noecho();
        }

        if (do_io_out) {
            //printf("tcp_send(%d, [%s], %d\n", s, netbuf, i);
            rc =tcp_send(s, (const char*) &netbuf, i);
            //printf("send() rc = %d\n", rc);
        }

        rc = tcp_recv(s, (const char *) &netbuf, 128);
    }

    while (_getchar() == 0) {
        /* busy wait */
    }
    s = tcp_close(s);
    printf("\n");

    exit(0);
}

