
int tcp_connect(const char *a, uint16_t p);
int tcp_recv(int s, const char *b, uint8_t l);
int tcp_send(int s, const char *b, uint8_t l);
int tcp_close(int s);
