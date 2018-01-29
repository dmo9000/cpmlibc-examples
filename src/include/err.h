#ifndef __ERR_H__
#define __ERR_H__

void err(int eval, const char *fmt, ...);
void errx(int eval, const char *fmt, ...);

//#define err(eval,a,...) 	if (eval) { printf(a, __VA_ARGS__) perror(); }
//#define errx(eval,a,...) 	if (eval) { printf(a, __VA_ARGS__) }

#endif /* __ERR_H__ */
