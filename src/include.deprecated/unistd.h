#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>

#define PATH_MAX 13

typedef uint16_t useconds_t;
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
off_t lseek(int fd, off_t offset, int whence);
unsigned int alarm(unsigned int seconds);
int usleep(useconds_t usec);
void *sbrk(intptr_t increment);
int isatty(int fd);
int unlink(const char *path);

