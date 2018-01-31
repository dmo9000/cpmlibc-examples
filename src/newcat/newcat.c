#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

//#define DEBUG1
//#define DEBUG2

int main(int argc, char *argv[])
{
    char buffer[128];
    struct stat statbuf;
    int catfd = 0;
    int rc = 0;
    int rd = 0;
    int i = 0;
    uint32_t offset = 0;

    //printf("argc = %d\n", argc);

    if (argc != 2) {
        printf("usage: newcat <filename>\n");
        exit(1);
        }

    rc = stat ((const char *) argv[1], &statbuf);

    catfd = open(argv[1], O_RDONLY);

    if (catfd == -1) {
        perror("open");
        exit(1);
        }

    rd = read(catfd, (char *) &buffer, 128);
    while (rd > 0) {                                                                                                
        for (i = 0; i < rd ; i++) {
            putchar(buffer[i]);
            }
#ifdef DEBUG1
				getchar();
#endif /* DEBUG1 */
        offset+=(uint32_t) rd;
#ifdef DEBUG2
        printf("\n[rd = %d, offset = %lu]\n", rd, offset);
#endif /* DEBUG2 */
        rd = read(catfd, (char *) &buffer, 128);
        }

    close(catfd);

    exit(0);
}
