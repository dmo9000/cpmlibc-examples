#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;
    int foobar = 0;
    printf("argc = 0x%02x, argv = 0x%04x", argc, argv);
	
		printf(" {");
    for (i = 0 ; i < argc; i++) {
            if (argv[i]) {
                printf("\"%s\"", (const char*) argv[i]);
								if (i < (argc - 1)) {
										printf(", ");
										}
                }
            }
		printf("}\n");
    exit(0);
}
