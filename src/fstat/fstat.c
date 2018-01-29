#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>


int main(int argc, char *argv[])
{

    static struct stat statbuf;
    int rc = 0;

    if (argc < 2) {
        printf("usage: fstat <filename>\n");
        exit(1);
        }
    printf("filename: %s\n", argv[1]);

    rc = stat(argv[1], (const struct stat *) &statbuf);
    
    printf("rc = %d\n", rc);

    if (rc != 0) {
        printf ("* Couldn't open file '%s' (%s)\n", argv[1], strerror (errno)); 
        exit(1);
        }

    printf("size = %lu bytes (approx)\n", statbuf.st_size);

    return 0;
}
