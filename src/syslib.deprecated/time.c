#include <stdint.h>
#include <time.h>


int32_t clock()
{
    /* llclock() returns a 32-bit unsigned integer (?) so we need to cast to int32_t */
    uint32_t wallclocktime = 0;
    wallclocktime = llclock(); 
    return (int32_t) wallclocktime;
}

