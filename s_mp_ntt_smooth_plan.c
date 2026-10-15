#include "tommath_private.h"
#ifdef S_MP_NTT_SMOOTH_PLAN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
   Radices of m in the order the stages apply them: all the 5s, then the 3s,
   then the 2s.  (Any order is correct; this one puts the long-radix work in the
   early stages, where the block is large and the twiddles are few or trivial,
   and leaves the lot of radix-2 stages for last, where a block is two adjacent
   elements.)  Returns the number of stages, or -1 if m is not 5-smooth or needs
   more room than the caller has.
 */
/* TODO: output to given variable and only return errors */
int s_mp_ntt_smooth_plan(int m, int *radix, int max)
{
   int n = 0, t = m;

   if (m < 2) return -1;
   while ((t % 5) == 0) {
      if (n >= max) {
         return -1;
      }
      radix[n++] = 5;
      t /= 5;
   }
   while ((t % 3) == 0) {
      if (n >= max) {
         return -1;
      }
      radix[n++] = 3;
      t /= 3;
   }
   while ((t % 2) == 0) {
      if (n >= max) {
         return -1;
      }
      radix[n++] = 2;
      t /= 2;
   }
   return (t == 1) ? n : -1;
}
#endif


#endif
