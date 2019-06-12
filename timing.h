#ifndef _TIMING_H_
#define _TIMING_H_

#include <opencv/cv.h>

//static void getCPUTick(tick_t *c)
//{
//     tick_t a, d;
//     asm("cpuid");
//     asm volatile("rdtsc" : "=a" (a), "=d" (d));

//     *c = (a | (d << 32));
//}

typedef int64 tick_t;

static __inline tick_t getCPUFreq() {
    return (tick_t) cvGetTickFrequency();
}

static __inline void getCPUTick(tick_t *c)
{
    *c = cvGetTickCount();
}

#define PRINT_TIMING(label, init, final, del) printf("%s: %.3lf%s\n", label, (final-init)/getCPUFreq()/1000.0, (del));
#define GET_COST_TIMING(init, final) ((final-init)/getCPUFreq()/1000.0)
#endif
