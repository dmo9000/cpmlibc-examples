#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{

    uint16_t size = 8192;
    void *malltest = NULL;

    /* required to use malloc()  - for now */

    __sdcc_heap_init();

    malltest = malloc(size);

    while (!malltest && size >= 1) {
        printf("malloc(%u) = 0x%04x\n", size, malltest);
        size = size / 2; 
        malltest = malloc(size);
        }

    exit(0);

}




