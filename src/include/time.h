#ifndef __TIME_H__
#define __TIME_H__

typedef uint32_t time_t;
float difftime(time_t time1, time_t time0);
time_t time(time_t *tloc);
int32_t clock();
unsigned int llclock();

#endif /* __TIME_H__ */


