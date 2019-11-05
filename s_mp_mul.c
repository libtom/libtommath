#include "tommath_private.h"
#ifdef S_MP_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* multiplies |a| * |b| and only computes upto digs digits of result
 * HAC pp. 595, Algorithm 14.12  Modified so you can control how
 * many digits of output are created.
 */
mp_err s_mp_mul(const mp_int *a, const mp_int *b, mp_int *c, int digs)
{
   mp_int  tmp, *c_;
   mp_err  err;
   int     pa, ix;

   /* prepare the destination */
   err = (MP_ALIAS(a, c) || MP_ALIAS(b, c))
         ? mp_init_size((c_ = &tmp), digs)
         : mp_grow((c_ = c), digs);
   if (err != MP_OKAY) {
      return err;
   }

   s_mp_zero_digs(c_->dp, c_->used);
   c_->used = digs;

   /* compute the digits of the product directly */
   pa = a->used;
   for (ix = 0; ix < pa; ix++) {
      int iy, pb;
      mp_digit u = 0;

      /* limit ourselves to making digs digits of output */
      pb = MP_MIN(b->used, digs - ix);

      /* compute the columns of the output and propagate the carry */
      for (iy = 0; iy < pb; iy++) {
         /* compute the column as a mp_word */
         mp_word r = (mp_word)c_->dp[ix + iy] +
                     ((mp_word)a->dp[ix] * (mp_word)b->dp[iy]) +
                     (mp_word)u;

         /* the new column is the lower part of the result */
         c_->dp[ix + iy] = (mp_digit)(r & (mp_word)MP_MASK);

         /* get the carry word from the result */
         u       = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);
      }
      /* set carry if it is placed below digs */
      if ((ix + iy) < digs) {
         c_->dp[ix + pb] = u;
      }
   }

   mp_clamp(c_);

   if (c_ == &tmp) {
      mp_clear(c);
      *c = *c_;
   }

   return MP_OKAY;
}
#endif
