/*
   Code obtained from https://github.com/Shay-Gueron/AES-GCM-SIV
*/

#ifndef MEASURE_H
#define MEASURE_H

#define RDTSC

#ifndef RDTSC
#define MEASURE(x) (x)
#endif




/* This part defines the functions and MACROS needed to measure using RDTSC */
#ifdef RDTSC

   static __inline__ int64_t rdtsc_s(void)
   {
   unsigned a, d; 
   asm volatile("cpuid" ::: "%rax", "%rbx", "%rcx", "%rdx");
   asm volatile("rdtsc" : "=a" (a), "=d" (d)); 
   return ((unsigned long)a) | (((unsigned long)d) << 32); 
   }

   static __inline__ int64_t rdtsc_e(void)
   {
   unsigned a, d; 
   asm volatile("rdtscp" : "=a" (a), "=d" (d)); 
   asm volatile("cpuid" ::: "%rax", "%rbx", "%rcx", "%rdx");
   return ((unsigned long)a) | (((unsigned long)d) << 32); 
   }

   #ifndef REPEAT     
      #define REPEAT 500
   #endif

   #ifndef OUTER_REPEAT
      #define OUTER_REPEAT 50
   #endif

   #ifndef WARMUP
      #define WARMUP REPEAT/4
   #endif

    unsigned long long RDTSC_start_clk, RDTSC_end_clk;
    double RDTSC_total_clk;
    double RDTSC_TEMP_CLK;
    int RDTSC_MEASURE_ITERATOR;
    int RDTSC_OUTER_ITERATOR;

   /* 
      This MACRO measures the number of cycles "x" runs.
    */
   #define RDTSC_MEASURE(x)                                                                         \
   for(RDTSC_MEASURE_ITERATOR=0; RDTSC_MEASURE_ITERATOR< WARMUP; RDTSC_MEASURE_ITERATOR++)          \
      {                                                                                             \
         {x};                                                                                       \
      }                                                                                             \
    RDTSC_total_clk = 1.7976931348623157e+308;                                                      \
    for(RDTSC_OUTER_ITERATOR=0;RDTSC_OUTER_ITERATOR<OUTER_REPEAT; RDTSC_OUTER_ITERATOR++){          \
      RDTSC_start_clk = rdtsc_s();                                                                 \
      for (RDTSC_MEASURE_ITERATOR = 0; RDTSC_MEASURE_ITERATOR < REPEAT; RDTSC_MEASURE_ITERATOR++)   \
      {                                                                                             \
         {x};                                                                                       \
      }                                                                                             \
      RDTSC_end_clk = rdtsc_e();                                                                   \
      RDTSC_TEMP_CLK = (double)(RDTSC_end_clk-RDTSC_start_clk)/REPEAT;                              \
        if(RDTSC_total_clk>RDTSC_TEMP_CLK) RDTSC_total_clk = RDTSC_TEMP_CLK;                        \
    }


    #define MEASURE(x) RDTSC_MEASURE(x)


#endif

#endif
