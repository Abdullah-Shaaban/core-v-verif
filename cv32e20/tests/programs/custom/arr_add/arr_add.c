#include <stdio.h>
// #include "coremark.h"

/*
    The current way of performance measurement is adopted from the start_time(), stop_time(), and get_time() 
    functions in this file: /pri/abal/V_Unit/CVE2X/examples/sw/benchmarks/coremark/cve2/core_portme.c
*/


#define PCOUNT_READ(name, dst) asm volatile("csrr %0, " #name ";" : "=r"(dst))
#define PCOUNT_ENABLE(inhibit_val) asm volatile("csrw  0x320, %0\n" : : "r"(inhibit_val))
#define PCOUNT_RESET() asm volatile("csrw mcycle, x0\n")

static unsigned int start_time_val, stop_time_val;
    
 int main()
{
    /* Variables */
    unsigned int N = 10;
    __int16_t A[N];
    __int16_t B[N];
    volatile __int16_t C[N];

    /* Data init */
    for (int i = 0; i < N; i++)
    {
        A[i] = i;
        B[i] = i;
    }

    // Disble, Reset, then Enable performance counter
    PCOUNT_ENABLE(0xFFFFFFFF);
    PCOUNT_RESET();    
    PCOUNT_ENABLE(0x0);
    PCOUNT_READ(mcycle, start_time_val);

    /* Perform benchmark */
    // NOTE: 1 NOP = 4 cycles, 2 NOP = 5 cycles, 3 NOP = 6 cycles -> the performance counting is reliable given that read/write overhead is ignored
    // asm volatile("nop \n" );
    // asm volatile("nop \n" );
    // asm volatile("nop \n" );
    unsigned int i;
    for (i = 0; i < N; i++)
    {
        C[i] = A[i] + B[i];
    }

    // Read performance counter, then disable it
    PCOUNT_READ(mcycle, stop_time_val);
    PCOUNT_ENABLE(0xFFFFFFFF);
    
    /* Report results */
    unsigned int total_time = stop_time_val - start_time_val;
    printf("\n\n *********** Number of Cycles: %u ***********\n\n", total_time);

    return 0;
}
