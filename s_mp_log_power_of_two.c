#include "tommath_private.h"
#ifdef S_MP_LOG_POWER_OF_TWO_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



int s_mp_log_power_of_two(const mp_int *a, int p_of_2)
{
   int x, bit_count;
   for (x = 0; (x < 7) && !((unsigned int)p_of_2 & 1u); x++) {
      p_of_2 >>= 1;
   }
   bit_count = mp_count_bits(a) - 1;
   return (bit_count/x);
}


#endif
