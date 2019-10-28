#include "tommath_private.h"
#ifdef S_MP_LOG_POW2_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

size_t s_mp_log_pow2(const mp_int *a, uint32_t base)
{
   unsigned y;
   for (y = 0; (base & 1u) == 0u; y++, base >>= 1) {}
   return (mp_count_bits(a) - 1) / y;
}
#endif
