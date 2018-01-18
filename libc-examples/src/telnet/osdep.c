#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

uint8_t _getchar()
{
    return 0;
}

uint8_t _getchar_noecho()
{
    return 0;
}

int tcp_connect(const char *a, uint16_t p)
{
    int sockfd = -1;
    int newfd = -1;
    int flags = 0;
    struct sockaddr_in my_addr;
    //printf("tcp_connect(%s, %d)\n", a, p);
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_port = htons(p);
    my_addr.sin_addr.s_addr = inet_addr(a);
    my_addr.sin_family = AF_INET;
    memset(&my_addr.sin_zero, 0, sizeof(my_addr.sin_zero));
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd > 0) {
        printf("sockfd = %d\n", sockfd);
        newfd = connect(sockfd, &my_addr, sizeof(my_addr));
        printf("newfd = %d\n", newfd);
        /* set non-blocking IO */
        printf("Non-blocking IO mode set!\n");
        flags = fcntl(sockfd, F_GETFL, 0);
        printf("flags = %u\n");
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK | O_NDELAY)) {
        
                };
        flags = fcntl(sockfd, F_GETFL, 0);
        printf("flags = %u\n");
        } else {
        return -1;
        }
    return(sockfd);
}

int tcp_recv(int s, const char *b, uint8_t l)
{
    int n = 0;
#ifdef __CPM__
    printf("read(%d, 0x%04x, %u)\n", s, b, l);
#else
//    printf("read(%d, 0x%08x, %u)\n", s, b, l);
#endif
    fcntl(s, F_SETFL, O_NONBLOCK);
    n = read(s, (const char *) b, l);

    if (n == -1) {
        if (errno == EAGAIN) {
            /* just say there are no bytes available */
            return 0;
            }
        perror("read");
        exit(1);
        }   
    //printf("read returning %d\n", n);
    //
#ifdef __LINUX__
    printf("read = %d\n", n);
#endif
    return n;
}

int tcp_send(int s, const char *b, uint8_t l)
{
    int n = 0;
    fcntl(s, F_SETFL, O_NONBLOCK); 
    n = write(s, b, l);
    if (n == -1) {
        perror("write");
        exit(1);
        }
    return n;
}

int tcp_close(int s)
{
    return -1;
}


int _putchar(char c)
{

    return putchar(c);
}


void set_cursor(int x, int y)
{

	return;

}

void show_cursor(bool s)
{
	return;
}

void clear_screen()
{
	return;
}


int _unix_getchar_noecho()
{
	return 0;
}


int getchar_noecho()
{
	return 0;
}
