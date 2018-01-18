#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpm_sysfunc.h"

extern void llnet(void);

uint8_t net_dmabuffer[128];

#define NET_TCP_NULL        0
#define NET_TCP_CONNECT     1
#define NET_TCP_CLOSE       2
#define NET_TCP_SEND        3
#define NET_TCP_RECV        4
#define NET_TCP_ABORT       5

int tcp_connect(const char *a, uint16_t p)
{
    char *p1 = NULL, *p2= NULL;
    int i = 0;
    uint8_t rc = 0;
    uint8_t o[4];
    int z = 0;

    p1 = a;
    p2 = NULL;
    z = strlen(a);
    for (i = 0; i < 4; i++) {
        switch (i) {
         case 0:
         case 1:
         case 2:
            p2 = memchr(p1, '.', z);
            p2[0] = '\0';
            o[i] = atoi(p1);
            p1 = p2+1;
            break;
        case 3:
            o[i] = atoi(p1);
            break; 
        }
    }

//    printf("\ntcp_connect(%d.%d.%d.%d, %u)\n", o[0], o[1], o[2], o[3], portnum);
    memset(&net_dmabuffer, 0, 128);
    net_dmabuffer[0] = NET_TCP_CONNECT;
    net_dmabuffer[1] = o[0];
    net_dmabuffer[2] = o[1];
    net_dmabuffer[3] = o[2];
    net_dmabuffer[4] = o[3];
    net_dmabuffer[5] = p & 0x00ff;
    net_dmabuffer[6] = (p & 0xff00) >> 8;

    cpm_setDMAAddr((uint16_t) &net_dmabuffer);
    llnet();
    rc = (int8_t) net_dmabuffer[1];
    //printf("net_dmabuffer[1] = 0x%02x -> %d\n", net_dmabuffer[1], rc);
    
    if (rc == 0xFF) {
        return -1;
        }    

    return (int8_t) rc;
}

int tcp_close(int s)
{

    //printf("\ntcp_close(%d)\n", s);
    memset(&net_dmabuffer, 0, 128);
    net_dmabuffer[0] = NET_TCP_CLOSE;
    net_dmabuffer[1] = s;
    cpm_setDMAAddr((uint16_t) &net_dmabuffer);
    llnet();
    return (int8_t) net_dmabuffer[1];
}

int tcp_recv(int s, char *buf, uint8_t len)
{
    uint16_t ptr = (uint16_t) buf;
//    printf("\ntcp_recv(%d, 0x%04x, %u)\n", s, buf, len);
    memset(&net_dmabuffer, 0, 128);
    net_dmabuffer[0] = NET_TCP_RECV;
    net_dmabuffer[1] = s;
    net_dmabuffer[2] = ptr & 0x00ff;
    net_dmabuffer[3] = (ptr & 0xff00) >> 8;
    net_dmabuffer[4] = len;
    cpm_setDMAAddr((uint16_t) &net_dmabuffer);
    llnet();
    return (int8_t) net_dmabuffer[1];
}

int tcp_send(int s, char *buf, uint8_t len)
{
    uint16_t ptr = (uint16_t) buf;
//    printf("\ntcp_recv(%d, 0x%04x, %u)\n", s, buf, len);
    memset(&net_dmabuffer, 0, 128);
    net_dmabuffer[0] = NET_TCP_SEND;
    net_dmabuffer[1] = s;
    net_dmabuffer[2] = ptr & 0x00ff;
    net_dmabuffer[3] = (ptr & 0xff00) >> 8;
    net_dmabuffer[4] = len;
    cpm_setDMAAddr((uint16_t) &net_dmabuffer);
    llnet();
    return (int8_t) net_dmabuffer[1];
}
