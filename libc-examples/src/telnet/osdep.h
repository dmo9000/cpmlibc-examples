int _unix_getchar_noecho();
#ifdef __CPM__
#define _getchar cpm_getchar
#define _getchar_noecho cpm_getchar_noecho
#define _putchar cpm_putchar
#else 
#define _getchar getchar
#define _getchar_noecho _unix_getchar_noecho
#define _putchar putchar
#define cpm_putchar putchar
#define cpm_putchar_noecho putchar_noecho
#define cpm_getchar_noecho getchar_noecho

int tcp_send(int s, const char *b, int l);
int tcp_recv(int s, const char *b, int l);
int tcp_connect(const char *address, uint16_t port);
int tcp_close(int fd);
void show_cursor(bool s);
void set_cursor(int x, int y);
#endif /* __CPM__ */
