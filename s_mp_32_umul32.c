#include "tommath_private.h"
#ifdef S_MP_32_UMUL32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

void s_mp_32_umul32(uint32_t a, uint32_t b, uint32_t *high, uint32_t *low)
{
#if ( MP_DIGIT_BIT > 160)
   uint64_t r = (uint64_t)a * b;
   if (high != NULL) {
      *high = r >> 32;
   }
   if (low != NULL) {
      *low = r & 0xFFFFFFFF;
   }
#else
   uint32_t a_low, a_high, b_low, b_high, p0, p1, p2, p3;
   uint32_t p1_low, p1_high, p2_low, p2_high, mid;

   a_low  = a & 0xFFFF;
   a_high = a >> 16;
   b_low  = b & 0xFFFF;
   b_high = b >> 16;

   p0 = a_low * b_low;
   p1 = a_low * b_high;
   p2 = a_high * b_low;
   p3 = a_high * b_high;

   p1_low  = p1 & 0xFFFF;
   p1_high = p1 >> 16;

   p2_low  = p2 & 0xFFFF;
   p2_high = p2 >> 16;

   /* mid includes upper half of part p0 */
   mid = p1_low + p2_low + (p0 >> 16);

   if (high != NULL) {
      *high = p3 + p1_high + p2_high + (mid >> 16);
   }

   if (low != NULL) {
      *low  = (p0 & 0xFFFF) | (mid << 16);
   }
#endif
}


#endif
