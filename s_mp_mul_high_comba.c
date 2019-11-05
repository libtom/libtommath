#include "tommath_private.h"
#ifdef S_MP_MUL_HIGH_COMBA_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* this is a modified version of s_mp_mul_comba that only produces
 * output digits *above* digs.  See the comments for s_mp_mul_comba
 * to see how it works.
 *
 * This is used in the Barrett reduction since for one of the multiplications
 * only the higher digits were needed.  This essentially halves the work.
 *
 * Based on Algorithm 14.12 on pp.595 of HAC.
 */
mp_err s_mp_mul_high_comba(const mp_int *a, const mp_int *b, mp_int *c, int digs)
{
   int     oldused, pa, ix;
   mp_err   err;
   mp_word  W;
   mp_int   tmp, *c_;

   /* prepare the destination */
   pa = a->used + b->used;
   err = (MP_ALIAS(a, c) || MP_ALIAS(b, c))
         ? mp_init_size((c_ = &tmp), pa)
         : mp_grow((c_ = c), pa);
   if (err != MP_OKAY) {
      return err;
   }

   /* number of output digits to produce */
   pa = a->used + b->used;
   W = 0;
   for (ix = digs; ix < pa; ix++) {
      int      tx, ty, iy, iz;

      /* get offsets into the two bignums */
      ty = MP_MIN(b->used-1, ix);
      tx = ix - ty;

      /* this is the number of times the loop will iterrate, essentially its
         while (tx++ < a->used && ty-- >= 0) { ... }
       */
      iy = MP_MIN(a->used-tx, ty+1);

      /* execute loop */
      for (iz = 0; iz < iy; iz++) {
         W += (mp_word)a->dp[tx + iz] * (mp_word)b->dp[ty - iz];
      }

      /* store term */
      c_->dp[ix] = (mp_digit)W & MP_MASK;

      /* make next carry */
      W = W >> (mp_word)MP_DIGIT_BIT;
   }

   /* setup dest */
   oldused  = c_->used;
   c_->used = pa;

   /* clear unused digits [that existed in the old copy of c] */
   s_mp_zero_digs(c_->dp + c_->used, oldused - c_->used);
   mp_clamp(c_);

   if (c_ == &tmp) {
      mp_clear(c);
      *c = *c_;
   }

   return MP_OKAY;
}
#endif
