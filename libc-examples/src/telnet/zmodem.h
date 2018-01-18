
/* ASCII */

#define SOH             0x01
#define STX             0x02
#define EOT             0x04
#define ENQ             0x05
#define ACK             0x06
#define LF              0x0a
#define CR              0x0d
#define XON             0x11
#define XOFF            0x13

/* ZMODEM */

#define ZPAD            0x2a    /* pad character; begins frames */
#define ZDLE            0x18    /* ctrl-x zmodem escape */
#define ZDLEE           0x58    /* escaped ZDLE */      
#define ZBIN            0x41    /* binary frame indicator (CRC16) */
#define ZHEX            0x42    /* hex frame indicator */
#define ZBIN32          0x43    /* binary frame indicator (CRC32) */
#define ZBINR32         0x44    /* run length encoded binary frame (CRC32) */
#define ZVBIN           0x61    /* binary frame indicator (CRC16) */
#define ZVHEX           0x62    /* hex frame indicator */
#define ZVBIN32         0x63    /* binary frame indicator (CRC32) */
#define ZVBINR32        0x64    /* run length encoded binary frame (CRC32) */

/* FRAME TYPES */

#define ZMODEM_NONE     -1
#define ZRQINIT         0x00    /* request receive init (s->r) */
#define ZRINIT          0x01    /* receive init (r->s) */
#define ZSINIT          0x02    /* send init sequence (optional) (s->r) */
#define ZACK            0x03    /* ack to ZRQINIT ZRINIT or ZSINIT (s<->r) */
#define ZFILE           0x04    /* file name (s->r) */
#define ZSKIP           0x05    /* skip this file (r->s) */
#define ZNAK            0x06    /* last packet was corrupted (?) */
#define ZABORT          0x07    /* abort batch transfers (?) */
#define ZFIN            0x08    /* finish session (s<->r) */
#define ZRPOS           0x09    /* resume data transmission here (r->s) */
#define ZDATA           0x0a    /* data packet(s) follow (s->r) */
#define ZEOF            0x0b    /* end of file reached (s->r) */
#define ZFERR           0x0c    /* fatal read or write error detected (?) */
#define ZCRC            0x0d    /* request for file CRC and response (?) */
#define ZCHALLENGE      0x0e    /* security challenge (r->s) */
#define ZCOMPL          0x0f    /* request is complete (?) */   
#define ZCAN            0x10    /* pseudo frame; */ 
#define ZFREECNT        0x11    /* request free bytes on file system (s->r) */
#define ZCOMMAND        0x12    /* issue command (s->r) */
#define ZSTDERR         0x13    /* output data to stderr (??) */

/* ZDLE SEQUENCES */

#define ZCRCE           0x68    /* CRC next, frame ends, header packet follows */
#define ZCRCG           0x69    /* CRC next, frame continues nonstop */
#define ZCRCQ           0x6a    /* CRC next, frame continuous, ZACK expected */
#define ZCRCW           0x6b    /* CRC next, ZACK expected, end of frame */
#define ZRUB0           0x6c    /* translate to rubout 0x7f */
#define ZRUB1           0x6d    /* translate to rubout 0xff */

/* RECEIVER CAPABILITY FLAGS */

#define CANFDX          0x01    /* Rx can send and receive true full duplex */
#define CANOVIO         0x02    /* Rx can receive data during disk I/O */
#define CANBRK          0x04    /* Rx can send a break signal */
#define CANCRY          0x08    /* Receiver can decrypt */
#define CANLZW          0x10    /* Receiver can uncompress */
#define CANFC32         0x20    /* Receiver can use 32 bit Frame Check */
#define ESCCTL          0x40    /* Receiver expects ctl chars to be escaped */
#define ESC8            0x80    /* Receiver expects 8th bit to be escaped */

#define DATA_INCOMING   0
#define DATA_OUTGOING   1

int zmodem_detect(int s, const char *rb, int rbsize, const char *nb, int nboffs, int nbsize);
int zmodem_recv(int fd);
int get_frame_type(const char *ptr);
int get_packet_type(const char *ptr);
void dump_buf(const char *foo, int direction, int limit, off_t offset);

